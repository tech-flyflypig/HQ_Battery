#include "serialworker.h"
#include "batteryinterfacefactory.h"
#include <QDebug>
#include <QElapsedTimer>

SerialWorker::SerialWorker(QObject *parent)
    : QThread(parent)
    , serialPort(nullptr)
    , m_running(false)
    , m_batteryInterface(nullptr)
    , m_queryInterval(1000)  // 默认1秒查询一次
    , m_communicationTimeout(5000)  // 默认5秒超时
    , m_lastDataTime(0)
    , m_isCommunicationTimeout(false)
{
    // 不再需要创建和连接QTimer
}

void SerialWorker::setupProcessorConnections()
{
    if (!m_batteryInterface) return;

    // 连接电池数据信号
    connect(m_batteryInterface, &BatteryInterface::batteryDataProcessed,
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
    
    delete m_batteryInterface;
}

void SerialWorker::startReading(const QString &portName, const QString &productType)
{
    QMutexLocker locker(&m_mutex);
    m_portName = portName;
    
    // 使用工厂创建对应的电池接口
    if (m_batteryInterface) {
        delete m_batteryInterface;
        m_batteryInterface = nullptr;
    }
    m_batteryInterface = BatteryInterfaceFactory::createBattery(productType);
    
    if (!m_batteryInterface) {
        emit error(tr("不支持的产品类型: %1").arg(productType));
        return;
    }
    
    // 配置处理器并设置连接
    m_batteryInterface->configure();
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
}

void SerialWorker::run()
{
    serialPort = new QSerialPort();
    QElapsedTimer queryTimer;  // 局部定时器，用于记录时间
    queryTimer.start();        // 开始计时
    
    QElapsedTimer communicationTimer; // 用于检测通信超时
    communicationTimer.start();       // 开始计时
    m_lastDataTime = communicationTimer.elapsed(); // 初始化最后数据时间
    
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
            
            // 不再使用QTimer启动定时查询
            queryTimer.restart();  // 重置定时器
            communicationTimer.restart(); // 重置通信计时器
            m_lastDataTime = communicationTimer.elapsed(); // 重置最后数据时间
            m_isCommunicationTimeout = false; // 重置超时状态
        }
        
        // 检查是否需要发送查询命令
        if (queryTimer.elapsed() >= m_queryInterval) {
            sendQuery();
            queryTimer.restart();
        }
        
        // 检查通信是否超时
        qint64 currentTime = communicationTimer.elapsed();
        if (currentTime - m_lastDataTime > m_communicationTimeout) {
            if (!m_isCommunicationTimeout) {
                m_isCommunicationTimeout = true;
                emit communicationTimeout();
                qDebug() << "Communication timeout: No data received for more than" 
                         << m_communicationTimeout/1000.0 << "seconds";
            }
        }
        
        if (serialPort->waitForReadyRead(100)) {
            QByteArray data = serialPort->readAll();
            if (!data.isEmpty()) {
                // 更新最后一次接收数据时间
                m_lastDataTime = communicationTimer.elapsed();
                
                // 如果之前是超时状态，恢复正常
                if (m_isCommunicationTimeout) {
                    m_isCommunicationTimeout = false;
                    qDebug() << "Communication restored";
                }
                
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
    if (m_batteryInterface) {
        m_batteryInterface->processSerialData(data);
    }
}

void SerialWorker::sendQuery()
{
    if (!m_batteryInterface) return;
    
    QByteArray cmd = m_batteryInterface->getQueryCommand();
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
    if (!m_batteryInterface || !m_batteryInterface->supportsControl()) return false;
    
    if (!m_batteryInterface->isValidControlValue(reg, value)) {
        emit error(tr("Invalid control command"));
        return false;
    }
    
    QByteArray cmd = m_batteryInterface->getControlCommand(reg, value);
    if (cmd.isEmpty()) {
        emit error(tr("Failed to create control command"));
        return false;
    }
    
    return sendCommand(cmd);
}
