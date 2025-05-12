#include "bms1infoshowform.h"
#include "ui_bms1infoshowform.h"
#include <QDebug>

BMS1InfoShowForm::BMS1InfoShowForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::BMS1InfoShowForm)
    , m_currentBattery(nullptr)
    , m_chartsInitialized(false)
{
    ui->setupUi(this);

    // 设置所有RadioButton为只读状态（禁用但保持正常视觉效果）
    initializeRadioButtons();

    // 创建曲线图组件
    m_temperatureChart = new BatteryChartWidget(TEMPERATURE_CHART, this);
    m_voltageCurrentChart = new BatteryChartWidget(VOLTAGE_CURRENT_CHART, this);

    // 将曲线图组件添加到布局
    QVBoxLayout *tempLayout = new QVBoxLayout(ui->widget_templine);
    tempLayout->setContentsMargins(10, 80, 10, 0);  // 只保留顶部边距，用于标题
    tempLayout->setSpacing(0);
    tempLayout->addWidget(m_temperatureChart);

    QVBoxLayout *voltageLayout = new QVBoxLayout(ui->widget_voltageline);
    voltageLayout->setContentsMargins(10, 80, 10, 0);  // 只保留顶部边距，用于标题
    voltageLayout->setSpacing(0);
    voltageLayout->addWidget(m_voltageCurrentChart);
}

BMS1InfoShowForm::~BMS1InfoShowForm()
{
    delete ui;
}

void BMS1InfoShowForm::setBatteryInfo(BatteryListForm *battery)
{
    // 如果已经连接了电池，先断开
    if (m_currentBattery)
    {
        disconnect(m_currentBattery, &BatteryListForm::dataReceived,
                   this, &BMS1InfoShowForm::updateBatteryData);
        disconnect(m_currentBattery, &BatteryListForm::communicationError,
                   this, &BMS1InfoShowForm::handleCommunicationError);
        disconnect(m_currentBattery, &BatteryListForm::communicationTimeout,
                   this, &BMS1InfoShowForm::handleCommunicationTimeout);

        // 停止图表更新
        m_temperatureChart->stopRealTimeUpdate();
        m_voltageCurrentChart->stopRealTimeUpdate();
    }

    m_currentBattery = battery;

    if (battery)
    {
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

        // 清除图表数据
        m_temperatureChart->clearAllData();
        m_voltageCurrentChart->clearAllData();

        // 开始图表实时更新
        m_temperatureChart->startRealTimeUpdate();
        m_voltageCurrentChart->startRealTimeUpdate();

        // 添加初始数据
        m_temperatureChart->addTemperatureData(lastData.tempMax / 10.0);
        m_voltageCurrentChart->addVoltageCurrentData(lastData.voltage / 100.0, lastData.current / 100.0);
    }
}

// 手动触发返回主界面信号
void BMS1InfoShowForm::triggerBackToMain()
{
    // 断开与电池的连接
    if (m_currentBattery)
    {
        disconnect(m_currentBattery, &BatteryListForm::dataReceived,
                   this, &BMS1InfoShowForm::updateBatteryData);
        disconnect(m_currentBattery, &BatteryListForm::communicationError,
                   this, &BMS1InfoShowForm::handleCommunicationError);
        disconnect(m_currentBattery, &BatteryListForm::communicationTimeout,
                   this, &BMS1InfoShowForm::handleCommunicationTimeout);

        // 停止图表更新
        m_temperatureChart->stopRealTimeUpdate();
        m_voltageCurrentChart->stopRealTimeUpdate();

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

    // 添加数据到图表
    double temp = data.tempMax / 10.0;
    double voltage = data.voltage / 100.0;
    double current = data.current / 100.0;

    // 更新图表数据
    m_temperatureChart->addTemperatureData(temp);
    m_voltageCurrentChart->addVoltageCurrentData(voltage, current);
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

// 初始化RadioButton为只读状态
void BMS1InfoShowForm::initializeRadioButtons()
{
    // 保护状态RadioButton设置
    QList<QRadioButton *> allRadioButtons;

    // 保护状态RadioButton
    allRadioButtons << ui->radioButton_OverchargeVoltage_p
                    << ui->radioButton_OverdischargeVoltage_p
                    << ui->radioButton_ChargingOvercurrent_p
                    << ui->radioButton_DischargingOvercurrent_p
                    << ui->radioButton_CellHighTemp_p
                    << ui->radioButton_CellLowTemp_p
                    << ui->radioButton_EnvHighTemp_p
                    << ui->radioButton_EnvLowTemp_p
                    << ui->radioButton_OutputShortCircuit_p
                    << ui->radioButton_PowerTubeHighTemp_p;

    // 告警状态RadioButton
    allRadioButtons << ui->radioButton_OverchargeVoltage
                    << ui->radioButton_OverdischargeVoltage
                    << ui->radioButton_ChargingOvercurrent
                    << ui->radioButton_DischargingOvercurrent
                    << ui->radioButton_CellHighTemp
                    << ui->radioButton_CellLowTemp
                    << ui->radioButton_EnvHighTemp
                    << ui->radioButton_EnvLowTemp
                    << ui->radioButton_BatteryLevel
                    << ui->radioButton_PowerTubeHighTemp;

    // 批量设置所有RadioButton为只读
    for (QRadioButton *radioButton : allRadioButtons)
    {
        // 使鼠标事件透明，阻止用户点击
        radioButton->setAttribute(Qt::WA_TransparentForMouseEvents, true);
        radioButton->setFocusPolicy(Qt::NoFocus); // 防止键盘焦点
    }
}
