#include "serialdataprocessorfactory.h"
#include "bms1processor.h"

SerialDataProcessor *SerialDataProcessorFactory::createProcessor(const QString &productType)
{
    if (productType == "BMS1")
    {
        return new BMS1Processor();
    }
    // else if (productType == "ProductB")
    // {
    //     return new ProductBProcessor();
    // }
    return nullptr;
}
