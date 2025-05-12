#ifndef BATTERYDETAILMAINFORM_H
#define BATTERYDETAILMAINFORM_H

#include <QWidget>
#include <QTimer>
#include "batterylistform.h"
#include "../utils/Struct.h"

// 图表相关头文件
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
QT_CHARTS_USE_NAMESPACE

namespace Ui
{
    class BatteryDetailMainForm;
}

class BatteryDetailMainForm : public QWidget
{
    Q_OBJECT

public:
    explicit BatteryDetailMainForm(QWidget *parent = nullptr);
    ~BatteryDetailMainForm();

    // 设置电池信息并显示
    void setBatteryInfo(BatteryListForm *battery);

public slots:
    // 返回按钮，回到主界面
    void onBackButtonClicked();

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    // 处理电池数据更新
    void updateBatteryData(BatteryListForm *battery, const BMS_1 &data);
    // 处理通信错误
    void handleCommunicationError(BatteryListForm *battery, const QString &errorMessage);
    // 处理通信超时
    void handleCommunicationTimeout(BatteryListForm *battery);

signals:
    // 返回主界面的信号
    void backToMain();

private:
    Ui::BatteryDetailMainForm *ui;
    BatteryListForm *m_currentBattery;

    // 电压电流曲线图表
    QChartView *m_voltageCurrentChartView;
    QLineSeries *m_voltageSeries;
    QLineSeries *m_currentSeries;

    // 温度曲线图表
    QChartView *m_temperatureChartView;
    QLineSeries *m_temperatureSeries;

    // 时间相关
    QTimer m_updateTimer;
    QList<QDateTime> m_timePoints;

    // 初始化图表
    void initCharts();
    // 更新图表数据
    void updateCharts(const BMS_1 &data);
    // 断开与之前电池的连接
    void disconnectFromBattery();
};

#endif // BATTERYDETAILMAINFORM_H
