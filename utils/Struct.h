#ifndef STRUCT_H
#define STRUCT_H
#include <QString>
#include <QByteArray>
#include <QPoint>
#include <QVariant>
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
//    int status;

    int x;
    int y;
    QPoint pos;
    float bcdy = 60; //标称电压
    float bcdl = 60; //标称电流
    float bcwd = 50; //标称温度
    float bcsysj = 2500;
};
struct JHJ_Param
{
    battery_info battery_info;
    int dclxbh; //电池类型编号
    float bcdy; //标称电压
    float bcrl; //标称容量
    float zddy; //主电电压
    float fddy; //辅电电压
    int cdzt = 0; //充电状态
    int fdzt = 0; //放电状态
    int cw1 = 0; // 电池异常状态 防爆腔内超温
    int cw2 = 0; // 电源模块超温
    int cw3 = 0; // 电池组超温
    int cw4 = 0; // 主设备超温
    int cw5 = 0; // 变压器超温
    int ljsysjcx = 0 ; // 累计使用时间超限
    int cfdcscx = 0; // 累计充放电次数超限
    int zdygy6 = 0; // 主电电压过高
    int zdydy6 = 0; // 主电电压过低
    int dycged = 0; // 电池电压过高
    int dydbd = 0; // 电池充电异常
    int cddlx = 0; // 充电电流小
    int jlzt = 0; // 交流状态
    float dy ;  // 电池组电压
    float dl ;  // 充放电电流
    float dymkzdy ;  // 主电电压
    float dymkfdy ;  // 辅助电压
    float wd1 ;  // 防爆腔内温度
    float wd2 ;  // 电源模块温度
    float wd3 ;  // 电池组温度
    float wd4 ;  // 主设备温度
    float wd5 ;  // 变压器温度
    int cfdcs ;// 充放电次数
    int ljsy_hour ;// 累计使用时间(小时)
    float dcrl ;// 电池实际容量 可能有损耗
    float dliang;//电池电量 百分比
    int bcfdsj ;//本次放电时间
    int year ;//年
    int month ;//  月
    int day ; // 日
    int hour ; // 小时
    int minute ; // 分钟
    int second ; // 秒
    int ljsy_day ;  // 累计使用时间(天)
    int scfdsj ; // 上次放电时间，EPOCH时间.单位为秒
    int fdsjjg_zd ; // 放电时间间隔
    int fdsk_h_zd ;  // 放电时刻（小时）
    int zdfd ;  // 自动放电
    int fdsk_m_zd ;  // 放电时刻（分钟）
    float jldy ;  // 交流电压
    int jldyd = 0; // 交流电压过低
    int jldyg = 0; // 交流电压过高
    int fzdygd = 0; // 辅助电压过低
    int fzdygg = 0; // 辅助电压过高
    float jldyedz; // 交流额定电压
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

#endif // STRUCT_H
