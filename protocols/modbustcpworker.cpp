#include "modbustcpworker.h"
#include "batteryinterfacefactory.h"
#include <QDebug>
#include <QElapsedTimer>
#include <QEventLoop>

ModbusTcpWorker::ModbusTcpWorker(QObject *parent)
    : QThread(parent)        // QThread可以传递parent
    , tcpSocket(nullptr)
    , m_running(false)
    , m_batteryInterface(nullptr)
    , m_queryInterval(1000)  // 默认1秒查询一次
    , m_communicationTimeout(5000)  // 默认5秒超时
    , m_lastDataTime(0)
    , m_isCommunicationTimeout(false)
    , m_dataBuffer()
    , m_port(502)  // 默认Modbus TCP端口
{

}

// 实现CommunicationWorker接口
void ModbusTcpWorker::startCommunication(const QString &address, const QString &productType)
{
    // 对于TCP通信，address是IP地址，端口使用默认值或通过setPort设置
    startReading(address, m_port, productType);
}

void ModbusTcpWorker::stopCommunication()
{
    stopReading();
}

void ModbusTcpWorker::onBatteryDataProcessed(const BMS_1 &batteryData)
{
    qDebug() << "Received battery data in ModbusTcpWorker::onBatteryDataProcessed, thread:" << QThread::currentThreadId();
    qDebug() << "Battery data - SOC:" << batteryData.soc
             << ", Voltage:" << batteryData.voltage
             << ", Current:" << batteryData.current;
    emit forwardBatteryData(batteryData);
}

void ModbusTcpWorker::setupProcessorConnections()
{
    if (!m_batteryInterface)
    {
        qDebug() << "Error: m_batteryInterface is null in setupProcessorConnections";
        return;
    }

    qDebug() << "Setting up processor connections in thread:" << QThread::currentThreadId();

    // 断开所有之前的连接 - 使用函数指针方式
    QObject::disconnect(m_batteryInterface, &BatteryInterface::batteryDataProcessed,
                        this, &ModbusTcpWorker::onBatteryDataProcessed);

    // 连接到中间槽函数 - 使用函数指针方式，避免类型转换
    bool connected = QObject::connect(m_batteryInterface, &BatteryInterface::batteryDataProcessed,
                                      this, &ModbusTcpWorker::onBatteryDataProcessed,
                                      Qt::QueuedConnection);

    qDebug() << "Signal connection established:" << connected;

    if (!connected)
    {
        qDebug() << "Failed to connect BatteryInterface signal to ModbusTcpWorker slot";
    }
}

ModbusTcpWorker::~ModbusTcpWorker()
{
    // 停止线程运行
    stopReading();

    // 等待线程结束，但设置超时防止永久阻塞
    if (!wait(3000))   // 等待最多3秒
    {
        qDebug() << "ModbusTcpWorker线程无法正常结束，强制终止";
        terminate(); // 强制终止线程（不安全，但作为最后手段）
        wait(); // 确保线程真正结束
    }

    // 安全删除TCP Socket对象
    QMutexLocker locker(&m_mutex); // 防止多线程访问冲突

    if (tcpSocket)
    {
        if (tcpSocket->state() == QAbstractSocket::ConnectedState)
        {
            tcpSocket->disconnectFromHost();
            if (tcpSocket->state() != QAbstractSocket::UnconnectedState)
                tcpSocket->waitForDisconnected();
        }
        delete tcpSocket;
        tcpSocket = nullptr;
    }

    // 释放电池接口对象
    if (m_batteryInterface)
    {
        delete m_batteryInterface;
        m_batteryInterface = nullptr;
    }
}

void ModbusTcpWorker::startReading(const QString &ipAddress, int port, const QString &productType)
{
    QMutexLocker locker(&m_mutex);
    qDebug() << "Starting ModbusTcpWorker in thread:" << QThread::currentThreadId();
    m_ipAddress = ipAddress;
    m_port = port > 0 ? port : 502; // 如果端口无效，使用默认端口

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

void ModbusTcpWorker::stopReading()
{
    QMutexLocker locker(&m_mutex);
    m_running = false;

    // 如果TCP连接打开，尝试唤醒等待中的waitForReadyRead
    if (tcpSocket && tcpSocket->state() == QAbstractSocket::ConnectedState)
    {
        // 发送空数据包激活waitForReadyRead返回
        tcpSocket->write(QByteArray());
    }
}

void ModbusTcpWorker::run()
{
    qDebug() << "Worker thread started, thread ID:" << QThread::currentThreadId();

    // 在工作线程中重新建立连接
    setupProcessorConnections();

    // 确保tcpSocket为空时创建新实例
    if (!tcpSocket)
    {
        tcpSocket = new QTcpSocket();
    }

    QElapsedTimer queryTimer;  // 局部定时器，用于记录时间
    queryTimer.start();        // 开始计时

    QElapsedTimer communicationTimer; // 用于检测通信超时
    communicationTimer.start();       // 开始计时
    m_lastDataTime = communicationTimer.elapsed(); // 初始化最后数据时间

    while (m_running)
    {
        if (tcpSocket->state() != QAbstractSocket::ConnectedState)
        {
            // 尝试连接到服务器
            tcpSocket->connectToHost(m_ipAddress, m_port);
            if (!tcpSocket->waitForConnected(3000))
            {
                emit error(tr("Failed to connect to host: %1").arg(tcpSocket->errorString()));

                // 休眠一段时间后再尝试重新连接，避免频繁尝试
                msleep(1000);
                continue; // 继续下一次循环尝试，而不是退出线程
            }

            qDebug() << "TCP connection established:" << m_ipAddress << ":" << m_port;

            // 重置定时器
            queryTimer.restart();
            communicationTimer.restart();
            m_lastDataTime = communicationTimer.elapsed();
            m_isCommunicationTimeout = false;
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

        // 读取可用数据
        if (tcpSocket->state() == QAbstractSocket::ConnectedState)
        {
            tcpSocket->waitForReadyRead(1000);
            QByteArray data = tcpSocket->readAll();
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

    // 确保安全关闭TCP连接
    if (tcpSocket && tcpSocket->state() == QAbstractSocket::ConnectedState)
    {
        tcpSocket->disconnectFromHost();
        if (tcpSocket->state() != QAbstractSocket::UnconnectedState)
            tcpSocket->waitForDisconnected();
        qDebug() << "关闭TCP连接";
    }
}

void ModbusTcpWorker::processData(const QByteArray &data)
{
    if (m_batteryInterface)
    {
        // 对于Modbus TCP，我们需要去掉MBAP头部（前7个字节）
        if (data.size() > 7)
        {
            QByteArray rtuData = data.mid(7);
            m_batteryInterface->processSerialData(rtuData);
        }
    }
}

void ModbusTcpWorker::sendQuery()
{
    if (!m_batteryInterface) return;

    QByteArray rtuCmd = m_batteryInterface->getQueryCommand();
    if (!rtuCmd.isEmpty())
    {
        // 将Modbus RTU命令转换为Modbus TCP命令
        QByteArray tcpCmd = createModbusTcpFromRtu(rtuCmd);
        sendCommand(tcpCmd);
        qDebug() << "发送TCP查询：" << tcpCmd.toHex();
    }
}

QByteArray ModbusTcpWorker::createModbusTcpFromRtu(const QByteArray &rtuCmd)
{
    // 移除RTU命令的CRC校验（最后两个字节）
    QByteArray cmd = rtuCmd.left(rtuCmd.size() - 2);

    // 创建MBAP头部
    QByteArray mbapHeader;
    // 事务标识符（2字节）
    mbapHeader.append((char)0x00);
    mbapHeader.append((char)0x01);
    // 协议标识符（2字节，0表示Modbus）
    mbapHeader.append((char)0x00);
    mbapHeader.append((char)0x00);
    // 长度（2字节，后续字节数）
    quint16 length = cmd.size();
    mbapHeader.append((char)((length >> 8) & 0xFF));
    mbapHeader.append((char)(length & 0xFF));

    // 组合MBAP头部和RTU命令（不含CRC）
    return mbapHeader + cmd;
}

bool ModbusTcpWorker::sendCommand(const QByteArray &cmd)
{
    if (!tcpSocket || tcpSocket->state() != QAbstractSocket::ConnectedState) return false;

    if (tcpSocket->write(cmd) != cmd.size())
    {
        emit error(tr("Failed to write data to TCP socket"));
        return false;
    }

    tcpSocket->flush();

    // 添加延时，给设备足够的响应时间
    QThread::msleep(100);

    return true;
}

bool ModbusTcpWorker::sendControlCommand(uint16_t reg, uint16_t value)
{
    if (!m_batteryInterface || !m_batteryInterface->supportsControl()) return false;

    if (!m_batteryInterface->isValidControlValue(reg, value))
    {
        emit error(tr("Invalid control command"));
        return false;
    }

    QByteArray rtuCmd = m_batteryInterface->getControlCommand(reg, value);
    if (rtuCmd.isEmpty())
    {
        emit error(tr("Failed to create control command"));
        return false;
    }

    // 将RTU命令转换为TCP命令
    QByteArray tcpCmd = createModbusTcpFromRtu(rtuCmd);
    return sendCommand(tcpCmd);
}

void ModbusTcpWorker::processBuffer()
{
    // Modbus TCP消息处理
    while (m_dataBuffer.size() >= 8) // MBAP头(7字节) + 功能码(1字节)
    {
        // 从MBAP头获取消息长度
        quint16 length = ((quint8)m_dataBuffer[4] << 8) | (quint8)m_dataBuffer[5];

        // 完整帧的长度 = MBAP头(6字节) + 单元标识符(1字节) + PDU长度
        int frameLength = 6 + 1 + length;

        // 检查缓冲区是否包含完整的帧
        if (m_dataBuffer.size() >= frameLength)
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
}
