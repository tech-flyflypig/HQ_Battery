#ifndef SERIALWORKER_H
#define SERIALWORKER_H

#include <QThread>
#include <QSerialPort>
#include <QMutex>
#include <QTimer>
#include "batteryinterface.h"

class SerialWorker : public QThread
{
    Q_OBJECT

public:
    explicit SerialWorker(QObject *parent = nullptr);
    ~SerialWorker();

    void startReading(const QString &portName, const QString &productType);
    void stopReading();
    
    // 新增：发送控制命令的接口
    bool sendControlCommand(uint16_t reg, uint16_t value);
    
    // 设置通信超时时间（毫秒）
    void setCommunicationTimeout(int timeout) { m_communicationTimeout = timeout; }

signals:
    void error(const QString &errorMessage);
    void dataReceived(const QByteArray &data);
    void forwardBatteryData(const BMS_1 &batteryData);
    void communicationTimeout(); // 通信超时信号

protected:
    void run() override;

private:
    void processData(const QByteArray &data);
    void setupProcessorConnections();
    void sendQuery();          // 发送查询命令
    bool sendCommand(const QByteArray& cmd);  // 通用发送命令函数

    QSerialPort *serialPort;
    QString m_portName;
    bool m_running;
    QMutex m_mutex;
    BatteryInterface *m_batteryInterface;  // 电池接口处理器
    int m_queryInterval;  // 查询间隔（毫秒）
    
    // 通信超时相关
    int m_communicationTimeout; // 通信超时时间（毫秒），默认5000ms
    qint64 m_lastDataTime;      // 上次接收数据的时间
    bool m_isCommunicationTimeout; // 当前是否处于通信超时状态
};

#endif // SERIALWORKER_H
