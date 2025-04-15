#include "batteryport.h"
#include <QtWidgets>
#include <QVBoxLayout>
#include <QDebug>
#include <QMouseEvent>
#include "showinfoform.h"
#include "ups1form.h"
#include "ups2form.h"
#include "qsingleton.h"
#include "myapp.h"

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
    connect(m_serial, &SerialWorker::forwardBatteryData, this, [this](const BMS_1 & bmsData)
    {
        /*
        m_data = data;
        emit out_data(data, m_batteryInfo.type);
        */
        // 更新电池显示
        if(m_batteryInfo.type == "BMS1")
        {
            m_battery->setAlarmValue(30);
            m_battery->setValue(bmsData.soc);
        }
    });

    connect(this, &BatteryPort::sig_start_stop, m_serial, &SerialWorker::start_or_stop);
    connect(QSingleton::instance().get(), &QSingleton::sig_control, m_serial, &SerialWorker::on_cfd_data);
    connect(QSingleton::instance().get(), &QSingleton::sig_serial_send, m_serial, &SerialWorker::on_send_data);

    if(!m_serial->get_status())
    {
        this->delete_action();
    }

    connect(m_serial, &SerialWorker::sig_fd, this, [ = ](bool status, QDateTime start_time)
    {
        QSingleton::instance().get()->cfd_record(m_batteryInfo.power_id, m_batteryInfo.site, status, start_time);
    });
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
            ShowInfoForm *show = new ShowInfoForm(m_batteryInfo.power_id);
            connect(m_serial, &SerialWorker::out_data, show, &ShowInfoForm::on_showinfo);
            show->resize(829, 662);
            show->move(500, 300);
            show->show();
        }
    }
}

void BatteryPort::enterEvent(QEvent *e)
{
    if(!m_data.isNull())
    {
        emit show_data(m_data, m_batteryInfo, true);
    }
}

void BatteryPort::leaveEvent(QEvent *e)
{
    emit show_data(QVariant(), m_batteryInfo, false);
}

void BatteryPort::contextMenuEvent(QContextMenuEvent *event)
{
    m_contextMenu->exec(event->globalPos());
}

void BatteryPort::delete_action()
{
    QSingleton::instance().get()->set_delete_power(m_batteryInfo.power_id);
    m_serial->deleteLater();
    this->deleteLater();
    QSingleton::instance().get()->table_clear();
}
