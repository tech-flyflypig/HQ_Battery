#ifndef PRODUCTPROCESSORS_H
#define PRODUCTPROCESSORS_H
#include "serialdataprocessor.h"
#include <QString>


class SerialDataProcessorFactory
{
public:
    static SerialDataProcessor *createProcessor(const QString &productType);
};

#endif // PRODUCTPROCESSORS_H
