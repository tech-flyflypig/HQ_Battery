#ifndef RIGHTSTATSFORM_H
#define RIGHTSTATSFORM_H

#include <QWidget>
#include "batterylistform.h"

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

private slots:
    // 更新电池数据显示
    void updateBatteryData(BatteryListForm *battery, const BMS_1 &data);
    
    // 处理通信错误
    void handleCommunicationError(BatteryListForm *battery, const QString &errorMessage);
    
    // 处理通信超时
    void handleCommunicationTimeout(BatteryListForm *battery);

private:
    Ui::RightStatsForm *ui;
    BatteryListForm *m_currentBattery; // 当前显示的电池
};

#endif // RIGHTSTATSFORM_H
