#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QSqlQuery>
#include <QDebug>
#include <myapp.h>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("HQ_Battery");
    this->init_sql();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::init_sql()
{
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
}
