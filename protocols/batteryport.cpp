#include "batteryport.h"
#include <QtWidgets>
#include <QVBoxLayout>
#include <QDebug>
#include <QMouseEvent>
// #include "showinfoform.h"
// #include "ups1form.h"
// #include "ups2form.h"
// #include "qsingleton.h"
// #include "myapp.h"

BatteryPort::BatteryPort(const battery_info batteryInfo, QWidget *parent)
    : QWidget(parent)
    , m_batteryInfo(batteryInfo)
{
    initUI();
    initThread();
    initContextMenu();
}



void BatteryPort::initUI()
{
    // 创建Battery实例
    m_battery = new Battery(this);
    m_battery->resize(80, 40);

    // 设置布局
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(m_battery);
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);

    // 设置大小策略
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setFixedSize(m_battery->size());
}

void BatteryPort::initContextMenu()
{
    m_contextMenu = new QMenu(this);
    QAction *m_delAction = new QAction("删除", this);
    QAction *m_startAction = new QAction("开始", this);
    QAction *m_stopAction = new QAction("停止", this);

    m_contextMenu->addAction(m_startAction);
    m_contextMenu->addAction(m_delAction);
    m_contextMenu->addAction(m_stopAction);

    connect(m_delAction, &QAction::triggered, this, &BatteryPort::delete_action);
}

void BatteryPort::initThread()
{
    m_serial = new SerialWorker();
    connect(m_serial, &SerialWorker::forwardBatteryData, this, [=](const BMS_1 & bmsData)
    {
        /*
        m_data = data;
        emit out_data(data, m_batteryInfo.type);
        */
        // 更新电池显示
        if(m_batteryInfo.type == "BMS_1")
        {
            m_battery->setAlarmValue(30);
            m_battery->setValue(bmsData.soc);
        }
    });
    connect(m_serial,&SerialWorker::error,this,[=](const QString &err){
        // TODO : 串口打开失败具体原因
    });
    connect(m_serial,&SerialWorker::communicationTimeout,this,[=]{
    // TODO : 电池通信超时问题
    });
    qDebug()<<m_batteryInfo.port_name;
    // TODO : 电池的打开和关闭 用于一键开启监控功能
    //connect(this, &BatteryPort::sig_start_stop, m_serial, &SerialWorker::start_or_stop);
    // TODO : 控制功能
    //connect(QSingleton::instance().get(), &QSingleton::sig_control, m_serial, &SerialWorker::on_cfd_data);
    //connect(QSingleton::instance().get(), &QSingleton::sig_serial_send, m_serial, &SerialWorker::on_send_data);

    // TODO : 如果开启失败，则删除本电池
    // if(!m_serial->get_status())
    // {
    //     this->delete_action();
    // }
    // // TODO : 放电记录
    // connect(m_serial, &SerialWorker::sig_fd, this, [ = ](bool status, QDateTime start_time)
    // {
    //     QSingleton::instance().get()->cfd_record(m_batteryInfo.power_id, m_batteryInfo.site, status, start_time);
    // });
    m_serial->startReading(m_batteryInfo.port_name,m_batteryInfo.type);
}

void BatteryPort::mousePressEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton)
    {
        clickPos = e->pos();
    }
}

void BatteryPort::mouseMoveEvent(QMouseEvent *e)
{
    if(e->buttons() & Qt::LeftButton)
    {
        QPoint offset = e->pos() - clickPos;
        if(offset != QPoint(0, 0))
        {
            QPoint endPoint = pos() + offset;
            move(endPoint);
            m_batteryInfo.pos = endPoint;
        }
    }
}

void BatteryPort::mouseDoubleClickEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        if(m_batteryInfo.type == "BMS1")
        {
            // TODO : 电池信息详情
            // ShowInfoForm *show = new ShowInfoForm(m_batteryInfo.power_id);
            // connect(m_serial, &SerialWorker::out_data, show, &ShowInfoForm::on_showinfo);
            // show->resize(829, 662);
            // show->move(500, 300);
            // show->show();
        }
    }
}

void BatteryPort::enterEvent(QEvent *e)
{
    emit show_data(bms_1, m_batteryInfo, true);
}

void BatteryPort::leaveEvent(QEvent *e)
{
    emit show_data(bms_1, m_batteryInfo, false);
}

void BatteryPort::contextMenuEvent(QContextMenuEvent *event)
{
    m_contextMenu->exec(event->globalPos());
}
// TODO : 删除电池操作
void BatteryPort::delete_action()
{
    // QSingleton::instance().get()->set_delete_power(m_batteryInfo.power_id);
    // m_serial->deleteLater();
    // this->deleteLater();
    // QSingleton::instance().get()->table_clear();
    this->deleteLater();
    qDebug()<<"删除电池操作";
}

void BatteryPort::setBatteryValue(const int &value)
{
    m_battery->setValue(value);
}
