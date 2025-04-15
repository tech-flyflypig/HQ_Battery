#include "serialworker.h"
#include "batteryinterfacefactory.h"
#include <QDebug>

SerialWorker::SerialWorker(QObject *parent)
    : QThread(parent)
    , serialPort(nullptr)
    , m_running(false)
    , m_processor(nullptr)
    , m_queryTimer(nullptr)
{
    m_queryTimer = new QTimer(this);
    connect(m_queryTimer, &QTimer::timeout, this, &SerialWorker::sendQuery);
}

void SerialWorker::setupProcessorConnections()
{
    if (!m_processor) return;

    // 连接电池数据信号
    connect(m_processor, &BatteryInterface::batteryDataProcessed,
            this, &SerialWorker::forwardBatteryData);
}

SerialWorker::~SerialWorker()
{
    stopReading();
    wait(); // 等待线程结束
    
    if (serialPort) {
        if (serialPort->isOpen()) {
            serialPort->close();
        }
        delete serialPort;
    }
    
    if (m_queryTimer) {
        m_queryTimer->stop();
        delete m_queryTimer;
    }
    
    delete m_processor;
}

void SerialWorker::startReading(const QString &portName, const QString &productType)
{
    QMutexLocker locker(&m_mutex);
    m_portName = portName;
    
    // 使用工厂创建对应的电池接口
    if (m_processor) {
        delete m_processor;
        m_processor = nullptr;
    }
    m_processor = BatteryInterfaceFactory::createBattery(productType);
    
    if (!m_processor) {
        emit error(tr("不支持的产品类型: %1").arg(productType));
        return;
    }
    
    // 配置处理器并设置连接
    m_processor->configure();
    setupProcessorConnections();
    
    if (!isRunning()) {
        m_running = true;
        start(QThread::HighPriority); // 使用高优先级启动线程
    }
}

void SerialWorker::stopReading()
{
    QMutexLocker locker(&m_mutex);
    m_running = false;
    if (m_queryTimer) {
        m_queryTimer->stop();
    }
}

void SerialWorker::run()
{
    serialPort = new QSerialPort();
    
    while (m_running) {
        if (!serialPort->isOpen()) {
            serialPort->setPortName(m_portName);
            if (!serialPort->open(QIODevice::ReadWrite)) {
                emit error(tr("Failed to open serial port: %1").arg(serialPort->errorString()));
                break;
            }
            
            // 配置串口参数
            serialPort->setBaudRate(QSerialPort::Baud9600);
            serialPort->setDataBits(QSerialPort::Data8);
            serialPort->setStopBits(QSerialPort::OneStop);
            serialPort->setParity(QSerialPort::NoParity);
            
            qDebug() << "Serial port opened:" << m_portName;
            
            // 启动定时查询
            m_queryTimer->start(1000); // 每秒查询一次
        }
        
        if (serialPort->waitForReadyRead(100)) {
            QByteArray data = serialPort->readAll();
            if (!data.isEmpty()) {
                processData(data);
                emit dataReceived(data);
            }
        }
        
        msleep(1); // 避免CPU占用过高
    }
    
    if (serialPort->isOpen()) {
        serialPort->close();
    }
}

void SerialWorker::processData(const QByteArray &data)
{
    if (m_processor) {
        m_processor->processSerialData(data);
    }
}

void SerialWorker::sendQuery()
{
    if (!m_processor) return;
    
    QByteArray cmd = m_processor->getQueryCommand();
    if (!cmd.isEmpty()) {
        sendCommand(cmd);
    }
}

bool SerialWorker::sendCommand(const QByteArray& cmd)
{
    if (!serialPort || !serialPort->isOpen()) return false;
    
    if (serialPort->write(cmd) != cmd.size()) {
        emit error(tr("Failed to write data to serial port"));
        return false;
    }
    
    serialPort->flush();
    return true;
}

bool SerialWorker::sendControlCommand(uint16_t reg, uint16_t value)
{
    if (!m_processor || !m_processor->supportsControl()) return false;
    
    if (!m_processor->isValidControlValue(reg, value)) {
        emit error(tr("Invalid control command"));
        return false;
    }
    
    QByteArray cmd = m_processor->getControlCommand(reg, value);
    if (cmd.isEmpty()) {
        emit error(tr("Failed to create control command"));
        return false;
    }
    
    return sendCommand(cmd);
}
