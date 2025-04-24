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
#if 0
    QMenu *data_manage = new QMenu("数据管理", this);
    QAction *cfd_record = new QAction("充放电记录", this);
    QAction *abnormal_record = new QAction("异常记录", this);
    data_manage->addAction(cfd_record);
    data_manage->addAction(abnormal_record);
    connect(cfd_record, &QAction::triggered, this, &MainWindow::cfd_record_action);
    connect(abnormal_record, &QAction::triggered, this, &MainWindow::abnormal_record_action);

    QMenu *system_manage = new QMenu("系统管理", this);
    QAction *device_manage = new QAction("设备管理", this);
    QAction *user_manage = new QAction("用户管理", this);

    QAction *user_book = new QAction("用户手册", this);
    connect(user_book, &QAction::triggered, []
    {
        QString pdfFile = QApplication::applicationDirPath() + "/后备电池电量管理软件客户端-操作手册-V2.0.pdf";
        QDesktopServices::openUrl(QUrl::fromLocalFile(pdfFile));
    });
    system_manage->addAction(device_manage);
    system_manage->addAction(user_manage);

    connect(device_manage, &QAction::triggered, this, &MainWindow::device_manage_action);
    connect(user_manage, &QAction::triggered, this, &MainWindow::user_manage_action);
    menu_list = new QMenu();
    menu_list->addMenu(data_manage);
    menu_list->addMenu(system_manage);
    menu_list->addAction(user_book);

    menu_about = new QMenu();
    QString version = QApplication::applicationVersion();
    version = "版本号 " + version.mid(0, version.lastIndexOf('.'));
    menu_about->addSeparator();
    menu_about->addAction(version);
    menu_about->addAction("检查更新", [ = ]
    {
        if(QMessageBox::question(nullptr, "请求", "升级时需关闭软件，是否立即关闭软件？") == QMessageBox::Yes)
        {
            QProcess process;
            QString str = "\"" + QApplication::applicationDirPath() + "/updater.exe" + "\"";
            process.startDetached(str);
            this->close();
        }
    });

    // 创建蓝色发光组件示例
    BlueGlowWidget *paramWidget = new BlueGlowWidget(this);
    paramWidget->setTitleText("保存参数");

    // 添加单选按钮组
    QStringList settingOptions = {"过充电压", "过放电压", "输出电源", "内部温度"};
    paramWidget->addRadioGroup("", settingOptions);

    QStringList moreOptions = {"环境温度", "均衡管理器"};
    paramWidget->addRadioGroup("", moreOptions);

    // 将组件添加到UI中的某个位置
    // 例如，如果你有一个名为parameterContainer的QWidget容器
    // ui->parameterContainer->layout()->addWidget(paramWidget);

    // 或者，如果你想添加到主布局中
    // centralWidget()->layout()->addWidget(paramWidget);
#endif
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

