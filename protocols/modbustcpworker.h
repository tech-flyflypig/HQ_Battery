#ifndef MODBUSTCPWORKER_H
#define MODBUSTCPWORKER_H

#include <QObject>
#include <QThread>
#include <QTcpSocket>
#include <QMutex>
#include <QTimer>
#include "../utils/Struct.h"
#include "batteryinterface.h"
#include "communicationworker.h"

// 使用虚拟继承来解决菱形继承问题
class ModbusTcpWorker : public  QThread, public  CommunicationWorker
{
    Q_OBJECT

public:
    explicit ModbusTcpWorker(QObject *parent = nullptr);
    ~ModbusTcpWorker() override;

    // 开始连接并读取数据
    void startReading(const QString &ipAddress, int port, const QString &productType);
    // 停止读取
    void stopReading();
    // 发送控制命令
    bool sendControlCommand(uint16_t reg, uint16_t value) override;

    // 实现CommunicationWorker接口
    void startCommunication(const QString &address, const QString &productType) override;
    void stopCommunication() override;
    bool isRunning() const override { return m_running; }
    
    // 实现asQObject方法，返回this作为QObject指针
    QObject* asQObject() override { return this; }

    // 设置端口号
    void setPort(int port) { m_port = port > 0 ? port : 502; }
    // 获取当前端口号
    int getPort() const { return m_port; }

signals:
    // 数据接收信号
    void dataReceived(const QByteArray &data);
    // 错误信号
    void error(const QString &message);
    // 通信超时信号
    void communicationTimeout();
    // 电池数据处理完成信号
    void forwardBatteryData(const BMS_1 &batteryData);

protected:
    // 线程执行函数
    void run() override;

private slots:
    // 处理电池数据
    void onBatteryDataProcessed(const BMS_1 &batteryData);

private:
    // 处理接收到的数据
    void processData(const QByteArray &data);
    // 发送查询命令
    void sendQuery();
    // 发送命令
    bool sendCommand(const QByteArray &cmd);
    // 处理缓冲区数据
    void processBuffer();
    // 设置处理器连接
    void setupProcessorConnections();
    // 将Modbus RTU命令转换为Modbus TCP命令
    QByteArray createModbusTcpFromRtu(const QByteArray &rtuCmd);

    QTcpSocket *tcpSocket;
    QMutex m_mutex;
    bool m_running;
    BatteryInterface *m_batteryInterface;
    int m_queryInterval;
    int m_communicationTimeout;
    qint64 m_lastDataTime;
    bool m_isCommunicationTimeout;
    QByteArray m_dataBuffer;
    QString m_ipAddress;
    int m_port;
};

#endif // MODBUSTCPWORKER_H 