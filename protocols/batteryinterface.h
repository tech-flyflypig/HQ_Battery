#ifndef BATTERYINTERFACE_H
#define BATTERYINTERFACE_H
#include <QObject>
#include <QString>
#include "Struct.h"

class BatteryInterface : public QObject
{
    Q_OBJECT

public:
    virtual ~BatteryInterface() = default;

    // 数据处理
    virtual void processSerialData(const QByteArray &data) = 0;
    virtual void configure() = 0;
    virtual QString getProcessorName() const = 0;

    // 查询命令
    virtual QByteArray getQueryCommand() const = 0;
    
    // 控制命令接口
    virtual bool supportsControl() const { return false; }
    virtual QByteArray getControlCommand(uint16_t reg, uint16_t value) const { return QByteArray(); }
    virtual bool isValidControlValue(uint16_t reg, uint16_t value) const { return false; }

signals:
    void batteryDataProcessed(const BMS_1 &batteryData);
};

#endif // BATTERYINTERFACE_H
