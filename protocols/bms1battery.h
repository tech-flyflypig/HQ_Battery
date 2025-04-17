#ifndef BMS1BATTERY_H
#define BMS1BATTERY_H

#include "batteryinterface.h"
#include <QtCore/QString>
#include "Struct.h"

class BMS1Battery : public BatteryInterface
{
    Q_OBJECT

public:
    BMS1Battery();

    // 数据处理接口实现
    void processSerialData(const QByteArray &data) override;
    void configure() override;
    QString getProcessorName() const override
    {
        return "BMS1";
    }

    // 查询命令接口实现
    QByteArray getQueryCommand() const override;

    // 控制命令接口实现
    bool supportsControl() const override
    {
        return true;
    }
    QByteArray getControlCommand(uint16_t reg, uint16_t value) const override;
    bool isValidControlValue(uint16_t reg, uint16_t value) const override;

private:
    // Modbus参数
    uint8_t SLAVE_ADDR;
    uint8_t READ_HOLDINGS;
    uint8_t WRITE_MULTIPLE;

    // 数据寄存器
    uint16_t DATA_START_REG;
    int DATA_REG_COUNT;

    // 控制寄存器
    uint16_t CHARGE_REG;
    uint16_t DISCHARGE_REG;
    uint16_t COMMAND_ENABLE;
    uint16_t COMMAND_DISABLE;

    // Modbus协议工具函数
    QByteArray createModbusCommand(uint8_t slave, uint8_t func,
                                   uint16_t addr, uint16_t value) const;
    quint16 calculateCRC16(const QByteArray &data) const;

    BMS_1 batteryData;

signals:
    void batteryDataProcessed(const BMS_1 &batteryData);
};

#endif // BMS1BATTERY_H
