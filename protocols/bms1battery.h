#ifndef BMS1BATTERY_H
#define BMS1BATTERY_H

#include "batteryinterface.h"
#include <QtCore/QString>

class BMS1Battery : public BatteryInterface
{
    Q_OBJECT

public:
    // 数据处理接口实现
    void processSerialData(const QByteArray &data) override;
    void configure() override;
    QString getProcessorName() const override { return "BMS1"; }

    // 查询命令接口实现
    QByteArray getQueryCommand() const override;
    
    // 控制命令接口实现
    bool supportsControl() const override { return true; }
    QByteArray getControlCommand(uint16_t reg, uint16_t value) const override;
    bool isValidControlValue(uint16_t reg, uint16_t value) const override;

private:
    // Modbus参数
    static const uint8_t SLAVE_ADDR = 0x01;
    static const uint8_t READ_HOLDINGS = 0x03;
    static const uint8_t WRITE_MULTIPLE = 0x10;
    
    // 数据寄存器
    static const uint16_t DATA_START_REG = 128;
    static const uint16_t DATA_REG_COUNT = 67;
    
    // 控制寄存器
    static const uint16_t CHARGE_REG = 1;
    static const uint16_t DISCHARGE_REG = 2;
    static const uint16_t COMMAND_ENABLE = 0xaaaa;
    static const uint16_t COMMAND_DISABLE = 0x5555;

    BMS_1 batteryData;
};

#endif // BMS1BATTERY_H
