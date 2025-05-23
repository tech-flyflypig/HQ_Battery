#ifndef BATTERYCHARTWIDGET_H
#define BATTERYCHARTWIDGET_H

#include <QWidget>
#include <QTimer>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QDateTimeAxis>
#include <QtCharts/QValueAxis>
#include <QVBoxLayout>
#include <QDateTime>
#include <QQueue>

QT_CHARTS_USE_NAMESPACE

enum ChartType
{
    TEMPERATURE_CHART,
    VOLTAGE_CURRENT_CHART
};

class BatteryChartWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BatteryChartWidget(ChartType type, QWidget *parent = nullptr);
    ~BatteryChartWidget();

public slots:
    // 添加温度数据点
    void addTemperatureData(double temperature);

    // 添加电压电流数据点
    void addVoltageCurrentData(double voltage, double current);

    // 清除所有数据点
    void clearAllData();

    // 启动/停止实时更新
    void startRealTimeUpdate();
    void stopRealTimeUpdate();

private:
    void setupTemperatureChart();
    void setupVoltageCurrentChart();
    void updateChart();

    // 通用图表属性
    QChart *m_chart;
    QChartView *m_chartView;
    QDateTimeAxis *m_axisX;
    QValueAxis *m_axisY;
    QValueAxis *m_axisY2;  // 仅用于电压电流图表的第二个Y轴

    // 温度图表的数据系列
    QLineSeries *m_temperatureSeries;

    // 电压电流图表的数据系列
    QLineSeries *m_voltageSeries;
    QLineSeries *m_currentSeries;

    // 数据存储队列（限制数据点数量）
    QQueue<QPointF> m_temperatureData;
    QQueue<QPointF> m_voltageData;
    QQueue<QPointF> m_currentData;

    // 实时更新相关
    QTimer *m_updateTimer;
    bool m_isUpdating;

    // 图表类型
    ChartType m_chartType;

    // 配置参数
    const int MAX_POINTS = 120;  // 最多显示60个数据点（可以调整）
    const int UPDATE_INTERVAL = 1000;  // 更新间隔（毫秒）
};

#endif // BATTERYCHARTWIDGET_H 
