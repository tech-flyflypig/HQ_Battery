#include "rightstatsform.h"
#include "ui_rightstatsform.h"
#include <QDebug>
#include <QResizeEvent>

RightStatsForm::RightStatsForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::RightStatsForm)
      // m_currentBattery 现在是 weak_ptr，不需要初始化
{
    ui->setupUi(this);
}

RightStatsForm::~RightStatsForm()
{
    // 不需要手动断开连接，m_currentBattery 作为弱引用
    // 不会影响对象的生命周期
    delete ui;
}

void RightStatsForm::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    adjustTopMargins();
}

void RightStatsForm::adjustTopMargins()
{
    // 基准高度（UI 文件中定义的）
    const double kBaseHeight = 951.0;
    // 基准 topMargin 值
    const int kBaseBatteryStatusMargin = 51;
    const int kBaseRunParamMargin = 69;
    const int kBaseDeviceInfoMargin = 52;

    // 当前高度
    double currentHeight = this->height();

    // 计算缩放比例
    double ratio = currentHeight / kBaseHeight;

    // 限制缩放范围，避免过小或过大
    if (ratio < 0.8) ratio = 0.8;
    if (ratio > 1.5) ratio = 1.5;

    // 动态计算新的 topMargin
    int newBatteryStatusMargin = static_cast<int>(kBaseBatteryStatusMargin * ratio);
    int newRunParamMargin = static_cast<int>(kBaseRunParamMargin * ratio);
    int newDeviceInfoMargin = static_cast<int>(kBaseDeviceInfoMargin * ratio);

    // 获取三个容器的主布局并调整 topMargin
    if (QLayout *layout = ui->widget_battery_status->layout())
    {
        QMargins margins = layout->contentsMargins();
        margins.setTop(newBatteryStatusMargin);
        layout->setContentsMargins(margins);
    }

    if (QLayout *layout = ui->run_param->layout())
    {
        QMargins margins = layout->contentsMargins();
        margins.setTop(newRunParamMargin);
        layout->setContentsMargins(margins);
    }

    if (QLayout *layout = ui->device_info->layout())
    {
        QMargins margins = layout->contentsMargins();
        margins.setTop(newDeviceInfoMargin);
        layout->setContentsMargins(margins);
    }
}

void RightStatsForm::setBatteryInfo(BatteryListForm *battery)
{
    // 如果原来的 weak_ptr 还有效，先断开连接
    auto currentBattery = m_currentBattery.lock(); // 尝试获取强引用
    if (currentBattery)
    {
        // 断开信号连接
        disconnect(currentBattery.get(), &BatteryListForm::dataReceived,
                   this, &RightStatsForm::updateBatteryData);
        disconnect(currentBattery.get(), &BatteryListForm::communicationError,
                   this, &RightStatsForm::handleCommunicationError);
        disconnect(currentBattery.get(), &BatteryListForm::communicationTimeout,
                   this, &RightStatsForm::handleCommunicationTimeout);
        disconnect(currentBattery.get(), &BatteryListForm::monitoringStatusChanged,
                   this, &RightStatsForm::handleMonitoringStatusChanged);
    }

    // 如果提供了新的电池对象
    if (battery)
    {
        // 尝试获取 shared_ptr
        try
        {
            auto sharedBattery = battery->getSharedPtr(); // 使用 BatteryListForm 的 shared_from_this()
            m_currentBattery = sharedBattery; // 存储弱引用

            // 获取电池信息
            battery_info info = battery->getBatteryInfo();
            BMS_1 lastData = battery->getLastData();

            qDebug() << "RightStatsForm: 设置电池信息 - ID:" << info.power_id << ", 位置:" << info.site;

            // 更新UI显示电池基本信息
            updateBatteryData(battery, lastData);

            // 连接信号，接收电池数据更新
            connect(battery, &BatteryListForm::dataReceived,
                    this, &RightStatsForm::updateBatteryData);
            connect(battery, &BatteryListForm::communicationError,
                    this, &RightStatsForm::handleCommunicationError);
            connect(battery, &BatteryListForm::communicationTimeout,
                    this, &RightStatsForm::handleCommunicationTimeout);
            connect(battery, &BatteryListForm::monitoringStatusChanged,
                    this, &RightStatsForm::handleMonitoringStatusChanged);
                    
            // 立即同步监控状态
            handleMonitoringStatusChanged(battery, battery->getMonitoringStatus());
        }
        catch (const std::bad_weak_ptr &e)
        {
            // 如果无法获取shared_ptr，这可能是因为对象不是由shared_ptr管理的
            qDebug() << "警告: 无法获取电池对象的shared_ptr: " << e.what();
        }
    }
    else
    {
        // 清除弱引用
        m_currentBattery.reset();
    }
}

void RightStatsForm::updateBatteryData(BatteryListForm *battery, const BMS_1 &data)
{
    // 增强的安全检查
    if (!battery) return; // 如果指针为空，直接返回

    // 检查弱引用是否指向同一个对象
    auto currentBattery = m_currentBattery.lock();
    if (!currentBattery || currentBattery.get() != battery) return;

    // 更新电池信息显示
    // 更新基本参数
    if (ui && ui->label_ratedCapacity)
    {
        ui->label_ratedCapacity->setText(QString::number(data.ratedCapacity / 100.0 * 1000, 'f', 1));
    }

    if (ui && ui->label_remainCapacity)
    {
        ui->label_remainCapacity->setText(QString::number(data.remainCapacity / 100.0 * 1000, 'f', 1));
    }

    // 电流分为充电电流和放电电流
    double current = data.current / 100.0;
    if (ui && ui->label_current_out)
    {
        ui->label_current_out->setText(current < 0 ? QString::number(-current, 'f', 2) : "0.00");
    }

    if (ui && ui->label_current_in)
    {
        ui->label_current_in->setText(current > 0 ? QString::number(current, 'f', 2) : "0.00");
    }
    ui->label_pcbtemp->setText(QString::number(data.ambientTemp / 10.0, 'f', 1));
    ui->label_voltage->setText(QString::number(data.voltage / 100.0, 'f', 2) );
    ui->label_tempMax->setText(QString::number(data.tempMax / 10.0, 'f', 1));
    // ui->label_sitecom->setText(data.battery_info.site);
    // ui->label_port->setText(data.battery_info.port_name);
    ui->label_upload_time->setText(data.battery_info.last_time.toString("yyyy-MM-dd HH:mm:ss"));

    // 更新电源状态（开启/关闭）
    // 根据 systemStatus 位标志判断：bit0=放电(0x0001), bit1=充电(0x0002)
    const uint16_t kDischargingBit = 0x0001;  // bit0
    const uint16_t kChargingBit = 0x0002;     // bit1

    bool isPowerOn = (data.systemStatus & kDischargingBit) || (data.systemStatus & kChargingBit);
    ui->label_battery_open->setText(isPowerOn ? "开启" : "关闭");

    // 更新供电方式（交流/直流）
    // 充电时（电流>0）为交流，放电或空闲时为直流
    if (data.current > 0) {
        ui->label_power->setText("交流");
    } else {
        ui->label_power->setText("直流");
    }
    try
    {
        battery_info info = battery->getBatteryInfo();
        ui->label_ip->setText(info.site);
        // ui->label_sitecom->setText(info.site);
        ui->label_port->setText(info.port_name);

    }
    catch (...)
    {
        // 安全捕获任何异常，避免崩溃
        qDebug() << "访问电池信息时发生异常";
    }
}

void RightStatsForm::handleCommunicationError(BatteryListForm *battery, const QString &errorMessage)
{
    // 检查弱引用是否有效且指向同一个对象
    auto currentBattery = m_currentBattery.lock();
    if (!currentBattery || currentBattery.get() != battery) return;
    ui->label_battery_status->setStyleSheet("border-image: url(:/image/故障.png);");

    qDebug() << "通信错误: " << errorMessage;
}

void RightStatsForm::handleCommunicationTimeout(BatteryListForm *battery)
{
    // 检查弱引用是否有效且指向同一个对象
    auto currentBattery = m_currentBattery.lock();
    if (!currentBattery || currentBattery.get() != battery) return;
    ui->label_battery_status->setStyleSheet("border-image: url(:/image/故障.png);");
    qDebug() << "通信超时";
}

void RightStatsForm::handleMonitoringStatusChanged(BatteryListForm *battery, BatteryListForm::MonitoringStatus status)
{
    // 检查弱引用是否有效且指向同一个对象
    auto currentBattery = m_currentBattery.lock();
    if (!currentBattery || currentBattery.get() != battery) return;
    
    // 根据监控状态更新UI
    switch (status) {
        case BatteryListForm::Running:
            ui->label_battery_status->setStyleSheet("border-image: url(:/image/运行.png);");
            break;
        case BatteryListForm::Stopped:
            ui->label_battery_status->setStyleSheet("border-image: url(:/image/停止.png);");
            break;
        case BatteryListForm::Error:
            ui->label_battery_status->setStyleSheet("border-image: url(:/image/故障.png);");
            break;
        default:
            break;
    }
    
    qDebug() << "电池监控状态变化: " << status;
}
