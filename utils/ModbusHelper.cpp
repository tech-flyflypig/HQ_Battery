#include "ModbusHelper.h"
#include <QList>
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
    static const quint16 crc_table[] =
    {
        0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241, 0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,
        0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40, 0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,
        0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40, 0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,
        0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641, 0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,
        0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240, 0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,
        0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41, 0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,
        0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41, 0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
        0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640, 0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,
        0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240, 0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,
        0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41, 0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,
        0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41, 0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,
        0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640, 0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,
        0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241, 0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,
        0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40, 0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
        0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40, 0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,
        0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641, 0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040
    };

    quint16 crc = 0xFFFF;
    for (char byte : data)
    {
        crc = (crc >> 8) ^ crc_table[(crc ^ static_cast<quint8>(byte)) & 0xFF];
    }
    return crc;
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
