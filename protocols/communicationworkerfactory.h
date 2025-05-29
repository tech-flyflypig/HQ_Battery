#ifndef COMMUNICATIONWORKERFACTORY_H
#define COMMUNICATIONWORKERFACTORY_H

#include <QObject>
#include <QString>
#include "communicationworker.h"
#include "serialworker.h"
#include "modbustcpworker.h"

// 通信类型枚举
enum class CommunicationType {
    Serial,  // 串口通信
    TCP      // TCP通信
};

// 通信工作器工厂类
class CommunicationWorkerFactory
{
public:
    // 创建通信工作器
    static CommunicationWorker* createWorker(CommunicationType type, QObject *parent = nullptr)
    {
        switch (type) {
            case CommunicationType::Serial:
                return new SerialWorker(parent);
            case CommunicationType::TCP:
                return new ModbusTcpWorker(parent);
            default:
                return nullptr;
        }
    }
    
    // 获取通信类型的字符串表示
    static QString getTypeName(CommunicationType type)
    {
        switch (type) {
            case CommunicationType::Serial:
                return "Serial";
            case CommunicationType::TCP:
                return "TCP";
            default:
                return "Unknown";
        }
    }
    
    // 从字符串获取通信类型
    static CommunicationType getTypeFromName(const QString &name)
    {
        if (name.toLower() == "serial") {
            return CommunicationType::Serial;
        } else if (name.toLower() == "tcp") {
            return CommunicationType::TCP;
        } else {
            return CommunicationType::Serial; // 默认使用串口
        }
    }
};

#endif // COMMUNICATIONWORKERFACTORY_H 