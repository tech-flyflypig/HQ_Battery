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
#include "batterylistform.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("HQ_Battery");
    this->initUI();
    this->init_sql();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initUI()
{
    // 创建电池网格组件
    batteryGrid = new BatteryGridWidget(ui->widget_center);

    // 设置网格大小和总电池数量
    batteryGrid->setGridSize(5, 5); // 5行5列
    batteryGrid->setTotalItems(49); // 设置50个电池

    // 设置底部空间和分页控件自动隐藏
    batteryGrid->setBottomMargin(20);  // 设置50像素的底部间距
    batteryGrid->setAutoHidePagination(true);  // 当电池数量不足一页时自动隐藏分页控件

    // 将电池网格添加到 widget_center
    QVBoxLayout *layout = new QVBoxLayout(ui->widget_center);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(batteryGrid);

    // 连接电池选择信号
    connect(batteryGrid, &BatteryGridWidget::batterySelected, this, [this](BatteryListForm * battery)
    {
        // 处理电池选择事件
        qDebug() << "Battery selected";
        
        // 显示选中电池的详细信息
        battery_info info = battery->getBatteryInfo();
        //ui->statusbar->showMessage(QString("选中电池: %1, 位置: %2").arg(info.power_id).arg(info.site));
    });

    // 为每个电池添加右键菜单
    for (BatteryListForm *battery : batteryGrid->getBatteryWidgets())
    {
        // 添加右键菜单
        battery->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(battery, &BatteryListForm::customContextMenuRequested, this, [this, battery](const QPoint &pos)
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
            });
            
            menu.addAction(startAction);
            menu.addAction(stopAction);
            menu.addAction(detailAction);
            
            // 显示菜单
            menu.exec(battery->mapToGlobal(pos));
        });
        
        connectBatterySignals(battery);
    }
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
        QMessageBox::warning(this, "通信错误",
                             QString("电池 %1 通信错误: %2")
                             .arg(battery->getBatteryInfo().site)
                             .arg(error));
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
#if 0
    ui->treeWidget->clear();
    //添加顶层节点
    QTreeWidgetItem *topItem1 = new QTreeWidgetItem(ui->treeWidget, 0);
    topItem1->setText(0, "BMS_1");
    //ui->treeWidget->addTopLevelItem(topItem1);
    QTreeWidgetItem *topItem2 = new QTreeWidgetItem(ui->treeWidget, 0);
    topItem2->setText(0, "BMS_2");
    //ui->treeWidget->addTopLevelItem(topItem2);
    //隐藏表头
    ui->treeWidget->setHeaderHidden(true);
    //    ui->treeWidget->setIconSize(QSize(40, 40));
    //设置展开
    ui->treeWidget->expandAll();
    QSqlQuery query;
    QString sql = "select power_id,type,site,port_name,baud_rate,data_bits,stop_bits,parity,x,y from power_source";
    if(query.exec(sql))
    {
        while (query.next())
        {
            if(query.value(1).toString() == "BMS_1")
            {
                QTreeWidgetItem *item = new QTreeWidgetItem(ui->treeWidget->topLevelItem(0), 1);
                item->setText(0, query.value(2).toString());
                item->setIcon(0, QIcon(":image/BMS_1.png"));
                qDebug() << query.value(0).toString();
                battery_info battery;
                battery.power_id = query.value(0).toString();
                battery.type = query.value(1).toString();
                battery.site = query.value(2).toString();
                battery.port_name = query.value(3).toString();
                battery.baud_rate = query.value(4).toUInt();
                battery.data_bits = query.value(5).toUInt();
                battery.stop_bits = query.value(6).toUInt();
                battery.parity = query.value(7).toUInt();
                if(query.value(8).toUInt() > 0 && query.value(9).toUInt() > 0)
                {
                    QPoint pos = QPoint(query.value(8).toUInt(), query.value(9).toUInt());
                    battery.pos = pos;
                    myApp::back_up_battery.append(battery);
                }
                item->setData(0, Qt::UserRole, QVariant::fromValue(battery));
            }
            if(query.value(1).toString() == "BMS_2")
            {
                QTreeWidgetItem *item = new QTreeWidgetItem(ui->treeWidget->topLevelItem(1));
                item->setText(0, query.value(2).toString());
                item->setIcon(0, QIcon(":image/BMS_2.png"));
                qDebug() << query.value(0).toString();
                battery_info battery;
                battery.power_id = query.value(0).toString();
                battery.type = query.value(1).toString();
                battery.site = query.value(2).toString();
                battery.port_name = query.value(3).toString();
                battery.baud_rate = query.value(4).toUInt();
                battery.data_bits = query.value(5).toUInt();
                battery.stop_bits = query.value(6).toUInt();
                battery.parity = query.value(7).toUInt();
                if(query.value(8).toUInt() > 0 && query.value(9).toUInt() > 0)
                {
                    QPoint pos = QPoint(query.value(8).toUInt(), query.value(9).toUInt());
                    battery.pos = pos;
                    myApp::back_up_battery.append(battery);
                }
                item->setData(0, Qt::UserRole, QVariant::fromValue(battery));
            }
        }
    }
    else
    {
        this->statusBar()->setToolTip("数据库未打开");
    }
#endif
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

