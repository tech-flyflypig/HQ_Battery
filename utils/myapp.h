#ifndef MYAPP_H
#define MYAPP_H
#include "Struct.h"
//struct configParam_jhj //交换机预设参数
//{
//    static float temp_fbq;     //防爆腔报警温度
//   static float temp_dymk;    //电源模块温度
//    float temp_dcz;     // 电池组温度
//    float temp_zsb;     // 主设备温度
//    float temp_dyq;     // 变压器温度
//    int times_cfd;      // 充放电次数
//    int ljsy_hour;     // 累计使用时间(小时)
//    float dcrl;        // 电池实际容量 可能有损耗
//};

// class configParam_jhj
// {
// public:
//     static float temp_fbq;     //防爆腔报警温度
//     static float temp_dymk;    //电源模块温度
//     static float temp_dcz;     // 电池组温度
//     static float temp_zsb;     // 主设备温度
//     static float temp_dyq;     // 变压器温度
//     static float alarm_voltage;       //报警电压阈值
//     static float alarm_current;       //报警电流阈值
//     static int times_cfd;      // 充放电次数
//     static int ljsy_hour;     // 累计使用时间(小时)
//     static float dcrl;        // 电池实际容量 可能有损耗
// };


// class configParam_ups1
// {
// public:
//     static float alarm_voltage;       //报警电压阈值
//     static float alarm_current;       //报警电流阈值
//     static float temp_dymk;    //电源模块温度 8/16块
//     static float temp_hj;    //环境温度阈值
//     static float temp_p;    //功率温度阈值
//     static int circletimes;      // 循环次数阈值
//     static float dcrl;        // 电池实际容量 可能有损耗
// };

// class configParam_common
// {
// public:
//     static float temperature;
//     static float voltage;
//     static float current;
//     static float capacity;
//     static int batterycircletimes;
// };

class myApp
{
public:
    static QString AppPath;
    static QString HostName;
    static int Port;
    static QString DatabaseName;
    static QString UserName;
    static QString Password;
    static QString BackGroundPath;
    static int LogFileCount;
    static QString CurrentLogFileName;

    //modbus ip
    static QString ModbusIp;

    static QStringList cd_powers;
    static QStringList fd_powers;

    static int run_mode;
    static int license;
    static QString kLibraryKey;
    static void init_param();
    static void ReadConfig();
    static void WriteConfig();
    static void SaveSql();
    static void Sleep(int msec);

    static QList<battery_info> back_up_battery;
    static QStringList JHJAlarmOnType;//交换机电池打开报警类型
    static QStringList UPS1AlarmOnType;//老UPS电池打开报警类型
    static QStringList UPS2AlarmOnType;//新UPS电池打开报警类型
    static bool alarm_ban;//一键屏蔽所有报警
    static bool modbus_mode;//modbus开启 对接龙软大屏modbus-tcp协议
};
void ManageLogFile();
void outputMessage(QtMsgType                 type,
                   const QMessageLogContext &context,
                   const QString            &msg);

#endif // MYAPP_H
