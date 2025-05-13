#include "leftstatsform.h"
#include "ui_leftstatsform.h"
#include "BatteryStats.h"

LeftStatsForm::LeftStatsForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LeftStatsForm)
{
    ui->setupUi(this);
    // 连接统计数据变化信号
    connect(BatteryStats::instance(), &BatteryStats::statsChanged, this, &LeftStatsForm::updateStats);
    // 初始化统计显示
    updateStats();
}

LeftStatsForm::~LeftStatsForm()
{
    delete ui;
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
    int over_voltage_discharge_protect = stats->getSingleOvervoltageCount() + stats->getSingleUndervoltageCount() + 
                                         stats->getTotalOvervoltageCount() + stats->getTotalUndervoltageCount();
    ui->label_over_voltage_discharge_protect->setText(QString::number(over_voltage_discharge_protect));
    
    int tempeture_protect = stats->getCellChargingHighTempCount() + stats->getCellDischargingHighTempCount() + 
                           stats->getCellChargingLowTempCount() + stats->getCellDischargingLowTempCount() + 
                           stats->getPcbHighTempCount() + stats->getPcbLowTempCount() + 
                           stats->getPowerEquipProtectCount();
    ui->label_tempeture_protect->setText(QString::number(tempeture_protect));
    
    ui->label_shortCircuitProtect_num->setText(QString::number(stats->getShortCircuitCount()));
    
    // 更新告警统计
    int voltage_alarm = stats->getSingleHighVoltageAlarmCount() + stats->getSingleLowVoltageAlarmCount() + 
                       stats->getTotalHighVoltageAlarmCount() + stats->getTotalLowVoltageAlarmCount();
    ui->label_voltage_alarm->setText(QString::number(voltage_alarm));
    
    int temperature_alarm = stats->getCellChargingHighTempAlarmCount() + stats->getCellDischargingHighTempAlarmCount() + 
                           stats->getCellChargingLowTempAlarmCount() + stats->getCellDischargingLowTempAlarmCount() + 
                           stats->getPcbHighTempAlarmCount() + stats->getPcbLowTempAlarmCount() + 
                           stats->getPowerEquipHighTempAlarmCount();
    ui->label_temperature_alarm->setText(QString::number(temperature_alarm));
    
    int current_alarm = stats->getChargingOvercurrentAlarmCount() + stats->getDischargingOvercurrentAlarmCount();
    ui->label_current_alarm->setText(QString::number(current_alarm));
}
