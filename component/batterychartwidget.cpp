#include "batterychartwidget.h"
#include <QDebug>
#include <QGraphicsLayout>

BatteryChartWidget::BatteryChartWidget(ChartType type, QWidget *parent)
    : QWidget(parent)
    , m_chartType(type)
    , m_isUpdating(false)
{
    // 设置此Widget的背景为透明
    this->setStyleSheet("background: transparent;");

    // 创建布局
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0); // 设置为0去除外部边距
    layout->setSpacing(0); // 移除间距

    // 创建图表
    m_chart = new QChart();
    m_chart->setAnimationOptions(QChart::NoAnimation); // 禁用动画以提高性能
    m_chart->legend()->setVisible(true);
    m_chart->legend()->setAlignment(Qt::AlignBottom);
    m_chart->legend()->setLabelColor(Qt::white); // 设置图例标签为白色
    m_chart->legend()->setFont(QFont("Microsoft YaHei UI", 10, QFont::Bold)); // 使用更大更粗的字体

    // 调整图例的整体大小和边距
    m_chart->legend()->setContentsMargins(10, 6, 10, 6);

    // 关键设置：图表背景完全透明
    m_chart->setBackgroundBrush(Qt::transparent);
    m_chart->setPlotAreaBackgroundBrush(QBrush(QColor(40, 50, 70, 200)));
    m_chart->setPlotAreaBackgroundVisible(true);

    m_chart->setTitleBrush(QBrush(Qt::white));
    m_chart->setTitleFont(QFont("Microsoft YaHei UI", 10, QFont::Bold));
    m_chart->setMargins(QMargins(0, 0, 0, 0)); // 设置图表内部边距为0
    m_chart->setBackgroundRoundness(0); // 移除背景圆角

    // 完全移除外部边距
    if (m_chart->layout())
    {
        m_chart->layout()->setContentsMargins(0, 0, 0, 0);
    }

    // 设置图表视图
    m_chartView = new QChartView(m_chart);
    m_chartView->setRenderHint(QPainter::Antialiasing);
    m_chartView->setStyleSheet("background: transparent; border: none;"); // CSS样式移除背景和边框
    m_chartView->setBackgroundBrush(Qt::transparent); // 设置视图背景为透明
    m_chartView->viewport()->setStyleSheet("background: transparent;"); // 设置视口背景为透明
    m_chartView->setFrameShape(QFrame::NoFrame); // 移除边框
    m_chartView->setFrameShadow(QFrame::Plain); // 无阴影
    m_chartView->setLineWidth(0); // 线宽为0
    m_chartView->setContentsMargins(0, 0, 0, 0); // 设置内容边距为0
    m_chartView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding); // 扩展填充

    layout->addWidget(m_chartView);

    // 初始化时间轴（X轴）
    m_axisX = new QDateTimeAxis;
    m_axisX->setFormat("HH:mm:ss");
    m_axisX->setTitleText("时间");
    m_axisX->setLabelsColor(Qt::white);
    m_axisX->setTitleBrush(Qt::white);
    m_axisX->setGridLineVisible(true);
    m_axisX->setGridLineColor(QColor(60, 70, 90));

    // 创建Y轴
    m_axisY = new QValueAxis;
    m_axisY->setLabelsColor(Qt::white);
    m_axisY->setTitleBrush(Qt::white);
    m_axisY->setGridLineVisible(true);
    m_axisY->setGridLineColor(QColor(60, 70, 90));

    // 根据图表类型初始化不同的图表
    if (m_chartType == TEMPERATURE_CHART)
    {
        setupTemperatureChart();
    }
    else
    {
        setupVoltageCurrentChart();
    }

    // 设置计时器
    m_updateTimer = new QTimer(this);
    connect(m_updateTimer, &QTimer::timeout, this, &BatteryChartWidget::updateChart);
}

BatteryChartWidget::~BatteryChartWidget()
{
    // 停止更新
    if (m_updateTimer)
    {
        if (m_updateTimer->isActive())
        {
            m_updateTimer->stop();
        }
        delete m_updateTimer;
        m_updateTimer = nullptr;
    }

    // 清理图表资源
    if (m_chartView)
    {
        // 不需要显式删除m_chart，因为QChartView会在其析构函数中删除它
        delete m_chartView;
        m_chartView = nullptr;
    }

    // m_chart不需要显式删除，已由QChartView删除
    m_chart = nullptr;

    // 轴和数据序列也不需要显式删除，因为它们是图表的子对象，会自动删除
    m_axisX = nullptr;
    m_axisY = nullptr;
    m_axisY2 = nullptr;

    m_temperatureSeries = nullptr;
    m_voltageSeries = nullptr;
    m_currentSeries = nullptr;
}

void BatteryChartWidget::setupTemperatureChart()
{
    // 设置图表标题
    //m_chart->setTitle("实时温度曲线");

    // 温度Y轴设置
    m_axisY->setTitleText("温度 (°C)");
    m_axisY->setRange(0, 60); // 温度范围0-60℃

    // 创建温度线系列
    m_temperatureSeries = new QLineSeries();
    m_temperatureSeries->setName("温度");
    m_temperatureSeries->setPen(QPen(QColor(85, 255, 255), 2));

    // 添加系列到图表
    m_chart->addSeries(m_temperatureSeries);

    // 将轴添加到图表，并将系列附加到轴
    m_chart->addAxis(m_axisX, Qt::AlignBottom);
    m_chart->addAxis(m_axisY, Qt::AlignLeft);
    m_temperatureSeries->attachAxis(m_axisX);
    m_temperatureSeries->attachAxis(m_axisY);

    // 设置时间范围（显示最近10分钟的数据）
    QDateTime now = QDateTime::currentDateTime();
    m_axisX->setRange(now.addSecs(-600), now);
}

void BatteryChartWidget::setupVoltageCurrentChart()
{
    // 设置图表标题
    //m_chart->setTitle("实时电压电流曲线");

    // 电压Y轴设置（左侧）
    m_axisY->setTitleText("电压 (V)");
    m_axisY->setRange(20, 30); // 电压范围20-30V

    // 创建电流Y轴（右侧）
    m_axisY2 = new QValueAxis;
    m_axisY2->setTitleText("电流 (A)");
    m_axisY2->setRange(-5, 5); // 电流范围-5A到5A
    m_axisY2->setLabelsColor(Qt::white);
    m_axisY2->setTitleBrush(Qt::white);
    m_axisY2->setGridLineVisible(false);

    // 创建电压线系列
    m_voltageSeries = new QLineSeries();
    m_voltageSeries->setName("电压");
    m_voltageSeries->setPen(QPen(QColor(85, 255, 255), 2));

    // 创建电流线系列
    m_currentSeries = new QLineSeries();
    m_currentSeries->setName("电流");
    m_currentSeries->setPen(QPen(QColor(255, 153, 0), 2));

    // 添加系列到图表
    m_chart->addSeries(m_voltageSeries);
    m_chart->addSeries(m_currentSeries);

    // 将轴添加到图表
    m_chart->addAxis(m_axisX, Qt::AlignBottom);
    m_chart->addAxis(m_axisY, Qt::AlignLeft);
    m_chart->addAxis(m_axisY2, Qt::AlignRight);

    // 将系列附加到轴
    m_voltageSeries->attachAxis(m_axisX);
    m_voltageSeries->attachAxis(m_axisY);

    m_currentSeries->attachAxis(m_axisX);
    m_currentSeries->attachAxis(m_axisY2);

    // 设置时间范围（显示最近10分钟的数据）
    QDateTime now = QDateTime::currentDateTime();
    m_axisX->setRange(now.addSecs(-60), now);
}

void BatteryChartWidget::addTemperatureData(double temperature)
{
    if (m_chartType != TEMPERATURE_CHART)
    {
        qWarning() << "尝试向非温度图表添加温度数据";
        return;
    }

    QDateTime now = QDateTime::currentDateTime();
    qint64 timestamp = now.toMSecsSinceEpoch();

    // 添加新数据点到队列
    m_temperatureData.enqueue(QPointF(timestamp, temperature));

    // 如果数据点超过最大数量，移除最旧的数据点
    while (m_temperatureData.size() > MAX_POINTS)
    {
        m_temperatureData.dequeue();
    }

    // 更新图表
    updateChart();
}

void BatteryChartWidget::addVoltageCurrentData(double voltage, double current)
{
    if (m_chartType != VOLTAGE_CURRENT_CHART)
    {
        qWarning() << "尝试向非电压电流图表添加电压电流数据";
        return;
    }

    QDateTime now = QDateTime::currentDateTime();
    qint64 timestamp = now.toMSecsSinceEpoch();

    // 添加新数据点到队列
    m_voltageData.enqueue(QPointF(timestamp, voltage));
    m_currentData.enqueue(QPointF(timestamp, current));

    // 如果数据点超过最大数量，移除最旧的数据点
    while (m_voltageData.size() > MAX_POINTS)
    {
        m_voltageData.dequeue();
    }

    while (m_currentData.size() > MAX_POINTS)
    {
        m_currentData.dequeue();
    }

    // 更新图表
    updateChart();
}

void BatteryChartWidget::clearAllData()
{
    // 清除所有数据队列
    m_temperatureData.clear();
    m_voltageData.clear();
    m_currentData.clear();

    // 清除图表系列数据
    if (m_chartType == TEMPERATURE_CHART)
    {
        m_temperatureSeries->clear();
    }
    else
    {
        m_voltageSeries->clear();
        m_currentSeries->clear();
    }

    // 重置X轴时间范围
    QDateTime now = QDateTime::currentDateTime();
    m_axisX->setRange(now.addSecs(-60), now);
}

void BatteryChartWidget::startRealTimeUpdate()
{
    if (!m_isUpdating)
    {
        m_isUpdating = true;
        m_updateTimer->start(UPDATE_INTERVAL);
    }
}

void BatteryChartWidget::stopRealTimeUpdate()
{
    if (m_isUpdating)
    {
        m_isUpdating = false;
        m_updateTimer->stop();
    }
}

void BatteryChartWidget::updateChart()
{
    QDateTime now = QDateTime::currentDateTime();

    // 更新X轴范围（显示最近1分钟的数据）
    m_axisX->setRange(now.addSecs(-60), now);

    if (m_chartType == TEMPERATURE_CHART)
    {
        // 清除并重新添加所有温度数据点
        m_temperatureSeries->clear();
        for (const QPointF &point : m_temperatureData)
        {
            m_temperatureSeries->append(point);
        }

        // 动态调整Y轴范围，包含所有数据的最小值和最大值
        if (!m_temperatureData.isEmpty())
        {
            double minTemp = 100, maxTemp = 0;
            for (const QPointF &point : m_temperatureData)
            {
                minTemp = qMin(minTemp, point.y());
                maxTemp = qMax(maxTemp, point.y());
            }

            // 确保有足够的边距
            minTemp = qMax(0.0, minTemp - 5);
            maxTemp = qMin(100.0, maxTemp + 5);

            m_axisY->setRange(minTemp, maxTemp);
        }
    }
    else
    {
        // 清除并重新添加所有电压和电流数据点
        m_voltageSeries->clear();
        m_currentSeries->clear();

        for (const QPointF &point : m_voltageData)
        {
            m_voltageSeries->append(point);
        }

        for (const QPointF &point : m_currentData)
        {
            m_currentSeries->append(point);
        }

        // 动态调整电压Y轴范围
        if (!m_voltageData.isEmpty())
        {
            double minVoltage = 100, maxVoltage = 0;
            for (const QPointF &point : m_voltageData)
            {
                minVoltage = qMin(minVoltage, point.y());
                maxVoltage = qMax(maxVoltage, point.y());
            }

            // 确保有足够的边距
            minVoltage = qMax(0.0, minVoltage - 1);
            maxVoltage = qMin(100.0, maxVoltage + 1);

            m_axisY->setRange(minVoltage, maxVoltage);
        }

        // 动态调整电流Y轴范围
        if (!m_currentData.isEmpty())
        {
            double minCurrent = 100, maxCurrent = -100;
            for (const QPointF &point : m_currentData)
            {
                minCurrent = qMin(minCurrent, point.y());
                maxCurrent = qMax(maxCurrent, point.y());
            }

            // 确保有足够的边距
            minCurrent = qMax(-20.0, minCurrent - 1);
            maxCurrent = qMin(20.0, maxCurrent + 1);

            m_axisY2->setRange(minCurrent, maxCurrent);
        }
    }
}
