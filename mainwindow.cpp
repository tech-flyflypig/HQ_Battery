#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QSqlQuery>
#include <QDebug>
#include <myapp.h>
#include <QDesktopServices>
#include <QMessageBox>
#include <QProcess>
#include "cfdrecordform.h"
#include "exceptionform.h"
#include "adduserform.h"
#include "queryform.h"
#include <QScrollArea>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QMenu>
#include <QSqlError>
#include <QMouseEvent>
#include <QDateTime>
#include <QTimer>
#include <ExceptionRecordForm.h>
#include <QScreen>
#include <QWindowStateChangeEvent>
#include <QGuiApplication>
#include "batterylistform.h"
#include "component/chargeanddischargerecordform.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_isMoving(false) // 初始化拖拽状态
    , m_currentUser("")
    , m_userPrivilege(9) // 默认为9，即最高权限(管理员)
{
    ui->setupUi(this);
    this->setWindowTitle("HQ_Battery");
    this->setWindowFlags(Qt::FramelessWindowHint);

    // 为标题栏安装事件过滤器
    ui->widget_2->installEventFilter(this);

    // 初始化UI
    this->initUI();

    // 初始化数据库并加载电池信息
    this->init_sql();
}

MainWindow::~MainWindow()
{
    // 停止并清理定时器
    if (m_timeTimer)
    {
        m_timeTimer->stop();
        delete m_timeTimer;
        m_timeTimer = nullptr;
    }

    // 清理资源，确保所有动态创建的对象被正确删除
    if (bms1InfoShowForm)
    {
        // 如果窗口关闭时，详情页面还存在，需要手动删除
        ui->stackedWidget->removeWidget(bms1InfoShowForm);
        delete bms1InfoShowForm;  // 直接删除而不是deleteLater，因为程序即将退出
        bms1InfoShowForm = nullptr;
    }

    // 删除电池网格
    if (batteryGrid)
    {
        delete batteryGrid;
        batteryGrid = nullptr;
    }

    delete ui;
}

void MainWindow::initUI()
{
    //ui->label_company->setVisible(false);

    // 初始化设置菜单
    m_settingsMenu = new QMenu(this);
    // QAction *userSettingsAction = new QAction("用户设置", this);
    QAction *deviceSettingsAction = new QAction("设备设置", this);
    // QAction *systemSettingsAction = new QAction("系统设置", this);
    QAction *userManageAction = new QAction("用户管理", this);

    // m_settingsMenu->addAction(userSettingsAction);
    m_settingsMenu->addAction(deviceSettingsAction);
    // m_settingsMenu->addAction(systemSettingsAction);
    m_settingsMenu->addAction(userManageAction);

    m_historyMenu = new QMenu(this);
    QAction *cfdRecordAction = new QAction("充放电记录", this);
    QAction *abnormalRecordAction = new QAction("异常记录", this);
    m_historyMenu->addAction(cfdRecordAction);
    m_historyMenu->addAction(abnormalRecordAction);

    // 默认情况下，用户管理只对管理员可见
    userManageAction->setVisible(m_userPrivilege == 9);

    // 连接菜单项的信号
    // connect(userSettingsAction, &QAction::triggered, this, [this]()
    // {
    //     // 用户设置功能，后续实现
    //     QMessageBox::information(this, "用户设置", "用户设置功能将在未来版本中实现");
    // });

    connect(deviceSettingsAction, &QAction::triggered, this, [this]()
    {
        // 打开设备管理界面
        device_manage_action();
    });

    // connect(systemSettingsAction, &QAction::triggered, this, [this]()
    // {
    //     QMessageBox::information(this, "系统设置", "系统设置功能将在未来版本中实现");
    // });

    connect(userManageAction, &QAction::triggered, this, &MainWindow::user_manage_action);

    connect(cfdRecordAction, &QAction::triggered, this, &MainWindow::cfd_record_action);
    connect(abnormalRecordAction, &QAction::triggered, this, &MainWindow::abnormal_record_action);

    // 创建电池网格组件
    batteryGrid = new BatteryGridWidget(ui->widget_center);

    // 设置网格大小，调整为合适的行列数
    batteryGrid->setGridSize(7, 6); // 初始设置

    // 设置底部空间和分页控件自动隐藏
    batteryGrid->setBottomMargin(30);  // 设置30像素的底部间距
    batteryGrid->setAutoHidePagination(true);  // 当电池数量不足一页时自动隐藏分页控件

    // 将电池网格添加到 widget_center，设置合适的边距让组件不会占满整个区域
    QVBoxLayout *layout = new QVBoxLayout(ui->widget_center);
    layout->setContentsMargins(20, 20, 20, 20); // 初始设置上下左右的边距
    layout->addWidget(batteryGrid, 0, Qt::AlignLeft | Qt::AlignTop); // 设置左上角对齐
    layout->addStretch(); // 添加弹性空间，确保组件靠上靠左

    // 创建返回按钮，放在与Logo相同的位置
    m_backButton = new QPushButton(ui->widget_2);
    m_backButton->setStyleSheet("QPushButton { border-image: url(:/image/undo.png);}");
    m_backButton->setFixedSize(51, 51); // 与logo相同的尺寸

    // 初始显示Logo，隐藏返回按钮
    m_backButton->hide();
    ui->label_logo->show();

    // 将返回按钮添加到布局中，替换logo的位置
    QHBoxLayout *hlayout = qobject_cast<QHBoxLayout *>(ui->widget_2->layout());
    if (hlayout)
    {
        // 在布局的第一个位置插入返回按钮（与logo相同位置）
        hlayout->insertWidget(0, m_backButton);
        m_backButton->raise(); // 确保返回按钮在最前面
    }

    // 连接返回按钮信号
    connect(m_backButton, &QPushButton::clicked, this, &MainWindow::onBackButtonClicked);

    // 初始化指针
    bms1InfoShowForm = nullptr;

    // 连接电池选择信号
    connect(batteryGrid, &BatteryGridWidget::batterySelected, this, &MainWindow::updateRightPanel);

    // 连接电池双击信号
    connect(batteryGrid, &BatteryGridWidget::batteryDoubleClicked, this, [this](BatteryListForm * battery)
    {
        qDebug() << "Battery double clicked";

        // 检查电池状态，只有运行状态才允许进入详情界面
        if (battery->getMonitoringStatus() != BatteryListForm::Running)
        {
            qDebug() << "电池不在运行状态，无法查看详情";
            QMessageBox::information(this, "提示", "只有运行状态的电池才能查看详情！");
            return;
        }

        // 如果已有实例，先删除
        if (bms1InfoShowForm)
        {
            ui->stackedWidget->removeWidget(bms1InfoShowForm);
            delete bms1InfoShowForm;
        }

        // 创建新的电池详情页面
        bms1InfoShowForm = new BMS1InfoShowForm();
        int detailIndex = ui->stackedWidget->addWidget(bms1InfoShowForm);

        // 设置电池详情
        try
        {
            // 使用智能指针安全地设置电池信息
            bms1InfoShowForm->setBatteryInfo(battery);

            // 切换到详情页
            ui->stackedWidget->setCurrentIndex(detailIndex);

            // 显示返回按钮，隐藏logo
            updateWidget2Content(true);
        }
        catch (const std::exception &e)
        {
            qDebug() << "设置电池详情时发生异常: " << e.what();
        }
    });

    // 初始化时间显示
    m_timeTimer = new QTimer(this);
    connect(m_timeTimer, &QTimer::timeout, this, &MainWindow::updateCurrentTime);
    m_timeTimer->start(1000); // 每秒更新一次

    // 初始更新一次时间
    updateCurrentTime();

    // 初始化上边距
    updateTopMargin();

    m_initialWindowSize = this->size();
    qDebug() << "m_initialWindowSize" << m_initialWindowSize;
}

void MainWindow::connectBatterySignals(BatteryListForm *battery)
{
    // 尝试获取shared_ptr
    try
    {
        auto sharedBattery = battery->getSharedPtr();

        // 连接数据接收信号
        connect(battery, &BatteryListForm::dataReceived, this, [this, weakBattery = std::weak_ptr<BatteryListForm>(sharedBattery)](BatteryListForm * battery, const BMS_1 & data)
        {
            // 安全检查
            auto currentBattery = weakBattery.lock();
            if (!currentBattery || currentBattery.get() != battery) return;

            // 更新状态栏或处理数据
            //ui->statusbar->showMessage(QString("电池: %1, SOC: %2%, 温度: %3°C")
            // .arg(battery->getBatteryInfo().site)
            // .arg(data.soc)
            // .arg(data.tempMax / 10.0));
        });

        // 连接通信错误信号
        connect(battery, &BatteryListForm::communicationError, this, [this, weakBattery = std::weak_ptr<BatteryListForm>(sharedBattery)](BatteryListForm * battery, const QString & error)
        {
            // 安全检查
            auto currentBattery = weakBattery.lock();
            if (!currentBattery || currentBattery.get() != battery) return;

            // 显示错误信息
            // QMessageBox::warning(this, "通信错误",
            //                      QString("电池 %1 通信错误: %2")
            //                      .arg(battery->getBatteryInfo().site)
            //                      .arg(error));
            qDebug() << QString("电池 %1 通信错误: %2") .arg(battery->getBatteryInfo().site).arg(error);
        });

        // 连接通信超时信号
        connect(battery, &BatteryListForm::communicationTimeout, this, [this, weakBattery = std::weak_ptr<BatteryListForm>(sharedBattery)](BatteryListForm * battery)
        {
            // 安全检查
            auto currentBattery = weakBattery.lock();
            if (!currentBattery || currentBattery.get() != battery) return;

            // 显示超时信息
            // ui->statusbar->showMessage(QString("电池: %1 通信超时")
            //                            .arg(battery->getBatteryInfo().site));
        });
    }
    catch (const std::bad_weak_ptr &e)
    {
        qDebug() << "警告: 无法获取电池对象的shared_ptr: " << e.what();
    }
}

void MainWindow::init_sql()
{
    // 从数据库中获取电池信息
    QSqlQuery query;
    QString sql = "select power_id, type, site, port_name, baud_rate, data_bits, stop_bits, parity, x, y from power_source";

    if(query.exec(sql))
    {
        // 清除全局存储的电池备份信息
        myApp::back_up_battery.clear();

        // 用于临时存储从数据库中读取的所有电池信息
        QList<battery_info> batteryList;

        while (query.next())
        {
            // 创建电池信息对象
            battery_info battery;
            battery.power_id = query.value(0).toString();
            battery.type = query.value(1).toString();
            battery.site = query.value(2).toString();
            battery.port_name = query.value(3).toString();
            battery.baud_rate = query.value(4).toUInt();
            battery.data_bits = query.value(5).toUInt();
            battery.stop_bits = query.value(6).toUInt();
            battery.parity = query.value(7).toUInt();

            // 获取电池的坐标位置
            battery.x = query.value(8).toUInt();
            battery.y = query.value(9).toUInt();

            if(battery.x > 0 && battery.y > 0)
            {
                QPoint pos = QPoint(battery.x, battery.y);
                battery.pos = pos;
            }

            // 设置默认的标称参数值
            battery.bcdy = 60.0;    // 标称电压默认值
            battery.bcdl = 60.0;    // 标称电流默认值
            battery.bcwd = 50.0;    // 标称温度默认值
            battery.bcsysj = 2500.0;  // 标称使用时间默认值

            // 将电池信息添加到列表
            batteryList.append(battery);

            // 添加到全局备份
            myApp::back_up_battery.append(battery);
        }

        // 设置电池网格的总数量，根据数据库中读取到的电池数量来设置
        batteryGrid->setTotalItems(batteryList.size());

        // 更新每个电池控件的信息
        QList<BatteryListForm *> batteryWidgets = batteryGrid->getBatteryWidgets();
        for (int i = 0; i < batteryList.size() && i < batteryWidgets.size(); i++)
        {
            batteryWidgets[i]->setBatteryInfo(batteryList[i]);
        }

        // 为每个电池添加右键菜单，在这里添加而不是在initUI中，这样只为实际从数据库读取的电池添加
        for (int i = 0; i < batteryList.size() && i < batteryWidgets.size(); i++)
        {
            BatteryListForm *battery = batteryWidgets[i];

            // 添加右键菜单
            battery->setContextMenuPolicy(Qt::CustomContextMenu);
            connect(battery, &BatteryListForm::customContextMenuRequested, this, [this, battery](const QPoint & pos)
            {
                // 创建右键菜单
                QMenu menu(this);
                QAction *startAction = new QAction("开始监控", this);
                QAction *stopAction = new QAction("停止监控", this);
                QAction *detailAction = new QAction("详细信息", this);

                // 连接菜单动作
                connect(startAction, &QAction::triggered, this, [battery]()
                {
                    battery->startCommunication();
                });

                connect(stopAction, &QAction::triggered, this, [battery]()
                {
                    battery->stopCommunication();
                });

                connect(detailAction, &QAction::triggered, this, [this, battery]()
                {
                    // 检查电池状态，只有运行状态才允许进入详情界面
                    if (battery->getMonitoringStatus() != BatteryListForm::Running)
                    {
                        qDebug() << "电池不在运行状态，无法查看详情";
                        QMessageBox::information(this, "提示", "只有运行状态的电池才能查看详情！");
                        return;
                    }

                    // 如果已有实例，先删除
                    if (bms1InfoShowForm)
                    {
                        ui->stackedWidget->removeWidget(bms1InfoShowForm);
                        delete bms1InfoShowForm;
                        bms1InfoShowForm = nullptr;
                    }

                    // 创建新的电池详情页面
                    bms1InfoShowForm = new BMS1InfoShowForm();
                    int detailIndex = ui->stackedWidget->addWidget(bms1InfoShowForm);

                    try
                    {
                        // 设置电池详情
                        bms1InfoShowForm->setBatteryInfo(battery);

                        // 切换到详情页
                        ui->stackedWidget->setCurrentIndex(detailIndex);

                        // 显示返回按钮，隐藏logo
                        updateWidget2Content(true);
                    }
                    catch (const std::exception &e)
                    {
                        qDebug() << "设置电池详情时发生异常: " << e.what();
                    }
                });

                menu.addAction(startAction);
                menu.addAction(stopAction);
                menu.addAction(detailAction);

                // 显示菜单
                menu.exec(battery->mapToGlobal(pos));
            });

            connectBatterySignals(battery);
        }

        // 刷新布局
        batteryGrid->refreshLayout();
        //xxx 暂时先不选择电池，先不更新右侧信息面板
        // 默认选中第一个电池并更新右侧信息面板（如果有电池）
        // if (!batteryWidgets.isEmpty())
        // {
        //     BatteryListForm *firstBattery = batteryWidgets.first();
        //     firstBattery->setSelected(true);

        //     // 更新右侧面板信息
        //     updateRightPanel(firstBattery);
        // }
    }
    else
    {
        qDebug() << "读取数据库失败: " << query.lastError().text();
        qDebug() << "数据库读取错误";
    }
}

void MainWindow::cfd_record_action()
{
    // CfdRecordForm *cfd_form = new CfdRecordForm();
    // cfd_form->show();
    qDebug() << "cfd_record_action";
    // 检查是否已存在充放电记录表单
    ChargeAndDischargeRecordForm *existingForm = nullptr;
    for (int i = 0; i < ui->stackedWidget->count(); i++)
    {
        existingForm = qobject_cast<ChargeAndDischargeRecordForm *>(ui->stackedWidget->widget(i));
        if (existingForm)
        {
            break;
        }
    }

    // 如果不存在则创建新的
    if (!existingForm)
    {
        existingForm = new ChargeAndDischargeRecordForm();
        ui->stackedWidget->addWidget(existingForm);
    }

    // 切换到该界面
    ui->stackedWidget->setCurrentWidget(existingForm);

    // 显示返回按钮，隐藏logo
    updateWidget2Content(true);
}

void MainWindow::abnormal_record_action()
{
    // ExceptionForm *exceptionform = new ExceptionForm();
    // exceptionform->show();
    qDebug() << "abnormal_record_action";
    // 检查是否已存在异常记录表单
    ExceptionRecordForm *existingForm = nullptr;
    for (int i = 0; i < ui->stackedWidget->count(); i++)
    {
        existingForm = qobject_cast<ExceptionRecordForm *>(ui->stackedWidget->widget(i));
        if (existingForm)
        {
            break;
        }
    }

    // 如果不存在则创建新的
    if (!existingForm)
    {
        existingForm = new ExceptionRecordForm();
        ui->stackedWidget->addWidget(existingForm);
    }

    // 切换到该界面
    ui->stackedWidget->setCurrentWidget(existingForm);

    // 显示返回按钮，隐藏logo
    updateWidget2Content(true);
}

void MainWindow::device_manage_action()
{
    QueryForm *queryform = new QueryForm();
    connect(queryform, &QueryForm::init_sql, this, &MainWindow::init_sql);
    queryform->show();
}

void MainWindow::user_manage_action()
{
    AddUserForm *userform = new AddUserForm();
    userform->show();
}


void MainWindow::updateWidget2Content(bool showBackButton)
{
    if (showBackButton)
    {
        // 显示返回按钮，隐藏Logo
        m_backButton->show();
        ui->label_logo->hide();
    }
    else
    {
        // 隐藏返回按钮，显示Logo
        m_backButton->hide();
        ui->label_logo->show();
    }
}

void MainWindow::onBackButtonClicked()
{
    // 获取当前显示的widget
    QWidget *currentWidget = ui->stackedWidget->currentWidget();

    // 先切换到主页面，防止stackedWidget继续引用即将删除的组件
    ui->stackedWidget->setCurrentIndex(0);

    // 从stackedWidget中移除详情页面
    if (bms1InfoShowForm && bms1InfoShowForm == currentWidget)
    {
        ui->stackedWidget->removeWidget(bms1InfoShowForm);

        // 标记为删除
        bms1InfoShowForm->deleteLater();
        bms1InfoShowForm = nullptr;
    }
    // 检查当前widget是否为ChargeAndDischargeRecordForm
    else if (qobject_cast<ChargeAndDischargeRecordForm *>(currentWidget))
    {
        ui->stackedWidget->removeWidget(currentWidget);
        currentWidget->deleteLater();
    }

    // 更新widget_2，隐藏返回按钮
    updateWidget2Content(false);
}

void MainWindow::switchToMainView()
{
    // 切换到主页
    ui->stackedWidget->setCurrentIndex(0);

    // 更新widget_2，隐藏返回按钮
    updateWidget2Content(false);
}


void MainWindow::on_btn_max_clicked(bool checked)
{
    if(checked)
    {
        qDebug() << "Maximizing window";
        // 对于无边框窗口，需要手动设置最大化状态
        this->setWindowState(Qt::WindowMaximized);
        // 强制更新窗口状态
        this->show();

        // 延迟更新布局，确保窗口状态已经生效
        QTimer::singleShot(50, this, [this]()
        {
            updateTopMargin();
        });
    }
    else
    {
        qDebug() << "Restoring window to normal state";
        // 对于无边框窗口，需要手动设置正常状态
        this->setWindowState(Qt::WindowNoState);

        // 恢复到初始窗口大小
        this->resize(m_initialWindowSize);

        // 强制更新窗口状态
        this->show();

        // 延迟更新布局，确保窗口状态已经生效
        QTimer::singleShot(50, this, [this]()
        {
            updateTopMargin();
        });
    }
}


void MainWindow::on_btn_min_clicked()
{
    this->showMinimized();
}

void MainWindow::on_btn_close_clicked()
{
    this->close();
}

void MainWindow::updateRightPanel(BatteryListForm *battery)
{
    if (!battery) return;

    try
    {
        // 处理电池选择事件
        qDebug() << "Battery selected, updating right panel";

        // 获取电池信息
        battery_info info = battery->getBatteryInfo();
        qDebug() << "Selected battery - ID:" << info.power_id << ", 位置:" << info.site;

        // 更新右侧信息面板显示
        ui->widget_right->setBatteryInfo(battery);
    }
    catch (const std::exception &e)
    {
        qDebug() << "更新右侧信息面板时发生异常: " << e.what();
    }
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    // 检查事件是否发生在标题栏组件上
    if (watched == ui->widget_2)
    {
        // 处理鼠标按下事件
        if (event->type() == QEvent::MouseButtonPress)
        {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
            if (mouseEvent->button() == Qt::LeftButton)
            {
                // 获取窗口左上角坐标与鼠标当前位置之间的偏移
                m_lastPos = mouseEvent->globalPos() - this->frameGeometry().topLeft();
                m_isMoving = true;
                return true;  // 事件已处理
            }
        }
        // 处理鼠标移动事件
        else if (event->type() == QEvent::MouseMove)
        {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
            if (m_isMoving && (mouseEvent->buttons() & Qt::LeftButton))
            {
                // 如果当前是最大化状态，退出最大化
                if (this->isMaximized() || this->windowState() == Qt::WindowMaximized || ui->btn_max->isChecked())
                {
                    qDebug() << "Exiting maximized state due to drag";

                    // 计算鼠标在窗口中的相对位置比例
                    QPoint globalMousePos = mouseEvent->globalPos();
                    QRect screenGeometry = this->frameGeometry();
                    double relativeX = (double)(globalMousePos.x() - screenGeometry.left()) / screenGeometry.width();

                    // 设置窗口为正常状态
                    ui->btn_max->setChecked(false);
                    this->setWindowState(Qt::WindowNoState);

                    // 恢复到初始窗口大小
                    this->resize(m_initialWindowSize);

                    // 根据鼠标相对位置重新计算窗口位置，确保鼠标仍在标题栏区域
                    int newX = globalMousePos.x() - (int)(m_initialWindowSize.width() * relativeX);
                    int newY = globalMousePos.y() - m_lastPos.y();

                    // 确保窗口不会移出屏幕
                    QScreen *screen = QGuiApplication::primaryScreen();
                    if (screen)
                    {
                        QRect screenRect = screen->availableGeometry();
                        newX = qMax(0, qMin(newX, screenRect.width() - m_initialWindowSize.width()));
                        newY = qMax(0, qMin(newY, screenRect.height() - m_initialWindowSize.height()));
                    }

                    this->move(newX, newY);

                    // 更新布局
                    updateTopMargin();

                    // 更新拖拽偏移量，基于新的窗口位置
                    m_lastPos = globalMousePos - this->frameGeometry().topLeft();
                }
                else
                {
                    // 正常拖拽移动
                    this->move(mouseEvent->globalPos() - m_lastPos);
                }
                return true;  // 事件已处理
            }
        }
        // 处理鼠标释放事件
        else if (event->type() == QEvent::MouseButtonRelease)
        {
            m_isMoving = false;
            return true;  // 事件已处理
        }
    }

    // 默认的事件处理
    return QMainWindow::eventFilter(watched, event);
}

void MainWindow::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::WindowStateChange)
    {
        QWindowStateChangeEvent *stateChangeEvent = static_cast<QWindowStateChangeEvent *>(event);
        Qt::WindowStates oldState = stateChangeEvent->oldState();
        Qt::WindowStates newState = this->windowState();

        qDebug() << "Window state changed from" << oldState << "to" << newState;

        // 确保按钮状态与窗口状态同步
        bool isMaximized = (newState & Qt::WindowMaximized) != 0;
        if (ui->btn_max->isChecked() != isMaximized)
        {
            ui->btn_max->setChecked(isMaximized);
        }

        // 延迟更新布局，确保窗口状态变化完成
        QTimer::singleShot(100, this, [this]()
        {
            updateTopMargin();
        });
    }

    QMainWindow::changeEvent(event);
}



void MainWindow::updateCurrentTime()
{
    QDateTime currentTime = QDateTime::currentDateTime();
    QString timeString = currentTime.toString("yyyy-MM-dd hh:mm:ss");
    ui->label_current_time->setText(timeString);
}

void MainWindow::on_btn_history_clicked()
{
    qDebug() << "on_btn_history_clicked";

    QPoint pos = ui->btn_history->mapToGlobal(QPoint(0, ui->btn_history->height()));
    m_historyMenu->popup(pos);


}

void MainWindow::on_btn_settings_clicked()
{
    // 在设置按钮旁边显示菜单
    QPoint pos = ui->btn_settings->mapToGlobal(QPoint(0, ui->btn_settings->height()));
    m_settingsMenu->popup(pos);
}

void MainWindow::setUserPrivilege(int privilege)
{
    m_userPrivilege = privilege;

    // 根据权限级别设置界面元素的可见性或启用状态
    // 例如：管理员可以看到所有功能，普通用户只能看到部分功能
    if (m_settingsMenu)
    {
        QList<QAction *> actions = m_settingsMenu->actions();

        // 假设actions[3]是"用户管理"选项，只有管理员(privilege=0)可见
        if (actions.size() > 3)
        {
            actions[3]->setVisible(privilege == 9);
        }
    }

    // 更新界面显示的用户名和权限
    if (ui->label_current_user)
    {
        QString privilegeText = (privilege == 9) ? "管理员" : "普通用户";
        ui->label_current_user->setText(m_currentUser + " (" + privilegeText + ")");
    }
}

void MainWindow::setCurrentUser(const QString &username)
{
    m_currentUser = username;

    // 更新界面显示
    if (ui->label_current_user)
    {
        QString privilegeText = (m_userPrivilege == 9) ? "管理员" : "普通用户";
        ui->label_current_user->setText(m_currentUser + " (" + privilegeText + ")");
    }
}

int MainWindow::getUserPrivilege() const
{
    return m_userPrivilege;
}

QString MainWindow::getCurrentUser() const
{
    return m_currentUser;
}

void MainWindow::updateTopMargin()
{
    // 获取widget_center的布局
    QVBoxLayout *layout = qobject_cast<QVBoxLayout *>(ui->widget_center->layout());
    if (!layout) return;

    // 根据窗口状态和大小动态调整上边距和网格大小
    int topMargin = 20;
    int gridCols = 7;  // 默认列数（与初始化时保持一致）
    int gridRows = 6;  // 默认行数（与初始化时保持一致）

    if (this->isMaximized())
    {
        // 最大化状态：根据屏幕分辨率动态调整
        QScreen *screen = QGuiApplication::primaryScreen();
        if (screen)
        {
            QSize screenSize = screen->size();
            int height = screenSize.height();
            int width = screenSize.width();

            // 分辨率越高，上边距越大
            if (height >= 1440)
            {
                topMargin = 40;
                // 高分辨率屏幕，可以显示更多列
                if (width >= 2560)  // 2K及以上
                {
                    gridCols = 10;
                    gridRows = 8;
                }
                else
                {
                    gridCols = 8;
                    gridRows = 7;
                }
            }
            else if (height >= 1080)
            {
                topMargin = 30;
                // 1080p屏幕
                if (width >= 1920)  // 宽屏
                {
                    gridCols = 8;
                    gridRows = 6;
                }
                else
                {
                    gridCols = 7;
                    gridRows = 6;
                }
            }
            else
            {
                topMargin = 20;
                gridCols = 6;
                gridRows = 5;
            }
        }
    }
    else
    {
        // 正常状态：根据当前窗口大小调整
        QSize currentSize = this->size();
        int windowWidth = currentSize.width();
        int windowHeight = currentSize.height();

        // 基于窗口大小计算合适的网格尺寸
        // 假设每个电池项目大小约为 120x100 像素，加上间距
        int availableWidth = windowWidth - 40;  // 减去左右边距
        int availableHeight = windowHeight - 200; // 减去标题栏、状态栏等

        // 计算可容纳的列数和行数
        gridCols = qMax(3, qMin(10, availableWidth / 140));  // 最少3列，最多10列
        gridRows = qMax(3, qMin(8, availableHeight / 120));   // 最少3行，最多8行

        // 根据窗口大小调整上边距
        if (windowHeight >= 800)
        {
            topMargin = 30;
        }
        else if (windowHeight >= 600)
        {
            topMargin = 20;
        }
        else
        {
            topMargin = 10;
        }
    }

    // qDebug() << "updateTopMargin - isMaximized:" << this->isMaximized()
    //          << "topMargin:" << topMargin
    //          << "gridCols:" << gridCols
    //          << "gridRows:" << gridRows
    //          << "windowSize:" << this->size();

    // 更新布局的上边距
    layout->setContentsMargins(20, topMargin, 20, 20);

    // 更新网格大小
    if (batteryGrid)
    {
        batteryGrid->setGridSize(gridCols, gridRows);
        batteryGrid->refreshLayout();
    }
}
