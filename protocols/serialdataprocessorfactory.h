#ifndef SERIALDATAPROCESSORFACTORY_H
#define SERIALDATAPROCESSORFACTORY_H

#include "serialdataprocessor.h"
#include <QtCore/QString>

class SerialDataProcessorFactory
{
public:
    static SerialDataProcessor *createProcessor(const QString &productType);
};

#endif // SERIALDATAPROCESSORFACTORY_H
