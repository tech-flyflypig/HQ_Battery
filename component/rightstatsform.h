#ifndef RIGHTSTATSFORM_H
#define RIGHTSTATSFORM_H

#include <QWidget>
#include "batterylistform.h"
#include <memory> // 添加智能指针头文件

namespace Ui {
class RightStatsForm;
}

class RightStatsForm : public QWidget
{
    Q_OBJECT

public:
    explicit RightStatsForm(QWidget *parent = nullptr);
    ~RightStatsForm();

    // 设置当前选中的电池，并更新显示
    void setBatteryInfo(BatteryListForm *battery);

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    // 更新电池数据显示
    void updateBatteryData(BatteryListForm *battery, const BMS_1 &data);
    
    // 处理通信错误
    void handleCommunicationError(BatteryListForm *battery, const QString &errorMessage);
    
    // 处理通信超时
    void handleCommunicationTimeout(BatteryListForm *battery);
    
    // 处理监控状态变化
    void handleMonitoringStatusChanged(BatteryListForm *battery, BatteryListForm::MonitoringStatus status);

private:
    // 动态调整内部布局的上边距
    void adjustTopMargins();

    Ui::RightStatsForm *ui;
    std::weak_ptr<BatteryListForm> m_currentBattery; // 使用弱引用避免所有权问题
};

#endif // RIGHTSTATSFORM_H
