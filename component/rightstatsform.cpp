#include "rightstatsform.h"
#include "ui_rightstatsform.h"
#include <QDebug>

RightStatsForm::RightStatsForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::RightStatsForm)
    , m_currentBattery(nullptr)
{
    ui->setupUi(this);
}

RightStatsForm::~RightStatsForm()
{
    // 断开与当前电池的连接
    if (m_currentBattery) {
        disconnect(m_currentBattery, &BatteryListForm::dataReceived,
                  this, &RightStatsForm::updateBatteryData);
        disconnect(m_currentBattery, &BatteryListForm::communicationError,
                  this, &RightStatsForm::handleCommunicationError);
        disconnect(m_currentBattery, &BatteryListForm::communicationTimeout,
                  this, &RightStatsForm::handleCommunicationTimeout);
    }
    
    delete ui;
}

void RightStatsForm::setBatteryInfo(BatteryListForm *battery)
{
    // 如果已经连接了电池，先断开
    if (m_currentBattery)
    {
        disconnect(m_currentBattery, &BatteryListForm::dataReceived,
                  this, &RightStatsForm::updateBatteryData);
        disconnect(m_currentBattery, &BatteryListForm::communicationError,
                  this, &RightStatsForm::handleCommunicationError);
        disconnect(m_currentBattery, &BatteryListForm::communicationTimeout,
                  this, &RightStatsForm::handleCommunicationTimeout);
    }

    m_currentBattery = battery;

    if (battery)
    {
        // A. 获取电池信息
        battery_info info = battery->getBatteryInfo();
        BMS_1 lastData = battery->getLastData();

        qDebug() << "RightStatsForm: 设置电池信息 - ID:" << info.power_id << ", 位置:" << info.site;

        // B. 更新UI显示电池基本信息
        updateBatteryData(battery, lastData);

        // C. 连接信号，接收电池数据更新
        connect(battery, &BatteryListForm::dataReceived,
                this, &RightStatsForm::updateBatteryData);
        connect(battery, &BatteryListForm::communicationError,
                this, &RightStatsForm::handleCommunicationError);
        connect(battery, &BatteryListForm::communicationTimeout,
                this, &RightStatsForm::handleCommunicationTimeout);
        
        // D. 确保电池开始通信
        battery->startCommunication();
    }
}

void RightStatsForm::updateBatteryData(BatteryListForm *battery, const BMS_1 &data)
{
    if (battery != m_currentBattery) return;

    // 更新电池信息显示
    // 更新基本参数
    if (ui->label_ratedCapacity) {
        ui->label_ratedCapacity->setText(QString::number(data.ratedCapacity / 100.0, 'f', 1));
    }
    
    if (ui->label_remainCapacity) {
        ui->label_remainCapacity->setText(QString::number(data.remainCapacity / 100.0, 'f', 1));
    }
    
    // 电流分为充电电流和放电电流
    double current = data.current / 100.0;
    if (ui->label_current_out) {
        ui->label_current_out->setText(current < 0 ? QString::number(-current, 'f', 2) : "0.00");
    }
    
    if (ui->label_current_in) {
        ui->label_current_in->setText(current > 0 ? QString::number(current, 'f', 2) : "0.00");
    }
    
    // 更新设备信息
    battery_info info = battery->getBatteryInfo();
    if (ui->label_sitecom) {
        ui->label_sitecom->setText(info.site);
    }
    
    if (ui->label_port) {
        ui->label_port->setText(info.port_name);
    }
}

void RightStatsForm::handleCommunicationError(BatteryListForm *battery, const QString &errorMessage)
{
    if (battery != m_currentBattery) return;
    qDebug() << "通信错误: " << errorMessage;
}

void RightStatsForm::handleCommunicationTimeout(BatteryListForm *battery)
{
    if (battery != m_currentBattery) return;
    qDebug() << "通信超时";
}
