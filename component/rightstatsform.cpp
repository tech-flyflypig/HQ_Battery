﻿#include "rightstatsform.h"
#include "ui_rightstatsform.h"
#include <QDebug>

RightStatsForm::RightStatsForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::RightStatsForm)
      // m_currentBattery 现在是 weak_ptr，不需要初始化
{
    ui->setupUi(this);
}

RightStatsForm::~RightStatsForm()
{
    // 不需要手动断开连接，m_currentBattery 作为弱引用
    // 不会影响对象的生命周期
    delete ui;
}

void RightStatsForm::setBatteryInfo(BatteryListForm *battery)
{
    // 如果原来的 weak_ptr 还有效，先断开连接
    auto currentBattery = m_currentBattery.lock(); // 尝试获取强引用
    if (currentBattery)
    {
        // 断开信号连接
        disconnect(currentBattery.get(), &BatteryListForm::dataReceived,
                   this, &RightStatsForm::updateBatteryData);
        disconnect(currentBattery.get(), &BatteryListForm::communicationError,
                   this, &RightStatsForm::handleCommunicationError);
        disconnect(currentBattery.get(), &BatteryListForm::communicationTimeout,
                   this, &RightStatsForm::handleCommunicationTimeout);
        disconnect(currentBattery.get(), &BatteryListForm::monitoringStatusChanged,
                   this, &RightStatsForm::handleMonitoringStatusChanged);
    }

    // 如果提供了新的电池对象
    if (battery)
    {
        // 尝试获取 shared_ptr
        try
        {
            auto sharedBattery = battery->getSharedPtr(); // 使用 BatteryListForm 的 shared_from_this()
            m_currentBattery = sharedBattery; // 存储弱引用

            // 获取电池信息
            battery_info info = battery->getBatteryInfo();
            BMS_1 lastData = battery->getLastData();

            qDebug() << "RightStatsForm: 设置电池信息 - ID:" << info.power_id << ", 位置:" << info.site;

            // 更新UI显示电池基本信息
            updateBatteryData(battery, lastData);

            // 连接信号，接收电池数据更新
            connect(battery, &BatteryListForm::dataReceived,
                    this, &RightStatsForm::updateBatteryData);
            connect(battery, &BatteryListForm::communicationError,
                    this, &RightStatsForm::handleCommunicationError);
            connect(battery, &BatteryListForm::communicationTimeout,
                    this, &RightStatsForm::handleCommunicationTimeout);
            connect(battery, &BatteryListForm::monitoringStatusChanged,
                    this, &RightStatsForm::handleMonitoringStatusChanged);
                    
            // 立即同步监控状态
            handleMonitoringStatusChanged(battery, battery->getMonitoringStatus());
        }
        catch (const std::bad_weak_ptr &e)
        {
            // 如果无法获取shared_ptr，这可能是因为对象不是由shared_ptr管理的
            qDebug() << "警告: 无法获取电池对象的shared_ptr: " << e.what();
        }
    }
    else
    {
        // 清除弱引用
        m_currentBattery.reset();
    }
}

void RightStatsForm::updateBatteryData(BatteryListForm *battery, const BMS_1 &data)
{
    // 增强的安全检查
    if (!battery) return; // 如果指针为空，直接返回

    // 检查弱引用是否指向同一个对象
    auto currentBattery = m_currentBattery.lock();
    if (!currentBattery || currentBattery.get() != battery) return;

    // 更新电池信息显示
    // 更新基本参数
    if (ui && ui->label_ratedCapacity)
    {
        ui->label_ratedCapacity->setText(QString::number(data.ratedCapacity / 100.0 * 1000, 'f', 1));
    }

    if (ui && ui->label_remainCapacity)
    {
        ui->label_remainCapacity->setText(QString::number(data.remainCapacity / 100.0 * 1000, 'f', 1));
    }

    // 电流分为充电电流和放电电流
    double current = data.current / 100.0;
    if (ui && ui->label_current_out)
    {
        ui->label_current_out->setText(current < 0 ? QString::number(-current, 'f', 2) : "0.00");
    }

    if (ui && ui->label_current_in)
    {
        ui->label_current_in->setText(current > 0 ? QString::number(current, 'f', 2) : "0.00");
    }
    ui->label_pcbtemp->setText(QString::number(data.ambientTemp / 10.0, 'f', 1));
    ui->label_voltage->setText(QString::number(data.voltage / 100.0, 'f', 2) );
    ui->label_tempMax->setText(QString::number(data.tempMax / 10.0, 'f', 1));
    // ui->label_sitecom->setText(data.battery_info.site);
    // ui->label_port->setText(data.battery_info.port_name);
    ui->label_upload_time->setText(data.battery_info.last_time.toString("yyyy-MM-dd HH:mm:ss"));
    try
    {
        battery_info info = battery->getBatteryInfo();
        ui->label_ip->setText(info.site);
        // ui->label_sitecom->setText(info.site);
        ui->label_port->setText(info.port_name);

    }
    catch (...)
    {
        // 安全捕获任何异常，避免崩溃
        qDebug() << "访问电池信息时发生异常";
    }
}

void RightStatsForm::handleCommunicationError(BatteryListForm *battery, const QString &errorMessage)
{
    // 检查弱引用是否有效且指向同一个对象
    auto currentBattery = m_currentBattery.lock();
    if (!currentBattery || currentBattery.get() != battery) return;
    ui->label_battery_status->setStyleSheet("border-image: url(:/image/故障.png);");

    qDebug() << "通信错误: " << errorMessage;
}

void RightStatsForm::handleCommunicationTimeout(BatteryListForm *battery)
{
    // 检查弱引用是否有效且指向同一个对象
    auto currentBattery = m_currentBattery.lock();
    if (!currentBattery || currentBattery.get() != battery) return;
    ui->label_battery_status->setStyleSheet("border-image: url(:/image/故障.png);");
    qDebug() << "通信超时";
}

void RightStatsForm::handleMonitoringStatusChanged(BatteryListForm *battery, BatteryListForm::MonitoringStatus status)
{
    // 检查弱引用是否有效且指向同一个对象
    auto currentBattery = m_currentBattery.lock();
    if (!currentBattery || currentBattery.get() != battery) return;
    
    // 根据监控状态更新UI
    switch (status) {
        case BatteryListForm::Running:
            ui->label_battery_status->setStyleSheet("border-image: url(:/image/运行.png);");
            break;
        case BatteryListForm::Stopped:
            ui->label_battery_status->setStyleSheet("border-image: url(:/image/停止.png);");
            break;
        case BatteryListForm::Error:
            ui->label_battery_status->setStyleSheet("border-image: url(:/image/故障.png);");
            break;
        default:
            break;
    }
    
    qDebug() << "电池监控状态变化: " << status;
}
