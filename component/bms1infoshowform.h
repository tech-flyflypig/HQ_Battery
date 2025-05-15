#ifndef BMS1INFOSHOWFORM_H
#define BMS1INFOSHOWFORM_H

#include <QWidget>
#include "component/batterylistform.h"
#include "component/batterychartwidget.h"

namespace Ui
{
    class BMS1InfoShowForm;
}

class BMS1InfoShowForm : public QWidget
{
    Q_OBJECT

public:
    explicit BMS1InfoShowForm(QWidget *parent = nullptr);
    ~BMS1InfoShowForm();

    // 设置电池信息
    void setBatteryInfo(BatteryListForm *battery);
signals:


private slots:
    // 处理电池数据更新
    void updateBatteryData(BatteryListForm *battery, const BMS_1 &data);

    // 处理通信错误
    void handleCommunicationError(BatteryListForm *battery, const QString &errorMessage);

    // 处理通信超时
    void handleCommunicationTimeout(BatteryListForm *battery);

private:
    // 初始化RadioButton为只读状态
    void initializeRadioButtons();

    // 更新保护状态显示
    void updateProtectionStatus(unsigned int protectStatus);

    // 更新告警状态显示
    void updateAlarmStatus(unsigned int alarmStatus);

    Ui::BMS1InfoShowForm *ui;
    BatteryListForm *m_currentBattery;

    // 曲线图组件
    BatteryChartWidget *m_temperatureChart;
    BatteryChartWidget *m_voltageCurrentChart;

    // 标记是否已经初始化图表
    bool m_chartsInitialized;
};

#endif // BMS1INFOSHOWFORM_H
