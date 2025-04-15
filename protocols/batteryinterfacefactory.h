#ifndef BATTERYINTERFACEFACTORY_H
#define BATTERYINTERFACEFACTORY_H

#include "batteryinterface.h"
#include <QString>

class BatteryInterfaceFactory
{
public:
    static BatteryInterface *createBattery(const QString &productType);
};

#endif // BATTERYINTERFACEFACTORY_H
