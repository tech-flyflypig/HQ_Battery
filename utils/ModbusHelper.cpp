#include "ModbusHelper.h"
#include <QList>
#include <QDebug>
ModbusHelper::ModbusHelper()
{
}

ModbusHelper::~ModbusHelper()
{
}

QByteArray ModbusHelper::createReadCommand(uint8_t slave, uint16_t addr, uint16_t count)
{
    return createModbusCommand(slave, READ_HOLDINGS, addr, count);
}

QByteArray ModbusHelper::createWriteCommand(uint8_t slave, uint16_t addr, uint16_t value)
{
    return createModbusCommand(slave, WRITE_SINGLE_REGISTER, addr, value);
}

QByteArray ModbusHelper::createWriteMultipleCommand(uint8_t slave, uint16_t addr, const QList<uint16_t> &values)
{
    // 此方法需要特殊实现，暂不提供
    return QByteArray();
}

QByteArray ModbusHelper::createModbusCommand(uint8_t slave, uint8_t func,
        uint16_t addr, uint16_t value)
{
    QByteArray cmd;
    cmd.append(static_cast<char>(slave));    // 从机地址
    cmd.append(static_cast<char>(func));     // 功能码
    cmd.append(static_cast<char>(addr >> 8));   // 寄存器地址高字节
    cmd.append(static_cast<char>(addr & 0xFF)); // 寄存器地址低字节

    if (func == READ_HOLDINGS || func == READ_INPUT_REGISTERS ||
            func == READ_COILS || func == READ_DISCRETE_INPUTS)
    {
        // 读取命令：附加读取长度
        cmd.append(static_cast<char>(value >> 8));
        cmd.append(static_cast<char>(value & 0xFF));
    }
    else if (func == WRITE_SINGLE_REGISTER || func == WRITE_SINGLE_COIL)
    {
        // 写单个寄存器命令：附加写入值
        cmd.append(static_cast<char>(value >> 8));
        cmd.append(static_cast<char>(value & 0xFF));
    }
    else if (func == WRITE_MULTIPLE)
    {
        // 写多个寄存器命令：附加寄存器数量、字节数和写入值
        cmd.append(static_cast<char>(0x00));     // 寄存器数量高字节（1个寄存器）
        cmd.append(static_cast<char>(0x01));     // 寄存器数量低字节
        cmd.append(static_cast<char>(0x02));     // 字节数（2字节）
        cmd.append(static_cast<char>(value >> 8));   // 写入值高字节
        cmd.append(static_cast<char>(value & 0xFF)); // 写入值低字节
    }

    // 计算并添加CRC校验
    quint16 crc = calculateCRC16(cmd);
    cmd.append(static_cast<char>(crc & 0xFF));    // CRC低字节
    cmd.append(static_cast<char>(crc >> 8));      // CRC高字节

    return cmd;
}

quint16 ModbusHelper::calculateCRC16(const QByteArray &data)
{
    quint16 wCRC = 0xFFFF;
    const quint16 sCRCTable[] =
    {
        0x0000, 0xCC01, 0xD801, 0x1400, 0xF001, 0x3C00, 0x2800, 0xE401,
        0xA001, 0x6C00, 0x7800, 0xB401, 0x5000, 0x9C01, 0x8801, 0x4400,
    };

    for (int i = 0; i < data.size(); i++)
    {
        quint8 chChar = static_cast<quint8>(data.at(i));
        wCRC = sCRCTable[(chChar ^ wCRC) & 0x0F] ^ (wCRC >> 4);
        wCRC = sCRCTable[((chChar >> 4) ^ wCRC) & 0x0F] ^ (wCRC >> 4);
    }

    return wCRC;
}

bool ModbusHelper::validateResponse(const QByteArray &data, uint8_t expectedSlave, uint8_t expectedFunc)
{
    // 检查数据长度是否合法
    if (data.size() < 4)   // 至少需要：从机地址(1) + 功能码(1) + 数据(至少0) + CRC(2)
    {
        return false;
    }

    // 检查从机地址和功能码
    uint8_t slave = static_cast<uint8_t>(data.at(0));
    uint8_t func = static_cast<uint8_t>(data.at(1));

    if (slave != expectedSlave)
    {
        return false;
    }

    // 检查是否为异常响应（Modbus异常响应的功能码会将最高位置1）
    if ((func & 0x80) == 0x80)
    {
        return false;
    }

    if (func != expectedFunc)
    {
        return false;
    }

    // 检查CRC校验
    size_t dataLen = data.size();
    QByteArray dataWithoutCRC = data.left(dataLen - 2);
    quint16 calcCRC = calculateCRC16(dataWithoutCRC);
    quint16 recvCRC = (static_cast<uint8_t>(data.at(dataLen - 1)) << 8) |
                      static_cast<uint8_t>(data.at(dataLen - 2));

    if (calcCRC != recvCRC)
    {
        return false;
    }

    return true;
}

QList<uint16_t> ModbusHelper::parseReadResponse(const QByteArray &data)
{
    QList<uint16_t> result;

    // 检查响应数据是否合法
    if (data.size() < 5)   // 至少需要：从机地址(1) + 功能码(1) + 字节数(1) + 数据(至少0) + CRC(2)
    {
        return result;
    }

    // 获取字节数
    uint8_t byteCount = static_cast<uint8_t>(data.at(2));

    // 检查长度
    if (data.size() < 3 + byteCount + 2)   // 3(头部) + 数据长度 + 2(CRC)
    {
        return result;
    }

    // 解析寄存器值
    for (int i = 0; i < byteCount; i += 2)
    {
        if (i + 4 < data.size())   // 确保不会越界
        {
            uint16_t value = (static_cast<uint8_t>(data.at(3 + i)) << 8) |
                             static_cast<uint8_t>(data.at(3 + i + 1));
            result.append(value);
        }
    }

    return result;
}

bool ModbusHelper::parseWriteResponse(const QByteArray &data)
{
    // 检查写响应是否有效
    // 写响应格式：从机地址(1) + 功能码(1) + 寄存器地址(2) + 写入值/数量(2) + CRC(2)
    if (data.size() != 8)
    {
        return false;
    }

    // 校验工作已经在validateResponse中完成，如果通过了校验，写入响应就是成功的
    return true;
}
