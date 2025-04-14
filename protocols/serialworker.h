#ifndef SERIALWORKER_H
#define SERIALWORKER_H

#include <QThread>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QMutex>
#include "serialdataprocessor.h"

class SerialWorker : public QThread
{
    Q_OBJECT

public:
    explicit SerialWorker(QObject *parent = nullptr);
    ~SerialWorker() override;
    
    void startReading(const QString &portName, const QString &productType);
    void stopReading();
    
signals:
    void dataReceived(const QByteArray &data);
    void error(const QString &message);

protected:
    void run() override;

private:
    QSerialPort *serialPort;
    QString m_portName;
    volatile bool m_running;
    QMutex m_mutex;
    SerialDataProcessor *m_processor;
    void processData(const QByteArray &data);
};

#endif // SERIALWORKER_H
