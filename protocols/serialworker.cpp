#include "serialworker.h"
#include "productprocessors.h"
#include <QDebug>

SerialWorker::SerialWorker(QObject *parent)
    : QThread(parent)
    , serialPort(nullptr)
    , m_running(false)
    , m_processor(nullptr)
{
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
    
    delete m_processor;
}

void SerialWorker::startReading(const QString &portName, const QString &productType)
{
    QMutexLocker locker(&m_mutex);
    m_portName = portName;
    
    // 创建对应的数据处理器
    if (m_processor) {
        delete m_processor;
        m_processor = nullptr;
    }
    m_processor = SerialDataProcessorFactory::createProcessor(productType);
    
    if (!m_processor) {
        emit error(tr("不支持的产品类型: %1").arg(productType));
        return;
    }
    
    // 配置处理器
    m_processor->configure();
    
    if (!isRunning()) {
        m_running = true;
        start(QThread::HighPriority); // 使用高优先级启动线程
    }
}

void SerialWorker::stopReading()
{
    QMutexLocker locker(&m_mutex);
    m_running = false;
}

void SerialWorker::run()
{
    // 在新线程中创建串口对象
    serialPort = new QSerialPort();
    
    while (m_running) {
        if (!serialPort->isOpen()) {
            serialPort->setPortName(m_portName);
            if (!serialPort->open(QIODevice::ReadOnly)) {
                emit error(tr("Failed to open serial port: %1").arg(serialPort->errorString()));
                break;
            }
            qDebug() << "Serial port opened:" << m_portName;
        }
        
        if (serialPort->waitForReadyRead(100)) { // 等待数据到达，超时100ms
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
