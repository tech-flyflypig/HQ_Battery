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

signals:
    void error(const QString &errorMessage);
    void dataReceived(const QByteArray &data);
    void forwardBatteryData(const BMS_1 &batteryData);

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
    QTimer* m_queryTimer;          // 定时查询定时器
};

#endif // SERIALWORKER_H
