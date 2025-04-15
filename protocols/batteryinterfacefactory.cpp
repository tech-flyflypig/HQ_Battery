#include "batteryinterfacefactory.h"
#include "bms1interface.h"

BatteryInterface* BatteryInterfaceFactory::createBattery(const QString &productType)
{
    if (productType == "BMS1")
    {
        return new BMS1Interface();
    }
    // 后续可以添加其他电池类型
    return nullptr;
}
