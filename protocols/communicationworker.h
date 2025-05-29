#ifndef COMMUNICATIONWORKER_H
#define COMMUNICATIONWORKER_H

#include <QObject>
#include "../utils/Struct.h"

// 通用通信接口，作为SerialWorker和ModbusTcpWorker的基类
class CommunicationWorker
{
public:
    virtual ~CommunicationWorker() {}
    virtual void startCommunication(const QString &address, const QString &productType) = 0;
    virtual void stopCommunication() = 0;
    virtual bool sendControlCommand(uint16_t reg, uint16_t value) = 0;
    virtual bool isRunning() const = 0;
    
    // 获取QObject指针，用于信号槽连接
    virtual QObject* asQObject() = 0;
};
#endif // COMMUNICATIONWORKER_H 