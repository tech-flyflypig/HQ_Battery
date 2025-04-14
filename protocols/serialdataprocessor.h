#ifndef SERIALDATAPROCESSOR_H
#define SERIALDATAPROCESSOR_H

#include <QString>


class SerialDataProcessor
{
public:
    virtual ~SerialDataProcessor() = default;

    // 处理串口数据的纯虚函数
    virtual void processSerialData(const QByteArray &data) = 0;

    // 配置串口参数的纯虚函数
    virtual void configure() = 0;

    // 获取处理器名称
    virtual QString getProcessorName() const = 0;
};

#endif // SERIALDATAPROCESSOR_H
