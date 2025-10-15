#include "batterylistform.h"
#include "ui_batterylistform.h"

#include <QPainter>
#include <QStyleOption>
#include <QMouseEvent>
#include <QDebug>
#include <cstring>
#include "../utils/BatteryStats.h"
#include "../utils/RecordManager.h"
#include <QElapsedTimer>
#include <QCoreApplication>
#include <QSqlQuery>
#include <QDateTime>
#include <QSqlError>

BatteryListForm::BatteryListForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::BatteryListForm)
    , m_selected(false)
    , m_communicationWorker(nullptr)
    , m_isRunning(false)
    , m_communicationType(CommunicationType::Serial) // 默认使用串口通信
    , m_chargeState(Unknown) // 初始状态为未知
    , m_monitoringStatus(Stopped) // 初始监控状态为停止
    , m_lastSystemStatus(0)
    , m_lastAlarmStatus(0)    // 初始报警状态
    , m_lastProtectStatus(0)  // 初始保护状态
    , m_lastFaultStatus(0)    // 初始故障状态
    , m_lastChargeStateChangeTime(QDateTime::currentDateTime()) // 初始充放电状态变化时间
{
    ui->setupUi(this);

    // 初始化m_lastData
    // memset(&m_lastData, 0, sizeof(BMS_1));
    m_lastData = BMS_1();
    
    // 设置初始样式
    updateStyle();
    
    // 确保电池状态图标初始化为停止状态
    updateStatusIcon();
}

BatteryListForm::~BatteryListForm()
{
    stopCommunication();

    // 确保m_communicationWorker被正确删除
    if (m_communicationWorker)
    {
        // 断开所有信号连接
        QObject *worker = m_communicationWorker->asQObject();
        disconnect(worker, nullptr, this, nullptr);
        delete m_communicationWorker;
        m_communicationWorker = nullptr;
    }

    delete ui;
}

// 设置选中状态
void BatteryListForm::setSelected(bool selected)
{
    if (m_selected != selected)
    {
        m_selected = selected;
        
        // 只更新背景样式，不更新状态图标
        if (m_selected)
        {
            // 选中状态样式 - 蓝色边框和浅蓝色背景
            setStyleSheet("BatteryListForm { border-image: url(:/image/单个电池模块背景_选中.png);} QLabel{ background-color: transparent;}");
        }
        else
        {
            // 未选中状态样式 - 使用原始背景图
            setStyleSheet("BatteryListForm { border-image: url(:/image/单个电池模块背景.png);}QLabel{ background-color: transparent;}");
        }
        
        // 强制重绘
        update();
        
        emit selectedChanged(m_selected);
    }
}

// 设置电池信息
void BatteryListForm::setBatteryInfo(const battery_info &info)
{
    m_batteryInfo = info;

    // 更新UI显示基本信息
    ui->label_address->setText(m_batteryInfo.site); // 显示电池位置

    // 如果通信已经在运行，需要先停止
    if (m_isRunning)
    {
        stopCommunication();
    }

    // 确定通信类型
    if (m_batteryInfo.port_name.contains(":"))
    {
        // 如果包含冒号，认为是IP地址:端口格式
        m_communicationType = CommunicationType::TCP;
    }
    else
    {
        // 否则认为是串口名称
        m_communicationType = CommunicationType::Serial;
    }

    // 初始化通信
    initCommunication();
    
    // 确保状态图标与当前监控状态一致
    updateStatusIcon();
}

battery_info BatteryListForm::getBatteryInfo() const
{
    return m_batteryInfo;
}

// 获取当前监控状态
BatteryListForm::MonitoringStatus BatteryListForm::getMonitoringStatus() const
{
    return m_monitoringStatus;
}

// 开始通信
void BatteryListForm::startCommunication()
{
    if (!m_communicationWorker || m_isRunning)
    {
        return;
    }
    
    // 重新初始化通信工作器和信号连接
    // 这确保每次开始监控时都有正确的信号连接
    initCommunication();

    // 启动通信
    QString address;
    if (m_communicationType == CommunicationType::TCP)
    {
        // 对于TCP，地址是IP:端口格式
        address = m_batteryInfo.port_name;
    }
    else
    {
        // 对于串口，地址就是串口名称
        address = m_batteryInfo.port_name;
    }

    m_communicationWorker->startCommunication(address, m_batteryInfo.type);
    
    // 先设置状态，再更新图标
    m_isRunning = true;
    m_monitoringStatus = Running;
    
    // 更新状态图标
    updateStatusIcon();

    // 发送信号
    emit monitoringStatusChanged(this, m_monitoringStatus);
}

// 停止通信
void BatteryListForm::stopCommunication()
{
    if (!m_communicationWorker || !m_isRunning)
    {
        return;
    }

    // 创建一个故障状态的BMS_1数据
    BMS_1 errorData = m_lastData;
    errorData.battery_info.status = 0x01; // 设置停止状态标志

    // 更新全局统计信息
    BatteryStats::instance()->updateBatteryStatus(m_batteryInfo.power_id, errorData);

    // 停止通信
    m_communicationWorker->stopCommunication();
    m_isRunning = false;

    // 只有当前不是错误状态时，才更新为停止状态
    if (m_monitoringStatus != Error)
    {
        m_monitoringStatus = Stopped;
        
        // 状态更新后再更新图标
        updateStatusIcon();
        
        emit monitoringStatusChanged(this, m_monitoringStatus);
    }

    // 等待一小段时间确保完成停止
    QElapsedTimer timer;
    timer.start();
    while (timer.elapsed() < 500)   // 最多等待500毫秒
    {
        QCoreApplication::processEvents();
    }
}

// 初始化通信
void BatteryListForm::initCommunication()
{
    // 如果已存在，先删除
    if (m_communicationWorker)
    {
        delete m_communicationWorker;
        m_communicationWorker = nullptr;
    }

    // 使用工厂创建通信工作器
    m_communicationWorker = CommunicationWorkerFactory::createWorker(m_communicationType, this);

    if (m_communicationWorker)
    {
        // 获取QObject指针用于连接信号和槽
        QObject *worker = m_communicationWorker->asQObject();

        // 连接信号和槽 - 使用QObject::connect避免歧义
        QObject::connect(worker, SIGNAL(forwardBatteryData(const BMS_1 &)),
                         this, SLOT(onBatteryDataReceived(const BMS_1 &)));
        QObject::connect(worker, SIGNAL(error(const QString &)),
                         this, SLOT(onCommunicationError(const QString &)));
        QObject::connect(worker, SIGNAL(communicationTimeout()),
                         this, SLOT(onCommunicationTimeout()));
    }
    else
    {
        qDebug() << "Failed to create communication worker for type:"
                 << CommunicationWorkerFactory::getTypeName(m_communicationType);
    }
    
    // 注意：这里不要设置状态图标，状态图标应该只在明确的状态变化时设置
}

// 更新UI显示
void BatteryListForm::updateDisplay(const BMS_1 &data)
{
    // 更新电量显示
    QString socStr = QString::number(data.soc) + "%";

    // 更新温度显示
    QString tempStr = QString::number(data.tempMax / 10.0, 'f', 1) + "℃";

    // 更新容量显示
    QString capacityStr = QString::number(data.remainCapacity / 100.0, 'f', 1) + "AH";

    // 更新UI标签
    ui->label_remainCapacity->setText(capacityStr);
    ui->label_tempMax->setText(tempStr);

    // 更新充放电状态显示
    // 根据 systemStatus 位标志判断：bit0=放电(0x0001), bit1=充电(0x0002)
    const uint16_t kDischargingBit = 0x0001;  // bit0
    const uint16_t kChargingBit = 0x0002;     // bit1

    if (data.systemStatus & kChargingBit) {
        ui->label_chargeState->setText("充电");
    } else if (data.systemStatus & kDischargingBit) {
        ui->label_chargeState->setText("放电");
    } else {
        ui->label_chargeState->setText("空闲");
    }

    //ui->label_battery_status->setStyleSheet("border-image: url(:/image/运行.png);");
    // 根据状态更新图标
    // if (data.alarmStatus > 0)
    // {
    //     // 告警状态
    //     ui->label_battery_status->setStyleSheet("border-image: url(:/image/告警.png);");
    // }
    // else if (data.protectStatus > 0)
    // {
    //     // 保护状态
    //     ui->label_battery_status->setStyleSheet("border-image: url(:/image/保护.png);");
    // }
    // else if (m_isRunning)
    // {
    //     // 正常运行状态
    //     ui->label_battery_status->setStyleSheet("border-image: url(:/image/运行.png);");
    // }
}

// 鼠标点击事件处理
void BatteryListForm::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        // 设置为选中状态
        setSelected(true);
        // 发出点击信号
        emit clicked(this);
    }
    QWidget::mousePressEvent(event);
}

void BatteryListForm::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        // 设置为选中状态
        setSelected(true);
        // 发出双击信号，传递自身指针
        emit doubleclicked(this);
    }
    QWidget::mouseDoubleClickEvent(event);
}

// 处理电池数据接收
void BatteryListForm::onBatteryDataReceived(const BMS_1 &data)
{
    // 保存上一次的系统状态
    uint16_t previousSystemStatus = m_lastData.systemStatus;

    // 保存之前的监控状态，用于检测是否从故障状态恢复
    MonitoringStatus previousMonitoringStatus = m_monitoringStatus;

    // 更新电池数据
    m_lastData = data;
    updateDisplay(data);

    // 根据当前监控状态设置battery_info.status
    if (m_isRunning)
    {
        m_lastData.battery_info.status = 0x00; // 设置为正常运行状态
    }
    else
    {
        m_lastData.battery_info.status = 0x01; // 设置为停止状态
    }

    // 如果之前是故障状态，现在收到正常数据，则恢复为运行状态
    if (previousMonitoringStatus == Error && m_isRunning)
    {
        // 更新监控状态为运行
        m_monitoringStatus = Running;

        // 确保状态标志正确设置为运行状态
        m_lastData.battery_info.status = 0x00;
        
        // 状态更新后再更新图标
        updateStatusIcon();

        // 发送状态变化信号
        emit monitoringStatusChanged(this, m_monitoringStatus);

        qDebug() << "电池从故障状态恢复为运行状态";
    }

    // 更新全局统计信息
    BatteryStats::instance()->updateBatteryStatus(m_batteryInfo.power_id, m_lastData);

    // 检查报警状态并记录异常（仅在状态变化时记录）
    if (data.alarmStatus != 0 && data.alarmStatus != m_lastAlarmStatus)
    {
        QString alarmInfo = QString("报警状态: 0x%1").arg(data.alarmStatus, 0, 16);
        RecordManager::instance()->exception_record(m_batteryInfo.power_id, m_batteryInfo.site, 2, alarmInfo);
        m_lastAlarmStatus = data.alarmStatus;
    }
    else if (data.alarmStatus == 0 && m_lastAlarmStatus != 0)
    {
        // 报警状态从非零变为零，记录恢复正常
        RecordManager::instance()->exception_record(m_batteryInfo.power_id, m_batteryInfo.site, 2, "报警状态恢复正常");
        m_lastAlarmStatus = 0;
    }
    
    // 检查保护状态并记录异常（仅在状态变化时记录）
    if (data.protectStatus != 0 && data.protectStatus != m_lastProtectStatus)
    {
        QString protectInfo = QString("保护状态: 0x%1").arg(data.protectStatus, 0, 16);
        RecordManager::instance()->exception_record(m_batteryInfo.power_id, m_batteryInfo.site, 2, protectInfo);
        m_lastProtectStatus = data.protectStatus;
    }
    else if (data.protectStatus == 0 && m_lastProtectStatus != 0)
    {
        // 保护状态从非零变为零，记录恢复正常
        RecordManager::instance()->exception_record(m_batteryInfo.power_id, m_batteryInfo.site, 2, "保护状态恢复正常");
        m_lastProtectStatus = 0;
    }
    
    // 检查故障状态并记录异常（仅在状态变化时记录）
    if (data.faultStatus != 0 && data.faultStatus != m_lastFaultStatus)
    {
        QString faultInfo = QString("故障状态: 0x%1").arg(data.faultStatus, 0, 16);
        RecordManager::instance()->exception_record(m_batteryInfo.power_id, m_batteryInfo.site, 2, faultInfo);
        m_lastFaultStatus = data.faultStatus;
    }
    else if (data.faultStatus == 0 && m_lastFaultStatus != 0)
    {
        // 故障状态从非零变为零，记录恢复正常
        RecordManager::instance()->exception_record(m_batteryInfo.power_id, m_batteryInfo.site, 2, "故障状态恢复正常");
        m_lastFaultStatus = 0;
    }

    // 检查系统状态是否变化
    if (data.systemStatus != previousSystemStatus)
    {
        // 根据系统状态确定充放电状态
        BatteryChargeState newState = determineChargeState(data.systemStatus);

        // 记录状态变化
        recordChargeStateChange(newState);

        // 更新上一次的系统状态和时间
        m_lastSystemStatus = data.systemStatus;
        m_lastChargeStateChangeTime = QDateTime::currentDateTime();
    }

    emit dataReceived(this, data);
}

// 处理通信错误
void BatteryListForm::onCommunicationError(const QString &errorMessage)
{
    qDebug() << "Battery communication error: " << errorMessage;

    // 记录通信错误异常
    RecordManager::instance()->exception_record(m_batteryInfo.power_id, m_batteryInfo.site, 1, "通信错误: " + errorMessage);

    // 创建一个故障状态的BMS_1数据
    BMS_1 errorData = m_lastData;
    errorData.battery_info.status = 0x02; // 设置故障状态标志

    // 更新全局统计信息
    BatteryStats::instance()->updateBatteryStatus(m_batteryInfo.power_id, errorData);

    // 更新监控状态
    m_monitoringStatus = Error;
    
    // 状态更新后再更新图标
    updateStatusIcon();

    // 发送信号
    emit monitoringStatusChanged(this, m_monitoringStatus);
    emit communicationError(this, errorMessage);
}

// 处理通信超时
void BatteryListForm::onCommunicationTimeout()
{
    qDebug() << "Battery communication timeout";

    // 记录通信超时异常
    RecordManager::instance()->exception_record(m_batteryInfo.power_id, m_batteryInfo.site, 1, "通信超时");

    // 创建一个故障状态的BMS_1数据
    BMS_1 timeoutData = m_lastData;
    timeoutData.battery_info.status = 0x02; // 设置超时故障标志

    // 更新全局统计信息
    BatteryStats::instance()->updateBatteryStatus(m_batteryInfo.power_id, timeoutData);

    // 更新监控状态
    m_monitoringStatus = Error;
    
    // 状态更新后再更新图标
    updateStatusIcon();

    // 发送信号
    emit monitoringStatusChanged(this, m_monitoringStatus);
    emit communicationTimeout(this);
}

// 更新组件样式
void BatteryListForm::updateStyle()
{
    if (m_selected)
    {
        // 选中状态样式 - 蓝色边框和浅蓝色背景
        setStyleSheet("BatteryListForm { border-image: url(:/image/单个电池模块背景_选中.png);} QLabel{ background-color: transparent;}");
    }
    else
    {
        // 未选中状态样式 - 使用原始背景图
        setStyleSheet("BatteryListForm { border-image: url(:/image/单个电池模块背景.png);}QLabel{ background-color: transparent;}");
    }
    
    // 不要在这里重新设置状态图标，因为这会覆盖故障状态
    // 只有在m_monitoringStatus状态变化时才应该改变状态图标
    
    // 强制重绘
    update();
}

// 重写绘制事件，确保样式表被应用
void BatteryListForm::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

// 根据系统状态确定充放电状态
BatteryListForm::BatteryChargeState BatteryListForm::determineChargeState(uint16_t systemStatus) const
{
    // 根据系统状态字的位来判断充放电状态
    // 这里需要根据实际协议来确定正确的位
    // 假设：bit1=充电状态，bit2=放电状态
    const uint16_t CHARGING_BIT = 0x0002;
    const uint16_t DISCHARGING_BIT = 0x0004;

    if (systemStatus & CHARGING_BIT)
    {
        return Charging;
    }
    else if (systemStatus & DISCHARGING_BIT)
    {
        return Discharging;
    }
    else
    {
        return Idle;
    }
}

// 记录充放电状态变化到数据库
void BatteryListForm::recordChargeStateChange(BatteryChargeState newState)
{
    // 仅在状态确实发生变化且不是未知状态时记录
    if (newState == Unknown || newState == m_chargeState)
    {
        return;
    }

    // 使用RecordManager记录充放电状态
    int chargeStatus = (newState == Charging) ? 1 : 0; // 1=充电，0=放电
    // 使用实际的状态变化时间，而不是假设的时间
    RecordManager::instance()->cfd_record(m_batteryInfo.power_id, m_batteryInfo.site, chargeStatus, m_lastChargeStateChangeTime);
    
    qDebug() << "电池" << m_batteryInfo.power_id << "充放电状态变更为:"
             << (newState == Charging ? "充电" : (newState == Discharging ? "放电" : "空闲"))
             << " - 已通过RecordManager记录，开始时间:" << m_lastChargeStateChangeTime.toString("yyyy-MM-dd hh:mm:ss");

    // 更新当前状态
    m_chargeState = newState;

    // 发送状态变化信号
    emit chargeStateChanged(this, newState);
}

// 更新状态图标
void BatteryListForm::updateStatusIcon()
{
    // 根据当前监控状态设置状态图标
    if (m_monitoringStatus == Error) {
        // 故障状态
        ui->label_battery_status->setStyleSheet("border-image: url(:/image/故障.png);");
    } else if (m_isRunning) {
        // 运行状态
        ui->label_battery_status->setStyleSheet("border-image: url(:/image/运行.png);");
    } else {
        // 停止状态
        ui->label_battery_status->setStyleSheet("border-image: url(:/image/停止.png);");
    }
}
