#include "layoutbatterywidget.h"
#include <QDragEnterEvent>
#include <QMimeData>
#include <QStyleOption>
#include <QPainter>
#include <QAction>
#include <QDebug>
#include <QMessageBox>
#include "batteryport.h"
LayoutBatteryWidget::LayoutBatteryWidget(QWidget *parent)
    : QWidget{parent}
{
    setContextMenuPolicy(Qt::ActionsContextMenu);
    lab = new QLabel(this);
    m_contextMenu = new QMenu;
    QAction *m_reviseAction = new QAction("修改背景", this);
    QAction *backUpAction = new QAction("位置备份", this);
    QAction *recoveryAction = new QAction("位置恢复", this);

    //给菜单添加菜单项
    //    m_contextMenu->addAction(m_addAction);
    m_contextMenu->addAction(m_reviseAction);
    m_contextMenu->addAction(backUpAction);
    m_contextMenu->addAction(recoveryAction);

    connect(m_reviseAction, &QAction::triggered, this, &LayoutBatteryWidget::reviseAction);
    connect(recoveryAction, &QAction::triggered,  [ = ]
    {
        this->on_recovery();
    });
    connect(backUpAction, &QAction::triggered,  [ = ]
    {
        this->on_back_up();
    });
    m_contextMenu->addAction("一键开启监控", [ = ]
    {
        QList<BatteryPort *> list_batteryport = this->findChildren<BatteryPort *>();
        for (BatteryPort *bport : list_batteryport )
        {
            //循环开启监控
            emit bport->sig_start_stop(true);
        }
    });

}
void LayoutBatteryWidget::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void LayoutBatteryWidget::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat("battery_info"))
    {
        event->setDropAction(Qt::MoveAction);
        event->accept();
    }
    else
    {
        event->ignore();
    }
}
void LayoutBatteryWidget::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasFormat("battery_info"))
    {
        QByteArray itemData = event->mimeData()->data("battery_info");
        battery_info *battery = (battery_info *)itemData.data();
        if(this->findChildren<BatteryPort *>(battery->power_id).count())
        {
            if(QMessageBox::critical(NULL, "警告", "该设备已开启监控") == QMessageBox::Ok)
            {
                return;
            }
        }
        BatteryPort *b = new BatteryPort(*battery, this);
        b->setObjectName(battery->power_id);
        connect(b, &BatteryPort::out_data, this, &LayoutBatteryWidget::out_data);
        connect(b, &BatteryPort::show_data, this, &LayoutBatteryWidget::on_info_show);
        b->resize(80, 40);
        b->setValue(15);
        b->move(event->pos());
        b->show();

        //        Battery* b1 = new Battery(this);
        //        b1->resize(200, 100);
        //        b1->setValue(15);
        //        b1->move(0, 0);
        //        b1->show();
        event->setDropAction(Qt::MoveAction);
        event->accept();

    }
    else
    {
        event->ignore();
    }
}
void LayoutBatteryWidget::on_info_show(const QVariant &data, battery_info battery, bool show)
{
    //lab->setText("测试");
    if(show)
    {
        if(battery.type == "JHJ")
        {
            JHJ_Param jhj_param = data.value<JHJ_Param>();
            QString str = QString("设备编号：%1\n安装地点：%2\n电池类型：%3\n电量：%4%").arg(battery.power_id).arg(battery.site)
                          .arg(battery.type).arg(jhj_param.dliang);
            lab->setText(str);

        }
        else if (battery.type == "UPS1")
        {

            UPS1 ups1 = data.value<UPS1>();
            QString str = QString("设备编号：%1\n安装地点：%2\n电池类型：%3\n电量：%4%").arg(battery.power_id).arg(battery.site)
                          .arg(battery.type).arg(ups1.ups1_yc.BatterySurplus / ups1.ups1_yc.BatteryCapacity * 100);
            lab->setText(str);
            //float percentage = ups1.ups1_yc.BatterySurplus / ups1.ups1_yc.BatteryCapacity * 100;

        }
        else if (battery.type == "UPS2")
        {
            if(data.isNull())
            {
                return;
            }
            UPS2 ups2 = data.value<UPS2>();
            QString str = QString("设备编号：%1\n安装地点：%2\n电池类型：%3\n电量：%4%").arg(battery.power_id).arg(battery.site)
                          .arg(battery.type).arg(ups2.AbsoluteStateofCharge);
            lab->setText(str);
            //float percentage = ups1.ups1_yc.BatterySurplus / ups1.ups1_yc.BatteryCapacity * 100;

        }
        else
        {
            QString str = "电池种类不详";
            lab->setText(str);
        }
        lab->move(mapFromGlobal(QCursor::pos()) + QPoint(50, 30));
        lab->show();
    }
    else
    {
        lab->hide();
    }

}

void LayoutBatteryWidget::on_recovery()
{
    qInfo() << "备份恢复-电池数量=" << myApp::back_up_battery.count();

    if(myApp::back_up_battery.count())
    {

        for (battery_info item : myApp::back_up_battery)
        {

            //如该电池已在界面中，只需移动位置即可
            QList<BatteryPort *> list_batteryport = this->findChildren<BatteryPort *>(item.power_id);
            if(list_batteryport.count())
            {
                list_batteryport.takeFirst()->move(item.pos);
                update();
                continue;
            }

            BatteryPort *b = new BatteryPort(item, this);
            emit b->sig_start_stop(false);
            b->setObjectName(item.power_id);
            connect(b, &BatteryPort::out_data, this, &LayoutBatteryWidget::out_data);
            connect(b, &BatteryPort::show_data, this, &LayoutBatteryWidget::on_info_show);
            b->resize(80, 40);
            b->setValue(15);
            b->move(item.pos);
            b->show();
            myHelper::Sleep(500);
        }
        if(QMessageBox::question(nullptr, "请求", "是否一键开启监控") == QMessageBox::Yes)
        {
            QList<BatteryPort *> list_batteryport = this->findChildren<BatteryPort *>();
            for (BatteryPort *bport : list_batteryport )
            {
                //循环开启监控
                emit bport->sig_start_stop(true);
            }
        }
        else
        {
            return;
        }


    }
    //    battery_info battery;
    //    battery.power_id = "JHJ_2";
    //    battery.site = "测试";
    //    battery.type = "JHJ";
    //    battery.port_name = "COM3";
    //    BatteryPort* b = new BatteryPort(battery, this);
    //    b->setObjectName(battery.power_id);
    //    connect(b, &BatteryPort::out_data, this, &LayoutBatteryWidget::out_data);
    //    connect(b, &BatteryPort::show_data, this, &LayoutBatteryWidget::on_info_show);
    //    b->resize(100, 50);
    //    b->setValue(15);
    //    b->move(100, 100);
    //    b->show();
}


/**
 * @brief  on_back_up
 * @author Wei.Liu
 * @date   2022-02-23
 * @script 电池备份函数
 */
void LayoutBatteryWidget::on_back_up()
{
    myApp::back_up_battery.clear();
    QList<BatteryPort *> list_batteryport = this->findChildren<BatteryPort *>();
    for (BatteryPort *bport : list_batteryport )
    {
        //先保存到本地，考虑到操作过程中如需还原，就需要从本地获取
        //程序结束后再进行备份
        myApp::back_up_battery.append( bport->getBatteryInfo());
    }
}


void LayoutBatteryWidget::mousePressEvent(QMouseEvent *e)
{
    if(e->button() == Qt::RightButton)
    {
        m_contextMenu->move(QCursor::pos());
        m_contextMenu->show();
    }
}

void LayoutBatteryWidget::reviseAction()
{
    QString filepath = FileHelper::GetFileName("Images (*.png *.xpm *.jpg)");
    qDebug() << filepath;
    this->setStyleSheet(QString("border-image: url(%1);").arg(filepath));
    myApp::BackGroundPath = filepath;
    myApp::WriteConfig();
}
