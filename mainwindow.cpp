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
#include "component/BlueGlowWidget.h"
#include <QScrollArea>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
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
        // 这里可以添加其他处理逻辑
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

