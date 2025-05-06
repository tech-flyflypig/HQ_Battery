#include "batterylistform.h"
#include "ui_batterylistform.h"

#include <QPainter>
#include <QStyleOption>
#include <QMouseEvent>
#include <QDebug>

BatteryListForm::BatteryListForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::BatteryListForm)
    , m_selected(false)
    , m_serialWorker(nullptr)
    , m_isRunning(false)
{
    ui->setupUi(this);

    // 设置初始样式
    updateStyle();
}

BatteryListForm::~BatteryListForm()
{
    stopCommunication();
    delete ui;
}

// 设置选中状态
void BatteryListForm::setSelected(bool selected)
{
    if (m_selected != selected)
    {
        m_selected = selected;
        updateStyle();
        emit selectedChanged(m_selected);
    }
}

// 设置电池信息
void BatteryListForm::setBatteryInfo(const battery_info &info)
{
    m_batteryInfo = info;

    // 更新UI显示基本信息
    ui->label_address->setText(m_batteryInfo.site); // 显示电池位置

    // 如果串口已经在运行，需要先停止
    if (m_isRunning)
    {
        stopCommunication();
    }

    // 初始化串口通信
    initSerialCommunication();
}

battery_info BatteryListForm::getBatteryInfo() const
{
    return m_batteryInfo;
}

// 开始串口通信
void BatteryListForm::startCommunication()
{
    if (!m_serialWorker || m_isRunning)
    {
        return;
    }

    // 设置为运行状态图标
    ui->label_battery_status->setStyleSheet("border-image: url(:/image/运行.png);");

    // 启动串口通信
    m_serialWorker->startReading(m_batteryInfo.port_name, m_batteryInfo.type);
    m_isRunning = true;
}

// 停止串口通信
void BatteryListForm::stopCommunication()
{
    if (!m_serialWorker || !m_isRunning)
    {
        return;
    }

    // 设置为停止状态图标
    ui->label_battery_status->setStyleSheet("border-image: url(:/image/停止.png);");

    // 停止串口通信
    m_serialWorker->stopReading();
    m_isRunning = false;
}

// 初始化串口通信
void BatteryListForm::initSerialCommunication()
{
    // 如果已存在，先删除
    if (m_serialWorker)
    {
        delete m_serialWorker;
    }

    // 创建SerialWorker
    m_serialWorker = new SerialWorker(this);

    // 连接信号和槽
    connect(m_serialWorker, &SerialWorker::forwardBatteryData,
            this, &BatteryListForm::onBatteryDataReceived);
    connect(m_serialWorker, &SerialWorker::error,
            this, &BatteryListForm::onCommunicationError);
    connect(m_serialWorker, &SerialWorker::communicationTimeout,
            this, &BatteryListForm::onCommunicationTimeout);
}

// 更新UI显示
void BatteryListForm::updateDisplay(const BMS_1 &data)
{
    // 更新电量显示
    QString socStr = QString::number(data.soc) + "%";

    // 更新温度显示
    QString tempStr = QString::number(data.tempMax / 10.0, 'f', 1) + "℃";

    // 更新容量显示
    QString capacityStr = QString::number(data.ratedCapacity / 100.0, 'f', 1) + "AH";

    // 更新UI标签
    ui->label_remainCapacity->setText(capacityStr);
    ui->label_tempMax->setText(tempStr);

    // 根据状态更新图标
    if (data.alarmStatus > 0)
    {
        // 告警状态
        ui->label_battery_status->setStyleSheet("border-image: url(:/image/告警.png);");
    }
    else if (data.protectStatus > 0)
    {
        // 保护状态
        ui->label_battery_status->setStyleSheet("border-image: url(:/image/保护.png);");
    }
    else if (m_isRunning)
    {
        // 正常运行状态
        ui->label_battery_status->setStyleSheet("border-image: url(:/image/运行.png);");
    }
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

// 处理电池数据接收
void BatteryListForm::onBatteryDataReceived(const BMS_1 &data)
{
    m_lastData = data;
    updateDisplay(data);
    emit dataReceived(this, data);
}

// 处理通信错误
void BatteryListForm::onCommunicationError(const QString &errorMessage)
{
    qDebug() << "Battery communication error: " << errorMessage;
    emit communicationError(this, errorMessage);
}

// 处理通信超时
void BatteryListForm::onCommunicationTimeout()
{
    qDebug() << "Battery communication timeout";
    // 设置超时状态图标
    ui->label_battery_status->setStyleSheet("border-image: url(:/image/停止.png);");
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
