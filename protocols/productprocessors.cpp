#include "productprocessors.h"

static SerialDataProcessor *createProcessor(const QString &productType)
{
    if (productType == "ProductA")
    {
        return new ProductAProcessor();
    }
    else if (productType == "ProductB")
    {
        return new ProductBProcessor();
    }
    return nullptr;
}
