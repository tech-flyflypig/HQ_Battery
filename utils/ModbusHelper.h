#ifndef MODBUSHELPER_H
#define MODBUSHELPER_H

#include <QByteArray>

class ModbusHelper
{
public:
    ModbusHelper();
    ~ModbusHelper();

    // Modbus功能码
    static const uint8_t READ_COILS = 0x01;
    static const uint8_t READ_DISCRETE_INPUTS = 0x02;
    static const uint8_t READ_HOLDINGS = 0x03;
    static const uint8_t READ_INPUT_REGISTERS = 0x04;
    static const uint8_t WRITE_SINGLE_COIL = 0x05;
    static const uint8_t WRITE_SINGLE_REGISTER = 0x06;
    static const uint8_t WRITE_MULTIPLE_COILS = 0x0F;
    static const uint8_t WRITE_MULTIPLE = 0x10;
    
    // 创建Modbus命令
    static QByteArray createReadCommand(uint8_t slave, uint16_t addr, uint16_t count);
    static QByteArray createWriteCommand(uint8_t slave, uint16_t addr, uint16_t value);
    static QByteArray createWriteMultipleCommand(uint8_t slave, uint16_t addr, const QList<uint16_t>& values);
    
    // 创建Modbus命令的通用方法
    static QByteArray createModbusCommand(uint8_t slave, uint8_t func, 
                                          uint16_t addr, uint16_t value);
    
    // CRC校验计算
    static quint16 calculateCRC16(const QByteArray& data);
    
    // Modbus响应数据解析
    static bool validateResponse(const QByteArray& data, uint8_t expectedSlave, uint8_t expectedFunc);
    static QList<uint16_t> parseReadResponse(const QByteArray& data);
    static bool parseWriteResponse(const QByteArray& data);
};

#endif // MODBUSHELPER_H
