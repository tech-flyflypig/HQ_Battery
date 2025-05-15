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
#include "batterylistform.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_isMoving(false) // 初始化拖拽状态
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
    delete ui;
}

void MainWindow::initUI()
{
    //ui->label_company->setVisible(false);
    // 创建电池网格组件
    batteryGrid = new BatteryGridWidget(ui->widget_center);

    // 设置网格大小，调整为合适的行列数
    batteryGrid->setGridSize(7, 6); // 每页最多4行4列

    // 设置底部空间和分页控件自动隐藏
    batteryGrid->setBottomMargin(30);  // 设置30像素的底部间距
    batteryGrid->setAutoHidePagination(true);  // 当电池数量不足一页时自动隐藏分页控件

    // 将电池网格添加到 widget_center，设置合适的边距让组件不会占满整个区域
    QVBoxLayout *layout = new QVBoxLayout(ui->widget_center);
    layout->setContentsMargins(20, 20, 20, 20); // 设置上下左右的边距
    layout->addWidget(batteryGrid, 0, Qt::AlignLeft | Qt::AlignTop); // 设置左上角对齐
    layout->addStretch(); // 添加弹性空间，确保组件靠上靠左

    // 创建返回按钮，放在与Logo相同的位置
    m_backButton = new QPushButton(ui->widget_2);
    m_backButton->setStyleSheet("QPushButton { border-image: url(:/image/undo.png);}");
    m_backButton->setFixedSize(60, 30);

    // 初始显示Logo，隐藏返回按钮
    m_backButton->hide();
    ui->label_logo->show();

    // 找到widget_2当前的布局并添加返回按钮
    // 注意：返回按钮和Logo占用相同位置
    if (ui->widget_2->layout())
    {
        // 已有布局，在原布局中添加返回按钮
        ui->widget_2->layout()->addWidget(m_backButton);
    }
    else
    {
        // 无布局，创建新布局
        QHBoxLayout *headerLayout = new QHBoxLayout(ui->widget_2);
        headerLayout->setContentsMargins(10, 5, 10, 5);
        headerLayout->addWidget(m_backButton);
        headerLayout->addStretch();
    }

    // 确保返回按钮与Logo对齐
    QRect logoRect = ui->label_logo->geometry();
    m_backButton->setGeometry(logoRect);

    // 连接返回按钮信号
    connect(m_backButton, &QPushButton::clicked, this, &MainWindow::onBackButtonClicked);

    // 创建电池详情页面
    bms1InfoShowForm = new BMS1InfoShowForm();
    ui->stackedWidget->addWidget(bms1InfoShowForm);

    // 连接电池选择信号
    connect(batteryGrid, &BatteryGridWidget::batterySelected, this, &MainWindow::updateRightPanel);

    // 连接电池双击信号
    connect(batteryGrid, &BatteryGridWidget::batteryDoubleClicked, this, [this](BatteryListForm * battery)
    {
        qDebug() << "Battery double clicked";

        // 设置电池详情
        bms1InfoShowForm->setBatteryInfo(battery);

        // 切换到详情页
        ui->stackedWidget->setCurrentIndex(1);  // 详情页索引为1

        // 显示返回按钮，隐藏logo
        updateWidget2Content(true);
    });
}

void MainWindow::connectBatterySignals(BatteryListForm *battery)
{
    // 连接数据接收信号
    connect(battery, &BatteryListForm::dataReceived, this, [this](BatteryListForm * battery, const BMS_1 & data)
    {
        // 更新状态栏或处理数据
        //ui->statusbar->showMessage(QString("电池: %1, SOC: %2%, 温度: %3°C")
        // .arg(battery->getBatteryInfo().site)
        // .arg(data.soc)
        // .arg(data.tempMax / 10.0));
    });

    // 连接通信错误信号
    connect(battery, &BatteryListForm::communicationError, this, [this](BatteryListForm * battery, const QString & error)
    {
        // 显示错误信息
        // QMessageBox::warning(this, "通信错误",
        //                      QString("电池 %1 通信错误: %2")
        //                      .arg(battery->getBatteryInfo().site)
        //                      .arg(error));
        qDebug() << QString("电池 %1 通信错误: %2") .arg(battery->getBatteryInfo().site).arg(error);
    });

    // 连接通信超时信号
    connect(battery, &BatteryListForm::communicationTimeout, this, [this](BatteryListForm * battery)
    {
        // 显示超时信息
        // ui->statusbar->showMessage(QString("电池: %1 通信超时")
        //                            .arg(battery->getBatteryInfo().site));
    });
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
                    // 显示详细信息表单
                    // TODO: 实现详细信息显示
                    // 设置电池详情
                    bms1InfoShowForm->setBatteryInfo(battery);

                    // 切换到详情页
                    ui->stackedWidget->setCurrentIndex(1);  // 详情页索引为1

                    // 显示返回按钮，隐藏logo
                    updateWidget2Content(true);
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
    CfdRecordForm *cfd_form = new CfdRecordForm();
    cfd_form->show();
}

void MainWindow::abnormal_record_action()
{
    ExceptionForm *exceptionform = new ExceptionForm();
    exceptionform->show();
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

void MainWindow::on_btn_menu_clicked()
{
#if 0
    menu_list->move(QCursor::pos());
    menu_list->show();
#endif
}


void MainWindow::on_btn_about_clicked()
{
#if 0
    menu_about->move(QCursor::pos());
    menu_about->show();
#endif
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
    // 调用BMS1InfoShowForm的触发返回方法，确保数据正确清理
    //bms1InfoShowForm->triggerBackToMain();
    bms1InfoShowForm->deleteLater();
    bms1InfoShowForm = nullptr;
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
        this->showFullScreen();
        ui->btn_max->setStyleSheet("QPushButton{ border-image: url(:/image/normal.png);}"
                                   "QPushButton:hover{border-image: url(:/image/normal_hover.png);}");

    }
    else
    {
        this->showNormal();
        ui->btn_max->setStyleSheet("QPushButton{ border-image: url(:/image/max.png);}"
                                   "QPushButton:hover{border-image: url(:/image/max_hover.png);}");
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

    // 处理电池选择事件
    qDebug() << "Battery selected, updating right panel";

    // 获取电池信息
    battery_info info = battery->getBatteryInfo();
    qDebug() << "Selected battery - ID:" << info.power_id << ", 位置:" << info.site;

    // 更新右侧信息面板显示
    ui->widget_right->setBatteryInfo(battery);
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    // 检查事件是否发生在标题栏组件上
    if (watched == ui->widget_2)
    {
        // 处理鼠标按下事件
        if (event->type() == QEvent::MouseButtonPress)
        {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
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
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            if (m_isMoving && (mouseEvent->buttons() & Qt::LeftButton))
            {
                this->move(mouseEvent->globalPos() - m_lastPos);
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

