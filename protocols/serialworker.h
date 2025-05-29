#ifndef SERIALWORKER_H
#define SERIALWORKER_H

#include <QThread>
#include <QSerialPort>
#include <QMutex>
#include <QTimer>
#include "batteryinterface.h"
#include "communicationworker.h"

// 使用虚拟继承来解决菱形继承问题
class SerialWorker : public  QThread, public  CommunicationWorker
{
    Q_OBJECT

public:
    explicit SerialWorker(QObject *parent = nullptr);
    ~SerialWorker() override;

    void startReading(const QString &portName, const QString &productType);
    void stopReading();

    // 实现CommunicationWorker接口
    void startCommunication(const QString &address, const QString &productType) override;
    void stopCommunication() override;
    bool sendControlCommand(uint16_t reg, uint16_t value) override;
    bool isRunning() const override { return m_running; }
    
    // 实现asQObject方法，返回this作为QObject指针
    QObject* asQObject() override { return this; }

    // 设置通信超时时间（毫秒）
    void setCommunicationTimeout(int timeout)
    {
        m_communicationTimeout = timeout;
    }

signals:
    void error(const QString &errorMessage);
    void dataReceived(const QByteArray &data);
    void forwardBatteryData(const BMS_1 &batteryData);
    void communicationTimeout(); // 通信超时信号

protected:
    void run() override;

private slots:
    void onBatteryDataProcessed(const BMS_1 &batteryData);

private:
    void processData(const QByteArray &data);
    void setupProcessorConnections();
    void sendQuery();          // 发送查询命令
    bool sendCommand(const QByteArray &cmd);  // 通用发送命令函数
    void processBuffer();          // 处理缓冲区中的数据
    QByteArray m_dataBuffer;  // 添加数据缓冲区成员变量
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
