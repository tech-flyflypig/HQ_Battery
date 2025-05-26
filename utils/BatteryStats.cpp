#include "BatteryStats.h"
#include <QDebug>

BatteryStats *BatteryStats::m_instance = nullptr;

BatteryStats *BatteryStats::instance()
{
    if (!m_instance)
    {
        m_instance = new BatteryStats();
    }
    return m_instance;
}

BatteryStats::BatteryStats(QObject *parent)
    : QObject(parent)
{
    // 初始化统计数据
    clearStats();
}

BatteryStats::~BatteryStats()
{
}

void BatteryStats::clearStats()
{
    m_batteryData.clear();
    m_runningCount = 0;
    m_stoppedCount = 0;
    m_faultCount = 0;
    m_protectStatusCounts.clear();
    m_alarmStatusCounts.clear();

    // 发出信号通知统计数据已变化
    emit statsChanged();
}

int BatteryStats::getTotalBatteryCount() const
{
    return m_batteryData.size();
}

int BatteryStats::getRunningBatteryCount() const
{
    return m_runningCount;
}

int BatteryStats::getStoppedBatteryCount() const
{
    return m_stoppedCount;
}

int BatteryStats::getFaultBatteryCount() const
{
    return m_faultCount;
}

// 保护状态统计方法
int BatteryStats::getSingleOvervoltageCount() const
{
    return m_protectStatusCounts.value(0x0001, 0); // BIT0
}

int BatteryStats::getSingleUndervoltageCount() const
{
    return m_protectStatusCounts.value(0x0002, 0); // BIT1
}

int BatteryStats::getTotalOvervoltageCount() const
{
    return m_protectStatusCounts.value(0x0004, 0); // BIT2
}

int BatteryStats::getTotalUndervoltageCount() const
{
    return m_protectStatusCounts.value(0x0008, 0); // BIT3
}

int BatteryStats::getChargingOvercurrentCount() const
{
    return m_protectStatusCounts.value(0x0010, 0); // BIT4
}

int BatteryStats::getDischargingOvercurrentCount() const
{
    return m_protectStatusCounts.value(0x0020, 0); // BIT5
}

int BatteryStats::getShortCircuitCount() const
{
    return m_protectStatusCounts.value(0x0040, 0); // BIT6
}

int BatteryStats::getFirstReservedCount() const
{
    return m_protectStatusCounts.value(0x0080, 0); // BIT7
}

int BatteryStats::getCellChargingHighTempCount() const
{
    return m_protectStatusCounts.value(0x0100, 0); // BIT8
}

int BatteryStats::getCellDischargingHighTempCount() const
{
    return m_protectStatusCounts.value(0x0200, 0); // BIT9
}

int BatteryStats::getCellChargingLowTempCount() const
{
    return m_protectStatusCounts.value(0x0400, 0); // BIT10
}

int BatteryStats::getCellDischargingLowTempCount() const
{
    return m_protectStatusCounts.value(0x0800, 0); // BIT11
}

int BatteryStats::getPcbHighTempCount() const
{
    return m_protectStatusCounts.value(0x1000, 0); // BIT12
}

int BatteryStats::getPcbLowTempCount() const
{
    return m_protectStatusCounts.value(0x2000, 0); // BIT13
}

int BatteryStats::getPowerEquipProtectCount() const
{
    return m_protectStatusCounts.value(0x4000, 0); // BIT14
}

int BatteryStats::getSecondReservedCount() const
{
    return m_protectStatusCounts.value(0x8000, 0); // BIT15
}

// 告警状态统计方法
int BatteryStats::getTotalAlarmCount() const
{
    int totalAlarms = 0;
    for (int count : m_alarmStatusCounts.values())
    {
        totalAlarms += count;
    }
    return totalAlarms;
}

int BatteryStats::getSingleHighVoltageAlarmCount() const
{
    return m_alarmStatusCounts.value(0x0001, 0); // BIT0
}

int BatteryStats::getSingleLowVoltageAlarmCount() const
{
    return m_alarmStatusCounts.value(0x0002, 0); // BIT1
}

int BatteryStats::getTotalHighVoltageAlarmCount() const
{
    return m_alarmStatusCounts.value(0x0004, 0); // BIT2
}

int BatteryStats::getTotalLowVoltageAlarmCount() const
{
    return m_alarmStatusCounts.value(0x0008, 0); // BIT3
}

int BatteryStats::getChargingOvercurrentAlarmCount() const
{
    return m_alarmStatusCounts.value(0x0010, 0); // BIT4
}

int BatteryStats::getDischargingOvercurrentAlarmCount() const
{
    return m_alarmStatusCounts.value(0x0020, 0); // BIT5
}

int BatteryStats::getFirstAlarmReservedCount() const
{
    return m_alarmStatusCounts.value(0x0040, 0); // BIT6
}

int BatteryStats::getSecondAlarmReservedCount() const
{
    return m_alarmStatusCounts.value(0x0080, 0); // BIT7
}

int BatteryStats::getCellChargingHighTempAlarmCount() const
{
    return m_alarmStatusCounts.value(0x0100, 0); // BIT8
}

int BatteryStats::getCellDischargingHighTempAlarmCount() const
{
    return m_alarmStatusCounts.value(0x0200, 0); // BIT9
}

int BatteryStats::getCellChargingLowTempAlarmCount() const
{
    return m_alarmStatusCounts.value(0x0400, 0); // BIT10
}

int BatteryStats::getCellDischargingLowTempAlarmCount() const
{
    return m_alarmStatusCounts.value(0x0800, 0); // BIT11
}

int BatteryStats::getPcbHighTempAlarmCount() const
{
    return m_alarmStatusCounts.value(0x1000, 0); // BIT12
}

int BatteryStats::getPcbLowTempAlarmCount() const
{
    return m_alarmStatusCounts.value(0x2000, 0); // BIT13
}

int BatteryStats::getPowerEquipHighTempAlarmCount() const
{
    return m_alarmStatusCounts.value(0x4000, 0); // BIT14
}

int BatteryStats::getBatteryLowAlarmCount() const
{
    return m_alarmStatusCounts.value(0x8000, 0); // BIT15
}

void BatteryStats::updateBatteryStatus(const QString &batteryId, const BMS_1 &data)
{
    // 保存或更新电池数据
    m_batteryData[batteryId] = data;

    // 重新计算统计数据
    calculateStats();

    // 发出信号通知统计数据已变化
    emit statsChanged();
}

void BatteryStats::calculateStats()
{
    // 重置计数器
    m_runningCount = 0;
    m_stoppedCount = 0;
    m_faultCount = 0;
    m_protectStatusCounts.clear();
    m_alarmStatusCounts.clear();

    // 遍历所有电池数据进行统计
    for (const auto &data : m_batteryData)
    {
        // 状态统计
        if (data.battery_info.status == 0x00)   // 运行状态
        {
            m_runningCount++;
        }
        else if (data.battery_info.status == 0x01)//停止
        {
            m_stoppedCount++;
        }
        else if (data.battery_info.status == 0x02) //故障
        {
            m_faultCount++;
        }

        // 保护状态统计 - 根据附表2
        uint16_t protectStatus = data.protectStatus;
        if (protectStatus & 0x0001) m_protectStatusCounts[0x0001]++; // BIT0: 单体过压保护
        if (protectStatus & 0x0002) m_protectStatusCounts[0x0002]++; // BIT1: 单体欠压保护
        if (protectStatus & 0x0004) m_protectStatusCounts[0x0004]++; // BIT2: 总体过压保护
        if (protectStatus & 0x0008) m_protectStatusCounts[0x0008]++; // BIT3: 总体欠压保护
        if (protectStatus & 0x0010) m_protectStatusCounts[0x0010]++; // BIT4: 充电过流保护
        if (protectStatus & 0x0020) m_protectStatusCounts[0x0020]++; // BIT5: 放电过流保护
        if (protectStatus & 0x0040) m_protectStatusCounts[0x0040]++; // BIT6: 短路
        if (protectStatus & 0x0080) m_protectStatusCounts[0x0080]++; // BIT7: 预留
        if (protectStatus & 0x0100) m_protectStatusCounts[0x0100]++; // BIT8: 电芯充电高温保护
        if (protectStatus & 0x0200) m_protectStatusCounts[0x0200]++; // BIT9: 电芯放电高温保护
        if (protectStatus & 0x0400) m_protectStatusCounts[0x0400]++; // BIT10: 电芯充电低温保护
        if (protectStatus & 0x0800) m_protectStatusCounts[0x0800]++; // BIT11: 电芯放电低温保护
        if (protectStatus & 0x1000) m_protectStatusCounts[0x1000]++; // BIT12: 环境(PCB)高温保护
        if (protectStatus & 0x2000) m_protectStatusCounts[0x2000]++; // BIT13: 环境(PCB)低温保护
        if (protectStatus & 0x4000) m_protectStatusCounts[0x4000]++; // BIT14: 功率器备温保护
        if (protectStatus & 0x8000) m_protectStatusCounts[0x8000]++; // BIT15: 预留

        // 告警状态统计 - 根据附表1
        uint16_t alarmStatus = data.alarmStatus;
        if (alarmStatus & 0x0001) m_alarmStatusCounts[0x0001]++; // BIT0: 单体高压告警
        if (alarmStatus & 0x0002) m_alarmStatusCounts[0x0002]++; // BIT1: 单体低压告警
        if (alarmStatus & 0x0004) m_alarmStatusCounts[0x0004]++; // BIT2: 总体高压告警
        if (alarmStatus & 0x0008) m_alarmStatusCounts[0x0008]++; // BIT3: 总体低压告警
        if (alarmStatus & 0x0010) m_alarmStatusCounts[0x0010]++; // BIT4: 充电过流告警
        if (alarmStatus & 0x0020) m_alarmStatusCounts[0x0020]++; // BIT5: 放电过流告警
        if (alarmStatus & 0x0040) m_alarmStatusCounts[0x0040]++; // BIT6: 预留
        if (alarmStatus & 0x0080) m_alarmStatusCounts[0x0080]++; // BIT7: 预留
        if (alarmStatus & 0x0100) m_alarmStatusCounts[0x0100]++; // BIT8: 电芯充电高温告警
        if (alarmStatus & 0x0200) m_alarmStatusCounts[0x0200]++; // BIT9: 电芯放电高温告警
        if (alarmStatus & 0x0400) m_alarmStatusCounts[0x0400]++; // BIT10: 电芯充电低温告警
        if (alarmStatus & 0x0800) m_alarmStatusCounts[0x0800]++; // BIT11: 电芯放电低温告警
        if (alarmStatus & 0x1000) m_alarmStatusCounts[0x1000]++; // BIT12: 环境(PCB)高温告警
        if (alarmStatus & 0x2000) m_alarmStatusCounts[0x2000]++; // BIT13: 环境(PCB)低温告警
        if (alarmStatus & 0x4000) m_alarmStatusCounts[0x4000]++; // BIT14: 功率器备高温告警
        if (alarmStatus & 0x8000) m_alarmStatusCounts[0x8000]++; // BIT15: 电量告警
    }
}
