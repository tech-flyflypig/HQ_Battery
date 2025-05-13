#ifndef BATTERYSTATS_H
#define BATTERYSTATS_H

#include <QObject>
#include <QMap>
#include "Struct.h"

class BatteryStats : public QObject
{
    Q_OBJECT
    
public:
    // 单例模式
    static BatteryStats* instance();
    
    // 获取统计数据
    int getTotalBatteryCount() const;
    int getRunningBatteryCount() const;
    int getStoppedBatteryCount() const;
    int getFaultBatteryCount() const;
    
    // 保护状态统计 - 根据附表2修改
    int getSingleOvervoltageCount() const;    // BIT0: 单体过压保护
    int getSingleUndervoltageCount() const;   // BIT1: 单体欠压保护
    int getTotalOvervoltageCount() const;     // BIT2: 总体过压保护
    int getTotalUndervoltageCount() const;    // BIT3: 总体欠压保护
    int getChargingOvercurrentCount() const;  // BIT4: 充电过流保护
    int getDischargingOvercurrentCount() const; // BIT5: 放电过流保护
    int getShortCircuitCount() const;         // BIT6: 短路
    int getFirstReservedCount() const;        // BIT7: 预留
    int getCellChargingHighTempCount() const; // BIT8: 电芯充电高温保护
    int getCellDischargingHighTempCount() const; // BIT9: 电芯放电高温保护
    int getCellChargingLowTempCount() const;  // BIT10: 电芯充电低温保护
    int getCellDischargingLowTempCount() const; // BIT11: 电芯放电低温保护
    int getPcbHighTempCount() const;          // BIT12: 环境(PCB)高温保护
    int getPcbLowTempCount() const;           // BIT13: 环境(PCB)低温保护
    int getPowerEquipProtectCount() const;    // BIT14: 功率器备温保护
    int getSecondReservedCount() const;       // BIT15: 预留
    
    // 告警状态统计 - 根据附表1修改
    int getTotalAlarmCount() const;
    int getSingleHighVoltageAlarmCount() const;    // BIT0: 单体高压告警
    int getSingleLowVoltageAlarmCount() const;     // BIT1: 单体低压告警
    int getTotalHighVoltageAlarmCount() const;     // BIT2: 总体高压告警
    int getTotalLowVoltageAlarmCount() const;      // BIT3: 总体低压告警
    int getChargingOvercurrentAlarmCount() const;  // BIT4: 充电过流告警
    int getDischargingOvercurrentAlarmCount() const; // BIT5: 放电过流告警
    int getFirstAlarmReservedCount() const;        // BIT6: 预留
    int getSecondAlarmReservedCount() const;       // BIT7: 预留
    int getCellChargingHighTempAlarmCount() const; // BIT8: 电芯充电高温告警
    int getCellDischargingHighTempAlarmCount() const; // BIT9: 电芯放电高温告警
    int getCellChargingLowTempAlarmCount() const;  // BIT10: 电芯充电低温告警
    int getCellDischargingLowTempAlarmCount() const; // BIT11: 电芯放电低温告警
    int getPcbHighTempAlarmCount() const;          // BIT12: 环境(PCB)高温告警
    int getPcbLowTempAlarmCount() const;           // BIT13: 环境(PCB)低温告警
    int getPowerEquipHighTempAlarmCount() const;   // BIT14: 功率器备高温告警
    int getBatteryLowAlarmCount() const;           // BIT15: 电量告警
    
    // 更新电池状态（由BatteryListForm调用）
    void updateBatteryStatus(const QString& batteryId, const BMS_1& data);
    
    // 清除统计数据
    void clearStats();
    
signals:
    // 当统计数据发生变化时发出的信号
    void statsChanged();
    
private:
    explicit BatteryStats(QObject* parent = nullptr);
    ~BatteryStats();
    
    // 存储电池ID与状态的映射
    QMap<QString, BMS_1> m_batteryData;
    
    // 统计数据缓存
    int m_runningCount = 0;
    int m_stoppedCount = 0;
    int m_faultCount = 0;
    
    // 保护状态计数
    QMap<int, int> m_protectStatusCounts;
    
    // 告警状态计数
    QMap<int, int> m_alarmStatusCounts;
    
    // 计算统计数据
    void calculateStats();
    
    static BatteryStats* m_instance;
};

#endif // BATTERYSTATS_H 