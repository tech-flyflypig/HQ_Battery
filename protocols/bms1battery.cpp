#include "bms1battery.h"
#include "ModbusHelper.h"
#include <QDebug>
#include <QThread>
// 构造函数初始化
BMS1Battery::BMS1Battery()
{
    // 在构造函数体中直接赋值，而不是使用初始化列表
    SLAVE_ADDR = 0x01;
    READ_HOLDINGS = 0x03;
    WRITE_MULTIPLE = 0x10;
    DATA_START_REG = 128;
    DATA_REG_COUNT = 122;
    CHARGE_REG = 1;
    DISCHARGE_REG = 2;
    COMMAND_ENABLE = 0xaaaa;
    COMMAND_DISABLE = 0x5555;
}

void BMS1Battery::configure()
{
    // 配置处理器，例如初始化batteryData的默认值
    batteryData = BMS_1();
}

void BMS1Battery::processSerialData(const QByteArray &data)
{
    // qDebug() << "接收：" << data.toHex();
    // 1. 检查响应的有效性
    if (!ModbusHelper::validateResponse(data, SLAVE_ADDR, READ_HOLDINGS))
    {
        return; // 响应无效，直接返回
    }

    // 2. 解析数据到寄存器值列表
    QList<uint16_t> registers = ModbusHelper::parseReadResponse(data);

    // 检查数据长度是否符合预期
    if (registers.size() < DATA_REG_COUNT)
    {
        return; // 数据不完整，直接返回
    }

    // 3. 解析数据到batteryData结构体
    // 从寄存器128开始，每个参数占2个字节

    // 电流，寄存器128，16位有符号整数，单位0.01A
    batteryData.current = static_cast<int16_t>(registers[0]);

    // 电池组电压，寄存器129，16位无符号整数，单位0.01V
    batteryData.voltage = registers[1];

    // SOC，寄存器130，16位无符号整数，单位%
    batteryData.soc = registers[2];

    // SOH，寄存器131，16位无符号整数，单位%
    batteryData.soh = registers[3];

    // 剩余容量，寄存器132，16位无符号整数，单位0.01Ah
    batteryData.remainCapacity = registers[4];

    // 满充容量，寄存器133，16位无符号整数，单位0.01Ah
    batteryData.fullChargeCapacity = registers[5];

    // 额定容量，寄存器134，16位无符号整数，单位0.01Ah
    batteryData.ratedCapacity = registers[6];

    // 循环次数，寄存器135，16位无符号整数
    batteryData.cycleCount = registers[7];

    // 告警状态，寄存器137，16位无符号整数
    batteryData.alarmStatus = registers[9];

    // 保护状态，寄存器138，16位无符号整数
    batteryData.protectStatus = registers[10];

    // 故障状态，寄存器139，16位无符号整数
    batteryData.faultStatus = registers[11];

    // 系统状态，寄存器140，16位无符号整数
    batteryData.systemStatus = registers[12];

    // 功能开关状态，寄存器141，16位无符号整数
    batteryData.functionSwitchStatus = registers[13];

    // 电池组串数，寄存器145，16位无符号整数
    batteryData.cellCount = registers[17];

    // 单体最大值，寄存器146，16位无符号整数，单位mV
    batteryData.cellVoltageMax = registers[18];

    // 单体最小值，寄存器147，16位无符号整数，单位mV
    batteryData.cellVoltageMin = registers[19];

    // 电芯温度个数，寄存器148，16位无符号整数
    batteryData.tempSensorCount = registers[20];

    // 电芯温度最大值，寄存器149，16位有符号整数，单位0.1°C
    batteryData.tempMax = static_cast<int16_t>(registers[21]);

    // 电芯温度最小值，寄存器150，16位有符号整数，单位0.1°C
    batteryData.tempMin = static_cast<int16_t>(registers[22]);

    // 功率管温度，寄存器151，16位有符号整数，单位0.1°C
    batteryData.powerTempValue = static_cast<int16_t>(registers[23]);

    // 环境温度，寄存器152，16位有符号整数，单位0.1°C
    batteryData.ambientTemp = static_cast<int16_t>(registers[24]);

    // 提取单体电压数据
    int cellVoltageStartIndex = 27; // 寄存器155开始 186结束
    for (int i = 0; i < 32 && (cellVoltageStartIndex + i) < registers.size(); i++)
    {
        batteryData.cellVoltage[i] = registers[cellVoltageStartIndex + i];
    }

    // 提取电芯温度数据
    int cellTempStartIndex = cellVoltageStartIndex + 32; // 电压数据固定占用32位
    for (int i = 0; i < 8 && i < batteryData.tempSensorCount && (cellTempStartIndex + i) < registers.size(); i++)
    {
        batteryData.cellTemp[i] = static_cast<int16_t>(registers[cellTempStartIndex + i]);
    }

    //预留5位
    // 均衡状态，位于温度数据之后13位
    int balanceStatusIndex = cellTempStartIndex + 13;
    if (balanceStatusIndex < registers.size())
    {
        batteryData.balanceStatus = registers[balanceStatusIndex];
    }
    //xxx 暂时先不读取，需要的时候再读取
    //
    // // BMS版本信息
    // if (balanceStatusIndex + 1 < registers.size()) {
    //     batteryData.bmsVersion = registers[balanceStatusIndex + 1];
    // }

    // // BMS生产信息
    // if (balanceStatusIndex + 2 < registers.size()) {
    //     batteryData.bmsProductionInfo = registers[balanceStatusIndex + 2];
    // }

    // // PACK生产信息
    // if (balanceStatusIndex + 3 < registers.size()) {
    //     batteryData.packProductionInfo = registers[balanceStatusIndex + 3];
    // }

    // 4. 发送解析后的数据
    qDebug() << "=== BMS1Battery Signal Emission ===";
    qDebug() << "Thread ID:" << QThread::currentThreadId();
    qDebug() << "BMS1Battery object at:" << this;
    qDebug() << "Parent:" << this->parent();
    qDebug() << "MetaObject:" << this->metaObject()->className();
    qDebug() << "Signal Index:" << this->metaObject()->indexOfSignal("batteryDataProcessed(BMS_1)");
    qDebug() << "Data Values:";
    qDebug() << "  - SOC:" << batteryData.soc;
    qDebug() << "  - Voltage:" << batteryData.voltage;
    qDebug() << "  - Current:" << batteryData.current;
    qDebug() << "  - Status:" << batteryData.systemStatus;
    qDebug() << "Emitting signal...";
    emit batteryDataProcessed(batteryData);
    qDebug() << "Signal emitted";
    qDebug() << "================================";
}

QByteArray BMS1Battery::getQueryCommand() const
{
    // 使用ModbusHelper来创建标准的Modbus查询命令
    return ModbusHelper::createModbusCommand(SLAVE_ADDR, READ_HOLDINGS,
            128, DATA_REG_COUNT);
}

QByteArray BMS1Battery::getControlCommand(uint16_t reg, uint16_t value) const
{
    if (!isValidControlValue(reg, value))
    {
        return QByteArray();
    }
    // 使用ModbusHelper来创建标准的Modbus控制命令
    return ModbusHelper::createModbusCommand(SLAVE_ADDR, WRITE_MULTIPLE, reg, value);
}

bool BMS1Battery::isValidControlValue(uint16_t reg, uint16_t value) const
{
    // 检查寄存器地址是否有效
    if (reg != CHARGE_REG && reg != DISCHARGE_REG)
    {
        return false;
    }

    // 检查控制值是否有效
    if (value != COMMAND_ENABLE && value != COMMAND_DISABLE)
    {
        return false;
    }

    return true;
}

// 添加Modbus协议工具函数的实现
QByteArray BMS1Battery::createModbusCommand(uint8_t slave, uint8_t func,
        uint16_t addr, uint16_t value) const
{
    // 直接调用ModbusHelper的静态方法
    return ModbusHelper::createModbusCommand(slave, func, addr, value);
}

quint16 BMS1Battery::calculateCRC16(const QByteArray &data) const
{
    // 直接调用ModbusHelper的静态方法
    return ModbusHelper::calculateCRC16(data);
}
