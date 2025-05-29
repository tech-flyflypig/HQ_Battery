#ifndef STRUCT_H
#define STRUCT_H
#include <QString>
#include <QByteArray>
#include <QPoint>
#include <QVariant>
#include <QDateTime>
struct battery_info
{
    QString power_id;
    QString type;
    QString site;
    QString port_name;
    int baud_rate = 9600;
    int data_bits = 8;
    int stop_bits = 1;
    int parity = 0;

//    float voltage;
//    float current;
//    float temperature;
    int status; // 0: 正常, 1: 通信错误, 2: 超时
    QDateTime last_time; //最后一次数据上报时间

    int x;
    int y;
    QPoint pos;
    float bcdy = 60; //标称电压
    float bcdl = 60; //标称电流
    float bcwd = 50; //标称温度
    float bcsysj = 2500;
};
struct BMS_1
{

    battery_info battery_info;
    int16_t current;            // 电流, 0.01A, 充电为正，放电为负
    uint16_t voltage;           // 电池组电压, 0.01V
    uint16_t soc;               // SOC, %, State of Charge
    uint16_t soh;               // SOH, %, State of Health
    uint16_t remainCapacity;    // 剩余容量, 0.01Ah
    uint16_t fullChargeCapacity; // 满充容量, 0.01Ah
    uint16_t ratedCapacity;     // 额定容量, 0.01Ah
    uint16_t cycleCount;        // 循环次数, Cycles

    uint16_t alarmStatus;       // 告警状态, Hex, 见附表1
    uint16_t protectStatus;     // 保护状态, Hex, 见附表2
    uint16_t faultStatus;       // 故障状态, Hex, 见附表3
    uint16_t systemStatus;      // 系统状态, Hex, 见附表4
    uint16_t functionSwitchStatus; // 功能开关状态, Hex, 见附表5

    uint16_t cellCount;         // 电池组串数
    uint16_t cellVoltageMax;    // 单体最大值, mV
    uint16_t cellVoltageMin;    // 单体最小值, mV
    uint16_t tempSensorCount;   // 电芯温度个数
    int16_t tempMax;            // 电芯温度最大值, 0.1°C
    int16_t tempMin;            // 电芯温度最小值, 0.1°C
    int16_t powerTempValue;     // 功率管温度, 0.1°C, 未检测上传0x8000
    int16_t ambientTemp;        // 环境温度, 0.1°C, 未检测上传0x8000

    //电芯单体电压
    uint16_t cellVoltage[32];  // 固定占用32位
    //电芯温度
    int16_t cellTemp[8];  // 固定占用8位


    uint16_t balanceStatus;     // 均衡状态
    uint8_t bmsVersionInfo[20]; // BMS版本信息占用20byte

    uint8_t bmsProductionInfo[20]; // BMS生产信息
    //PACK生产信息
    uint8_t packProductionInfo[20]; // PACK生产信息

    BMS_1()
        : current(0), voltage(0), soc(0), soh(0),
          remainCapacity(0), fullChargeCapacity(0), ratedCapacity(0), cycleCount(0),
          alarmStatus(0), protectStatus(0), faultStatus(0), systemStatus(0), functionSwitchStatus(0),
          cellCount(0), cellVoltageMax(0), cellVoltageMin(0), tempSensorCount(0),
          tempMax(0), tempMin(0), powerTempValue(0), ambientTemp(0),
          balanceStatus(0)
    {
        // 初始化数组
        memset(cellVoltage, 0, sizeof(cellVoltage));
        memset(cellTemp, 0, sizeof(cellTemp));
        memset(bmsVersionInfo, 0, sizeof(bmsVersionInfo));
        memset(bmsProductionInfo, 0, sizeof(bmsProductionInfo));
        memset(packProductionInfo, 0, sizeof(packProductionInfo));
    }

};

struct BMS_1_Protection
{
    // 总体电压告警和保护参数
    uint16_t totalVoltageOverAlarm;          // 总体过压告警, 0.01V
    uint16_t totalVoltageOverProtect;        // 总体过压保护, 0.01V
    uint16_t totalVoltageOverProtectRelease; // 总体过压保护恢复, 0.01V
    uint16_t totalVoltageOverProtectDelay;   // 总体过压保护延时, 0.1S
    uint16_t totalVoltageLowAlarm;           // 总体欠压告警, mV
    uint16_t totalVoltageLowProtect;         // 总体欠压保护, mV
    uint16_t totalVoltageLowProtectRelease;  // 总体欠压保护恢复, mV
    uint16_t totalVoltageLowProtectDelay;    // 总体欠压保护延时, 0.1S

    // 单体电压告警和保护参数
    uint16_t cellVoltageOverAlarm;           // 单体过压告警, 0.01V
    uint16_t cellVoltageOverProtect;         // 单体过压保护, 0.01V
    uint16_t cellVoltageOverProtectRelease;  // 单体过压保护恢复, 0.01V
    uint16_t cellVoltageOverProtectDelay;    // 单体过压保护延时, 0.1S
    uint16_t cellVoltageLowAlarm;            // 单体欠压告警, mV
    uint16_t cellVoltageLowProtect;          // 单体欠压保护, mV
    uint16_t cellVoltageLowProtectRelease;   // 单体欠压保护恢复, mV
    uint16_t cellVoltageLowProtectDelay;     // 单体欠压保护延时, 0.1S

    // 充电电流告警和保护参数
    uint16_t chargeCurrentOverAlarm;         // 充电过流告警, A
    uint16_t chargeCurrentOverProtect;       // 充电过流保护, A
    uint16_t chargeCurrentOverProtectDelay;  // 充电过流保护延时, 0.1S
    uint16_t chargeCurrentProtectRelease;    // 充电过流—恢复保护, A
    uint16_t chargeCurrentProtectReleaseDelay; // 充电过流—恢复保护延时, 0.01S
    uint16_t dischargeCurrentOverAlarm;      // 放电过流告警, A
    uint16_t dischargeCurrentOverProtect;    // 放电过流保护, A
    uint16_t dischargeCurrentOverProtectDelay; // 放电过流保护延时, 0.1S
    uint16_t dischargeCurrentProtectRelease; // 放电过流—恢复保护, A
    uint16_t dischargeCurrentProtectReleaseDelay; // 放电过流—恢复保护延时, 0.01S

    // 短路保护参数
    uint16_t shortCircuitProtect;            // 短路保护电流, A
    uint16_t shortCircuitProtectDelay;       // 短路保护延时, 0.01ms

    // 温度保护参数
    int16_t chargeTempHighAlarm;            // 充电高温告警, 0.1°C
    int16_t chargeTempHighProtect;          // 充电高温保护, 0.1°C
    int16_t chargeTempHighProtectRelease;   // 充电高温保护恢复, 0.1°C
    int16_t chargeTempLowAlarm;             // 充电低温告警, 0.1°C
    int16_t chargeTempLowProtect;           // 充电低温保护, 0.1°C
    int16_t chargeTempLowProtectRelease;    // 充电低温保护恢复, 0.1°C
    int16_t dischargeTempHighAlarm;         // 放电高温告警, 0.1°C
    int16_t dischargeTempHighProtect;       // 放电高温保护, 0.1°C
    int16_t dischargeTempHighProtectRelease; // 放电高温保护恢复, 0.1°C
    int16_t dischargeTempLowAlarm;          // 放电低温告警, 0.1°C
    int16_t dischargeTempLowProtect;        // 放电低温保护, 0.1°C
    int16_t dischargeTempLowProtectRelease; // 放电低温保护恢复, 0.1°C

    // 功率管温度保护参数
    int16_t powerTempHighAlarm;             // 功率管高温告警, 0.1°C
    int16_t powerTempHighProtect;           // 功率管高温保护, 0.1°C
    int16_t powerTempHighProtectRelease;    // 功率管高温保护恢复, 0.1°C

    // 环境温度保护参数
    int16_t environmentTempHighAlarm;       // 环境高温告警, 0.1°C
    int16_t environmentTempHighProtect;     // 环境高温保护, 0.1°C
    int16_t environmentTempHighProtectRelease; // 环境高温保护恢复, 0.1°C
    int16_t environmentTempLowAlarm;        // 环境低温告警, 0.1°C
    int16_t environmentTempLowProtect;      // 环境低温保护, 0.1°C
    int16_t environmentTempLowProtectRelease; // 环境低温保护恢复, 0.1°C

    // 均衡参数
    uint16_t balanceStartVoltage;           // 均衡开启电压, mV
    uint16_t balanceStartDeltaVoltage;      // 均衡开启压差, mV
    uint16_t fullChargeVoltage;             // 充满判断电压, 0.01V
    uint16_t fullChargeCurrent;             // 充满判断电流, mA
    uint16_t cellVoltageSleepValue;         // 单体低压休眠电压, mV
    uint16_t cellVoltageSleepTime;          // 单体低压休眠时间, min
    uint16_t staticBalanceTimeout;          // 待机休眠时间, 0.1hr
    uint16_t lowPowerWarning;               // 低电量警告值, %
    uint16_t overDischargeRecoveryDelta;    // 过压保护容量比例恢复, %
    int16_t powerOnTemperature;            // 加热膜开启温度, 0.1°C
    int16_t powerOffTemperature;           // 加热膜关闭温度, 0.1°C
};

#if 0
struct UPS1_YC
{
    int pack ;                    // pack组 packe1为主机
    int   BatteryNumber ;         // 单体电池数量
    QList<float>  List_Voltage ;  // 电压数组 与电池数量有关

    int   TemperatureNumber ;
    QList<float>  List_Temperature ;

    float  TemperatureHJ ;
    float TemperatureP ;

    float Current ;

    float VoltageTotal ;

    int BatterySurplus ;          // 电池剩余容量 AH

    int YCNumber ;                  // 自定义遥测数量 2.0版本为10,2.6版本仅有2

    int BatteryCapacity ;         // 电池总容量  AH  2.6
    int SOC;                      //剩余电量  % /10
    int BatteryRate;                //额定容量  /100 AH  2.6
    int  BatteryCircleTimes ;       //循环次数
    int SOH;                      //电池的健康度 % /10
    float BusVoltage;              //母线电压  /100
    float DriftCurrent;             //温漂电流 /1000
    float ZeroCurrent;              //零点电流 此为第8个遥测量,剩余两个未知 /1000


//             system_time_year ;   // 年
//             system_time_month = ""  // 月
//             system_time_day = ""  // 日
//             system_time_hour = ""  // 时
//             system_time_minute = ""  // 分
//             system_time_second = ""  // 秒
//             CellVoltage ;
};
struct UPS1_YX
{
    int   BatteryNumber ;         // 单体电池数量
    QList<int>   List_Voltage_status ;  // 电压状态数组
    int   TemperatureNumber ;
    QList<int>  List_Temperature_status ;  // 温度状态数组
    int  TemperatureHJ_status ;
    int TemperatureP_status ;
    int Current_status ;
    int VoltageTotal_status ;
    int  BatterySurplus_status ;
    int AlarmNumber ;               // 自定义告警量数量
    QList<int> Event_Equilibrium ;  // 均衡事件
    QList<int> Event_Voltage ;      // 电压事件
    QList<int> Event_Temperature ;  // 温度事件
    QList<int>  Event_Current ;     // 电流事件

    QList<int> status_FET ;         // 开关事件代码
    QList<int> Status_Equilibrium ; // 均衡状态代码
    QList<int> Status_System ;      // 系统状态代码
};
struct UPS1
{
    battery_info battery_info;
    UPS1_YC ups1_yc;
    UPS1_YX ups1_yx;
};
struct UPS2
{
    battery_info battery_info;
    int PackVoltage ;               // 总电压 mV
    int PackCurrent ;               // 总电流 mA
    int Serial_Number ;             // 序列号
    int RelativeStateofCharge ;     // 电量的相对状态 %
    int AbsoluteStateofCharge ;     // 电量的绝对状态 %
    int RemainingCapacity = 0 ;       // 剩余电量  mAh
    int FullChargeCapacity = 1 ;      // 充满容量  mAh
    int RunTimeToEmpty ;            // 运行时间   minutes
    int AverageTimeToEmpty ;        // 平均可运行时间 minutes
    int AverageTimeToFull ;         // 平均充满时间 minutes
    int ChargingCurrent ;           // 充电电流 mA
    int ChargingVoltage ;           // 充电电压  mV
    int BatteryStatus ;             // 电池状态    16bit
    QStringList list_battery_status;// 电池状态列表
    int CycleCount ;                // 循环次数
    int DesignCapacity ;            // 设计容量  mAh
    int DesignVoltage ;             // 设计电压   mV
    int MaxTemperature ;            // 最高温度   Kelvin
    int BoardTemperature ;          // 电路板温度  Kelvin
    int ActualNumberofCells ;       // 实际电芯数量
    int TemperatureSensorCount ;    // 温度传感器数量
    int SafetyStatus ;              // 安全状态寄存器 16bit
    QStringList list_safety_status; // 安全状态列表
    int PermanentDisableStatus ;    // 永久失效状态寄存器 16bit
    QStringList list_permanentdisable_status; // 永久失效状态列表
    int ChargeStatus ;              // 充电状态寄存器  16bit
    QStringList list_charge_status; // 永久失效状态列表
    int PermanentDisableStatus1 ;
    int PermanentDisableStatus2 ;   //温度检测线断线状态寄存器 16bit
    int SystemStatus1 ;
    int SafetyStatus1 ;             // 安全状态寄存器1 16bit

};

struct UPS2_Cell_Dynamic
{
    QList<int> CellVoltage;         //电芯电压 mV
    QList<int> CellTemperature ;   //电芯温度 Kelvin
    //以下参数太专业，可不读取
    QList<int> HF_Impedance ;      //高频阻抗  mohm
    QList<int> OCV_SOC;            //开路电压脉冲 %
    QList<int> EstOCV ;                    //mV

    QList<int> CellStatus ;                //电芯状态 16bit
};
#endif

struct USER
{
    int rowid;
    QString user_name;
    QString password;
    int privilege;
};
struct SetParam
{
    float temperature;
    float voltage;
    float current;
    float capacity;
};


Q_DECLARE_METATYPE(battery_info);

// Q_DECLARE_METATYPE(JHJ_Param);
// Q_DECLARE_METATYPE(UPS1);
// Q_DECLARE_METATYPE(UPS2);
// Q_DECLARE_METATYPE(UPS2_Cell_Dynamic);
Q_DECLARE_METATYPE(BMS_1);
#endif // STRUCT_H
