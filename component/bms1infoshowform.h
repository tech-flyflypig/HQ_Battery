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

private:
    Ui::BMS1InfoShowForm *ui;
    BatteryListForm *m_currentBattery;
};

#endif // BMS1INFOSHOWFORM_H
