#include "batterydetailmainform.h"
#include "ui_batterydetailmainform.h"
#include <QDebug>
#include <QDateTime>
#include <QtCharts/QValueAxis>
#include <QtCharts/QDateTimeAxis>

BatteryDetailMainForm::BatteryDetailMainForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::BatteryDetailMainForm)
    , m_currentBattery(nullptr)
{
    ui->setupUi(this);
    
    // 初始化图表
    initCharts();
    
    // 连接返回按钮
    connect(ui->btn_back, &QPushButton::clicked, this, &BatteryDetailMainForm::onBackButtonClicked);
    
    // 设置数据更新定时器
    m_updateTimer.setInterval(1000);  // 1秒更新一次
}

BatteryDetailMainForm::~BatteryDetailMainForm()
{
    disconnectFromBattery();
    delete ui;
}

void BatteryDetailMainForm::setBatteryInfo(BatteryListForm *battery)
{
    // 断开与之前电池的连接
    disconnectFromBattery();
    
    // 设置当前电池
    m_currentBattery = battery;
    
    if (battery) {
        // 获取电池信息
        battery_info info = battery->getBatteryInfo();
        BMS_1 lastData = battery->getLastData();  // 获取最新数据
        
        // 更新UI显示
        ui->label_battery_id->setText(info.power_id);
        ui->label_battery_site->setText(info.site);
        ui->label_port->setText(info.port_name);
        ui->label_battery_type->setText(info.type);
        
        // 连接信号
        connect(battery, &BatteryListForm::dataReceived, 
                this, &BatteryDetailMainForm::updateBatteryData);
        connect(battery, &BatteryListForm::communicationError,
                this, &BatteryDetailMainForm::handleCommunicationError);
        connect(battery, &BatteryListForm::communicationTimeout,
                this, &BatteryDetailMainForm::handleCommunicationTimeout);
        
        // 清空图表数据
        m_voltageSeries->clear();
        m_currentSeries->clear();
        m_temperatureSeries->clear();
        m_timePoints.clear();
        
        // 使用初始数据更新一次
        updateBatteryData(battery, lastData);
        
        // 开始定时器
        m_updateTimer.start();
    }
}

void BatteryDetailMainForm::onBackButtonClicked()
{
    // 停止更新定时器
    m_updateTimer.stop();
    
    // 发送返回信号
    emit backToMain();
}

void BatteryDetailMainForm::updateBatteryData(BatteryListForm *battery, const BMS_1 &data)
{
    if (battery != m_currentBattery) return;
    
    // 更新基本信息显示
    ui->label_soc->setText(QString::number(data.soc) + "%");
    ui->label_temp->setText(QString::number(data.tempMax / 10.0, 'f', 1) + "℃");
    ui->label_capacity->setText(QString::number(data.ratedCapacity / 100.0, 'f', 1) + "AH");
    ui->label_voltage->setText(QString::number(data.voltage / 100.0, 'f', 2) + "V");
    ui->label_current->setText(QString::number(data.current / 100.0, 'f', 2) + "A");
    
    // 更新图表
    updateCharts(data);
}

void BatteryDetailMainForm::handleCommunicationError(BatteryListForm *battery, const QString &errorMessage)
{
    if (battery != m_currentBattery) return;
    
    // 处理通信错误
    ui->label_status->setText("通信错误: " + errorMessage);
}

void BatteryDetailMainForm::handleCommunicationTimeout(BatteryListForm *battery)
{
    if (battery != m_currentBattery) return;
    
    // 处理通信超时
    ui->label_status->setText("通信超时");
}

void BatteryDetailMainForm::initCharts()
{
    // 创建电压电流图表
    QChart *voltageCurrentChart = new QChart();
    voltageCurrentChart->setTitle("电压/电流曲线");
    
    m_voltageSeries = new QLineSeries();
    m_voltageSeries->setName("电压(V)");
    
    m_currentSeries = new QLineSeries();
    m_currentSeries->setName("电流(A)");
    
    voltageCurrentChart->addSeries(m_voltageSeries);
    voltageCurrentChart->addSeries(m_currentSeries);
    
    // 创建温度图表
    QChart *temperatureChart = new QChart();
    temperatureChart->setTitle("温度曲线");
    
    m_temperatureSeries = new QLineSeries();
    m_temperatureSeries->setName("温度(℃)");
    
    temperatureChart->addSeries(m_temperatureSeries);
    
    // 创建坐标轴
    QDateTimeAxis *axisX1 = new QDateTimeAxis;
    axisX1->setTitleText("时间");
    axisX1->setFormat("hh:mm:ss");
    
    QValueAxis *axisY1 = new QValueAxis;
    axisY1->setTitleText("值");
    
    QDateTimeAxis *axisX2 = new QDateTimeAxis;
    axisX2->setTitleText("时间");
    axisX2->setFormat("hh:mm:ss");
    
    QValueAxis *axisY2 = new QValueAxis;
    axisY2->setTitleText("温度(℃)");
    
    // 将坐标轴添加到图表
    voltageCurrentChart->addAxis(axisX1, Qt::AlignBottom);
    voltageCurrentChart->addAxis(axisY1, Qt::AlignLeft);
    
    temperatureChart->addAxis(axisX2, Qt::AlignBottom);
    temperatureChart->addAxis(axisY2, Qt::AlignLeft);
    
    // 将系列附加到坐标轴
    m_voltageSeries->attachAxis(axisX1);
    m_voltageSeries->attachAxis(axisY1);
    m_currentSeries->attachAxis(axisX1);
    m_currentSeries->attachAxis(axisY1);
    
    m_temperatureSeries->attachAxis(axisX2);
    m_temperatureSeries->attachAxis(axisY2);
    
    // 创建图表视图
    m_voltageCurrentChartView = new QChartView(voltageCurrentChart);
    m_voltageCurrentChartView->setRenderHint(QPainter::Antialiasing);
    
    m_temperatureChartView = new QChartView(temperatureChart);
    m_temperatureChartView->setRenderHint(QPainter::Antialiasing);
    
    // 添加到布局
    ui->layout_chart_voltage_current->addWidget(m_voltageCurrentChartView);
    ui->layout_chart_temperature->addWidget(m_temperatureChartView);
}

void BatteryDetailMainForm::updateCharts(const BMS_1 &data)
{
    // 获取当前时间
    QDateTime now = QDateTime::currentDateTime();
    m_timePoints.append(now);
    
    // 限制数据点数量，保留最近30个点
    if (m_timePoints.size() > 30) {
        m_timePoints.removeFirst();
        m_voltageSeries->remove(0);
        m_currentSeries->remove(0);
        m_temperatureSeries->remove(0);
    }
    
    // 添加新数据点
    qreal msecsSinceEpoch = now.toMSecsSinceEpoch();
    m_voltageSeries->append(msecsSinceEpoch, data.voltage / 100.0);
    m_currentSeries->append(msecsSinceEpoch, data.current / 100.0);
    m_temperatureSeries->append(msecsSinceEpoch, data.tempMax / 10.0);
    
    // 更新坐标轴范围
    if (m_timePoints.size() > 1) {
        QDateTimeAxis *axisX1 = qobject_cast<QDateTimeAxis*>(m_voltageCurrentChartView->chart()->axes(Qt::Horizontal).first());
        QDateTimeAxis *axisX2 = qobject_cast<QDateTimeAxis*>(m_temperatureChartView->chart()->axes(Qt::Horizontal).first());
        
        axisX1->setRange(m_timePoints.first(), m_timePoints.last());
        axisX2->setRange(m_timePoints.first(), m_timePoints.last());
    }
}

void BatteryDetailMainForm::disconnectFromBattery()
{
    if (m_currentBattery) {
        // 断开与当前电池的所有连接
        disconnect(m_currentBattery, &BatteryListForm::dataReceived, 
                   this, &BatteryDetailMainForm::updateBatteryData);
        disconnect(m_currentBattery, &BatteryListForm::communicationError,
                   this, &BatteryDetailMainForm::handleCommunicationError);
        disconnect(m_currentBattery, &BatteryListForm::communicationTimeout,
                   this, &BatteryDetailMainForm::handleCommunicationTimeout);
        
        m_currentBattery = nullptr;
    }
    
    m_updateTimer.stop();
}
