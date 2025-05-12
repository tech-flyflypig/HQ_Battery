#include "bms1infoshowform.h"
#include "ui_bms1infoshowform.h"
#include <QDebug>

BMS1InfoShowForm::BMS1InfoShowForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::BMS1InfoShowForm)
    , m_currentBattery(nullptr)
{
    ui->setupUi(this);
    
    // 连接返回按钮信号
    connect(ui->btn_back, &QPushButton::clicked, this, &BMS1InfoShowForm::onBackButtonClicked);
}

BMS1InfoShowForm::~BMS1InfoShowForm()
{
    delete ui;
}

void BMS1InfoShowForm::setBatteryInfo(BatteryListForm *battery)
{
    m_currentBattery = battery;
    
    if (battery) {
        // 获取电池信息
        battery_info info = battery->getBatteryInfo();
        BMS_1 lastData = battery->getLastData();
        
        qDebug() << "BMS1InfoShowForm: 设置电池信息 - ID:" << info.power_id << ", 位置:" << info.site;
        
        // 更新UI显示电池基本信息
        updateBatteryData(battery, lastData);
        
        // 连接信号，接收电池数据更新
        connect(battery, &BatteryListForm::dataReceived,
                this, &BMS1InfoShowForm::updateBatteryData);
        connect(battery, &BatteryListForm::communicationError,
                this, &BMS1InfoShowForm::handleCommunicationError);
        connect(battery, &BatteryListForm::communicationTimeout,
                this, &BMS1InfoShowForm::handleCommunicationTimeout);
    }
}

void BMS1InfoShowForm::onBackButtonClicked()
{
    // 断开与电池的连接
    if (m_currentBattery) {
        disconnect(m_currentBattery, &BatteryListForm::dataReceived,
                  this, &BMS1InfoShowForm::updateBatteryData);
        disconnect(m_currentBattery, &BatteryListForm::communicationError,
                  this, &BMS1InfoShowForm::handleCommunicationError);
        disconnect(m_currentBattery, &BatteryListForm::communicationTimeout,
                  this, &BMS1InfoShowForm::handleCommunicationTimeout);
        
        m_currentBattery = nullptr;
    }
    
    // 发送返回信号
    emit backToMain();
}

void BMS1InfoShowForm::updateBatteryData(BatteryListForm *battery, const BMS_1 &data)
{
    if (battery != m_currentBattery) return;
    
    // 更新电池信息显示
    ui->label_soc->setText(QString::number(data.soc) + "%");
    ui->label_voltage->setText(QString::number(data.voltage / 100.0, 'f', 2) + "V");
    ui->label_current->setText(QString::number(data.current / 100.0, 'f', 2) + "A");
    ui->label_current_f->setText(QString::number(data.current / 100.0, 'f', 2) + "A");
    ui->label_tempMax->setText(QString::number(data.tempMax / 10.0, 'f', 1));
    
    // 更新其他数据
    ui->label_ratedCapacity->setText(QString::number(data.ratedCapacity / 100.0));
    ui->label_remainCapacity->setText(QString::number(data.remainCapacity));
    
    // 更新保护状态和告警信息（如果有）
    updateProtectionStatus(data.protectStatus);
    updateAlarmStatus(data.alarmStatus);
}

void BMS1InfoShowForm::handleCommunicationError(BatteryListForm *battery, const QString &errorMessage)
{
    if (battery != m_currentBattery) return;
    
    qDebug() << "通信错误: " << errorMessage;
}

void BMS1InfoShowForm::handleCommunicationTimeout(BatteryListForm *battery)
{
    if (battery != m_currentBattery) return;
    
    qDebug() << "通信超时";
}

void BMS1InfoShowForm::updateProtectionStatus(unsigned int protectStatus)
{
    // 更新保护状态RadioButton
    ui->radioButton_OverchargeVoltage_p->setChecked(protectStatus & 0x0001);
    ui->radioButton_OverdischargeVoltage_p->setChecked(protectStatus & 0x0002);
    ui->radioButton_ChargingOvercurrent_p->setChecked(protectStatus & 0x0004);
    ui->radioButton_DischargingOvercurrent_p->setChecked(protectStatus & 0x0008);
    ui->radioButton_CellHighTemp_p->setChecked(protectStatus & 0x0010);
    ui->radioButton_CellLowTemp_p->setChecked(protectStatus & 0x0020);
    ui->radioButton_EnvHighTemp_p->setChecked(protectStatus & 0x0040);
    ui->radioButton_EnvLowTemp_p->setChecked(protectStatus & 0x0080);
    ui->radioButton_OutputShortCircuit_p->setChecked(protectStatus & 0x0100);
    ui->radioButton_PowerTubeHighTemp_p->setChecked(protectStatus & 0x0200);
}

void BMS1InfoShowForm::updateAlarmStatus(unsigned int alarmStatus)
{
    // 更新告警状态RadioButton
    ui->radioButton_OverchargeVoltage->setChecked(alarmStatus & 0x0001);
    ui->radioButton_OverdischargeVoltage->setChecked(alarmStatus & 0x0002);
    ui->radioButton_ChargingOvercurrent->setChecked(alarmStatus & 0x0004);
    ui->radioButton_DischargingOvercurrent->setChecked(alarmStatus & 0x0008);
    ui->radioButton_CellHighTemp->setChecked(alarmStatus & 0x0010);
    ui->radioButton_CellLowTemp->setChecked(alarmStatus & 0x0020);
    ui->radioButton_EnvHighTemp->setChecked(alarmStatus & 0x0040);
    ui->radioButton_EnvLowTemp->setChecked(alarmStatus & 0x0080);
    ui->radioButton_BatteryLevel->setChecked(alarmStatus & 0x0100);
    ui->radioButton_PowerTubeHighTemp->setChecked(alarmStatus & 0x0200);
}
