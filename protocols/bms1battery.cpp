#include "bms1battery.h"
#include "../utils/ModbusHelper.h"

void BMS1Battery::configure()
{
    // 配置处理器，例如初始化batteryData的默认值
    batteryData = BMS_1();
}

void BMS1Battery::processSerialData(const QByteArray &data)
{
    // TODO: 实现Modbus响应数据的解析
    // 1. 检查响应的有效性
    // 2. 解析数据到batteryData结构体
    // 3. 发送解析后的数据
    emit batteryDataProcessed(batteryData);
}

QByteArray BMS1Battery::getQueryCommand() const
{
    // 使用ModbusHelper来创建标准的Modbus查询命令
    return ModbusHelper::createModbusCommand(SLAVE_ADDR, READ_HOLDINGS,
                             DATA_START_REG, DATA_REG_COUNT);
}

QByteArray BMS1Battery::getControlCommand(uint16_t reg, uint16_t value) const
{
    if (!isValidControlValue(reg, value)) {
        return QByteArray();
    }
    // 使用ModbusHelper来创建标准的Modbus控制命令
    return ModbusHelper::createModbusCommand(SLAVE_ADDR, WRITE_MULTIPLE, reg, value);
}

bool BMS1Battery::isValidControlValue(uint16_t reg, uint16_t value) const
{
    // 检查寄存器地址是否有效
    if (reg != CHARGE_REG && reg != DISCHARGE_REG) {
        return false;
    }
    
    // 检查控制值是否有效
    if (value != COMMAND_ENABLE && value != COMMAND_DISABLE) {
        return false;
    }
    
    return true;
}
