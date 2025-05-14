#include "leftstatsform.h"
#include "ui_leftstatsform.h"
#include "BatteryStats.h"

LeftStatsForm::LeftStatsForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LeftStatsForm)
{
    ui->setupUi(this);
    
    // 初始化RadioButton为只读状态
    initializeRadioButtons();
    
    // 连接统计数据变化信号
    connect(BatteryStats::instance(), &BatteryStats::statsChanged, this, &LeftStatsForm::updateStats);
    // 初始化统计显示
    updateStats();
}

LeftStatsForm::~LeftStatsForm()
{
    delete ui;
}

// 初始化RadioButton为只读状态
void LeftStatsForm::initializeRadioButtons()
{
    // 创建RadioButton列表
    QList<QRadioButton *> allRadioButtons;
    
    // 故障报警部分的RadioButton
    allRadioButtons << ui->rbtn_voltage_alarm   // 电压报警
                    << ui->rbtn_current_alarm   // 电流报警
                    << ui->rbtn_temperature_alarm; // 温度告警
    
    // 保护状态部分的RadioButton
    allRadioButtons << ui->rbtn_short_circuit // 输出短路
                    << ui->rbtn_tempeture_protect // 温度保护
                    << ui->rbtn_over_voltage_and_discharge; // 过压过放
    
    // 批量设置所有RadioButton为只读
    for (QRadioButton *radioButton : allRadioButtons)
    {
        // 使鼠标事件透明，阻止用户点击
        radioButton->setAttribute(Qt::WA_TransparentForMouseEvents, true);
        radioButton->setFocusPolicy(Qt::NoFocus); // 防止键盘焦点
    }
}

void LeftStatsForm::updateStats()
{
    BatteryStats *stats = BatteryStats::instance();

    // 更新基本统计
    ui->label_total_num->setText(QString::number(stats->getTotalBatteryCount()));
    ui->label_runing_num->setText(QString::number(stats->getRunningBatteryCount()));
    ui->label_stop_num->setText(QString::number(stats->getStoppedBatteryCount()));
    ui->label_fault_num->setText(QString::number(stats->getFaultBatteryCount()));

    // 更新保护状态统计
    // 过压过放
    int over_voltage_discharge_protect = stats->getSingleOvervoltageCount() + stats->getSingleUndervoltageCount() + 
                                         stats->getTotalOvervoltageCount() + stats->getTotalUndervoltageCount();
    ui->label_over_voltage_discharge_protect->setText(QString::number(over_voltage_discharge_protect));

    // 温度保护
    int tempeture_protect = stats->getCellChargingHighTempCount() + stats->getCellDischargingHighTempCount() + 
                           stats->getCellChargingLowTempCount() + stats->getCellDischargingLowTempCount() + 
                           stats->getPcbHighTempCount() + stats->getPcbLowTempCount() + 
                           stats->getPowerEquipProtectCount();
    ui->label_tempeture_protect->setText(QString::number(tempeture_protect));

    // 输出短路
    ui->label_shortCircuitProtect_num->setText(QString::number(stats->getShortCircuitCount()));
    
    // 更新告警统计
    // 电压告警
    int voltage_alarm = stats->getSingleHighVoltageAlarmCount() + stats->getSingleLowVoltageAlarmCount() + 
                       stats->getTotalHighVoltageAlarmCount() + stats->getTotalLowVoltageAlarmCount();
    ui->label_voltage_alarm->setText(QString::number(voltage_alarm));

    // 温度告警
    int temperature_alarm = stats->getCellChargingHighTempAlarmCount() + stats->getCellDischargingHighTempAlarmCount() + 
                           stats->getCellChargingLowTempAlarmCount() + stats->getCellDischargingLowTempAlarmCount() + 
                           stats->getPcbHighTempAlarmCount() + stats->getPcbLowTempAlarmCount() + 
                           stats->getPowerEquipHighTempAlarmCount();

    ui->label_temperature_alarm->setText(QString::number(temperature_alarm));

    // 电流告警
    int current_alarm = stats->getChargingOvercurrentAlarmCount() + stats->getDischargingOvercurrentAlarmCount();
    ui->label_current_alarm->setText(QString::number(current_alarm));
    
    // 更新RadioButton选中状态
    ui->rbtn_voltage_alarm->setChecked(voltage_alarm > 0);
    ui->rbtn_current_alarm->setChecked(current_alarm > 0);
    ui->rbtn_temperature_alarm->setChecked(temperature_alarm > 0);
    ui->rbtn_over_voltage_and_discharge->setChecked(over_voltage_discharge_protect > 0);
    ui->rbtn_tempeture_protect->setChecked(tempeture_protect > 0);
    ui->rbtn_short_circuit->setChecked(stats->getShortCircuitCount() > 0);
}
