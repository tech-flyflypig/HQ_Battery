#ifndef BATTERYINTERFACEFACTORY_H
#define BATTERYINTERFACEFACTORY_H

#include "batteryinterface.h"
#include "bms1battery.h"
#include <QString>

class BatteryInterfaceFactory
{
public:
    static BatteryInterface *createBattery(const QString &productType)
    {
        if (productType == "BMS_1")
        {
            return new BMS1Battery();
        }
        // 后续可以添加其他电池类型
        return nullptr;
    }
};

#endif // BATTERYINTERFACEFACTORY_H
