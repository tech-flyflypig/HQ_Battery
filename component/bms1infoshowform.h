#ifndef BMS1INFOSHOWFORM_H
#define BMS1INFOSHOWFORM_H

#include <QWidget>
#include "batterylistform.h"

namespace Ui {
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
    // 返回主界面信号
    void backToMain();

private slots:
    // 处理返回按钮点击事件
    void onBackButtonClicked();
    
    // 处理电池数据更新
    void updateBatteryData(BatteryListForm *battery, const BMS_1 &data);
    
    // 处理通信错误
    void handleCommunicationError(BatteryListForm *battery, const QString &errorMessage);
    
    // 处理通信超时
    void handleCommunicationTimeout(BatteryListForm *battery);

private:
    Ui::BMS1InfoShowForm *ui;
    BatteryListForm *m_currentBattery;
    
    // 更新保护状态显示
    void updateProtectionStatus(unsigned int protectStatus);
    
    // 更新告警状态显示
    void updateAlarmStatus(unsigned int alarmStatus);
};

#endif // BMS1INFOSHOWFORM_H
