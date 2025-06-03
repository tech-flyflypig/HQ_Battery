#include "serialworker.h"
#include "batteryinterfacefactory.h"
#include <QDebug>
#include <QElapsedTimer>
#include <QEventLoop>

SerialWorker::SerialWorker(QObject *parent)
    : QThread(parent)        // QThread可以传递parent
    , m_dataBuffer()
    , serialPort(nullptr)
    , m_running(false)
    , m_batteryInterface(nullptr)  // 默认1秒查询一次
    , m_queryInterval(1000)  // 默认5秒超时
    , m_communicationTimeout(5000)
    , m_lastDataTime(0)
    , m_isCommunicationTimeout(false)
{
    // 不再需要创建和连接QTimer
}

// 实现CommunicationWorker接口
void SerialWorker::startCommunication(const QString &address, const QString &productType)
{
    // 对于串口通信，address就是端口名称
    startReading(address, productType);
}

void SerialWorker::stopCommunication()
{
    stopReading();
}

void SerialWorker::onBatteryDataProcessed(const BMS_1 &batteryData)
{
    qDebug() << "Received battery data in SerialWorker::onBatteryDataProcessed, thread:" << QThread::currentThreadId();
    qDebug() << "Battery data - SOC:" << batteryData.soc
             << ", Voltage:" << batteryData.voltage
             << ", Current:" << batteryData.current;
    emit forwardBatteryData(batteryData);
}

void SerialWorker::setupProcessorConnections()
{
    if (!m_batteryInterface)
    {
        qDebug() << "Error: m_batteryInterface is null in setupProcessorConnections";
        return;
    }

    qDebug() << "Setting up processor connections in thread:" << QThread::currentThreadId();
    qDebug() << "m_batteryInterface at:" << m_batteryInterface;
    // qDebug() << "this object at:" << this;

    // 断开所有之前的连接 - 使用函数指针方式
    QObject::disconnect(m_batteryInterface, &BatteryInterface::batteryDataProcessed,
                        this, &SerialWorker::onBatteryDataProcessed);

    // 连接到中间槽函数 - 使用函数指针方式，避免类型转换
    bool connected = QObject::connect(m_batteryInterface, &BatteryInterface::batteryDataProcessed,
                                      this, &SerialWorker::onBatteryDataProcessed,
                                      Qt::QueuedConnection);

    qDebug() << "Signal connection established:" << connected;

    if (!connected)
    {
        qDebug() << "Failed to connect BatteryInterface signal to SerialWorker slot";
    }
}


SerialWorker::~SerialWorker()
{
    // 停止线程运行
    stopReading();

    // 等待线程结束，但设置超时防止永久阻塞
    if (!wait(3000))   // 等待最多3秒
    {
        qDebug() << "SerialWorker线程无法正常结束，强制终止";
        terminate(); // 强制终止线程（不安全，但作为最后手段）
        wait(); // 确保线程真正结束
    }

    // 安全删除串口对象
    QMutexLocker locker(&m_mutex); // 防止多线程访问冲突

    if (serialPort)
    {
        if (serialPort->isOpen())
        {
            serialPort->clear(); // 清空缓冲区
            serialPort->close();
        }
        delete serialPort;
        serialPort = nullptr;
    }

    // 释放电池接口对象
    if (m_batteryInterface)
    {
        delete m_batteryInterface;
        m_batteryInterface = nullptr;
    }
}

void SerialWorker::startReading(const QString &portName, const QString &productType)
{
    QMutexLocker locker(&m_mutex);
    qDebug() << "Starting SerialWorker in thread:" << QThread::currentThreadId();
    m_portName = portName;

    // 使用工厂创建对应的电池接口
    if (m_batteryInterface)
    {
        delete m_batteryInterface;
        m_batteryInterface = nullptr;
    }
    m_batteryInterface = BatteryInterfaceFactory::createBattery(productType);

    if (!m_batteryInterface)
    {
        emit error(tr("不支持的产品类型: %1").arg(productType));
        return;
    }

    // 配置处理器并设置连接
    m_batteryInterface->configure();
    setupProcessorConnections();

    if (!isRunning())
    {
        m_running = true;
        start(QThread::HighPriority); // 使用高优先级启动线程
    }
}

void SerialWorker::stopReading()
{
    QMutexLocker locker(&m_mutex);
    m_running = false;

    // 如果串口打开，尝试唤醒等待中的waitForReadyRead
    if (serialPort && serialPort->isOpen())
    {
        // 发送空数据包激活waitForReadyRead返回
        serialPort->write(QByteArray());
    }
}

void SerialWorker::run()
{
    qDebug() << "Worker thread started, thread ID:" << QThread::currentThreadId();

    // 在工作线程中重新建立连接
    setupProcessorConnections();

    // 确保serialPort为空时创建新实例
    if (!serialPort)
    {
        serialPort = new QSerialPort();
    }

    QElapsedTimer queryTimer;  // 局部定时器，用于记录时间
    queryTimer.start();        // 开始计时

    QElapsedTimer communicationTimer; // 用于检测通信超时
    communicationTimer.start();       // 开始计时
    m_lastDataTime = communicationTimer.elapsed(); // 初始化最后数据时间
    QEventLoop eventLoop;
    while (m_running)
    {
        if (!serialPort->isOpen())
        {
            serialPort->setPortName(m_portName);
            if (!serialPort->open(QIODevice::ReadWrite))
            {
                emit error(tr("Failed to open serial port: %1").arg(serialPort->errorString()));

                // 休眠一段时间后再尝试重新打开，避免频繁尝试
                msleep(1000);
                continue; // 继续下一次循环尝试，而不是退出线程
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
        if (queryTimer.elapsed() >= m_queryInterval)
        {
            sendQuery();
            queryTimer.restart();
        }

        // 检查通信是否超时
        qint64 currentTime = communicationTimer.elapsed();
        if (currentTime - m_lastDataTime > m_communicationTimeout)
        {
            if (!m_isCommunicationTimeout)
            {
                m_isCommunicationTimeout = true;
                emit communicationTimeout();
                qDebug() << "Communication timeout: No data received for more than"
                         << m_communicationTimeout / 1000.0 << "seconds";
            }
        }

        // 仅在串口打开时进行读取操作
        if (serialPort->isOpen() )
        {
            serialPort->waitForReadyRead(1000);
            // 确保读取所有可用数据
            QByteArray data = serialPort->readAll();
            if (!data.isEmpty())
            {
                // 更新最后一次接收数据时间
                m_lastDataTime = communicationTimer.elapsed();

                // 如果之前是超时状态，恢复正常
                if (m_isCommunicationTimeout)
                {
                    m_isCommunicationTimeout = false;
                    qDebug() << "Communication restored";
                }

                // 将新数据添加到缓冲区
                m_dataBuffer.append(data);

                // 尝试处理缓冲区中的数据
                processBuffer();

                emit dataReceived(data);
            }
        }
        msleep(10); // 避免CPU占用过高，但保持响应
    }

    // 确保安全关闭串口
    if (serialPort && serialPort->isOpen())
    {
        serialPort->close();
        qDebug() << "关闭串口";
    }
}

void SerialWorker::processData(const QByteArray &data)
{
    if (m_batteryInterface)
    {
        m_batteryInterface->processSerialData(data);
    }
}

void SerialWorker::sendQuery()
{
    if (!m_batteryInterface) return;

    QByteArray cmd = m_batteryInterface->getQueryCommand();
    if (!cmd.isEmpty())
    {
        sendCommand(cmd);
        qDebug() << "发送：" << cmd.toHex();
    }
}

bool SerialWorker::sendCommand(const QByteArray &cmd)
{
    if (!serialPort || !serialPort->isOpen()) return false;

    if (serialPort->write(cmd) != cmd.size())
    {
        emit error(tr("Failed to write data to serial port"));
        return false;
    }

    serialPort->flush();

    // 添加延时，给设备足够的响应时间
    QThread::msleep(100);

    return true;
}

bool SerialWorker::sendControlCommand(uint16_t reg, uint16_t value)
{
    if (!m_batteryInterface || !m_batteryInterface->supportsControl())
        return false;

    if (!m_batteryInterface->isValidControlValue(reg, value))
    {
        emit error(tr("Invalid control command"));
        qDebug() << "Invalid control command";
        return false;
    }

    QByteArray cmd = m_batteryInterface->getControlCommand(reg, value);
    if (cmd.isEmpty())
    {
        emit error(tr("Failed to create control command"));
        qDebug() << "Failed to create control command";
        return false;
    }

    return sendCommand(cmd);
}

void SerialWorker::processBuffer()
{
    if (!m_batteryInterface) return;

    // 从电池接口获取 Modbus 参数
    uint8_t slaveAddr = 0x01;  // 默认从机地址，应该从接口获取
    uint8_t readHoldings = 0x03;  // 默认功能码，应该从接口获取

    // 检查缓冲区中是否有完整的 Modbus 帧
    while (m_dataBuffer.size() >= 5) // 最小的 Modbus 响应长度
    {
        // 检查是否是有效的 Modbus 响应开头
        if (static_cast<uint8_t>(m_dataBuffer[0]) == slaveAddr &&
                static_cast<uint8_t>(m_dataBuffer[1]) == readHoldings)
        {
            // 获取数据长度字节
            uint8_t byteCount = static_cast<uint8_t>(m_dataBuffer[2]);

            // 计算完整帧的预期长度 (从机地址 + 功能码 + 字节数 + 数据 + CRC)
            //???发送过来的字节数不对 获取122寄存器，应该数据位的字节数是244
            int frameLength = 3 + byteCount + 2;

            // 如果缓冲区中有完整的帧
            if (m_dataBuffer.size() >= byteCount)
            {
                // 提取完整的帧
                QByteArray frame = m_dataBuffer.left(frameLength);

                // 处理这个完整的帧
                processData(frame);

                // 从缓冲区中移除已处理的数据
                m_dataBuffer.remove(0, frameLength);
            }
            else
            {
                // 缓冲区中没有完整的帧，等待更多数据
                break;
            }
        }
        else
        {
            // 不是有效的帧开头，移除第一个字节并继续查找
            m_dataBuffer.remove(0, 1);
        }
    }
}
