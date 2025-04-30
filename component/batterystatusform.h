#ifndef BATTERYSTATUSFORM_H
#define BATTERYSTATUSFORM_H

#include <QWidget>

namespace Ui {
class BatteryStatusForm;
}

class BatteryStatusForm : public QWidget
{
    Q_OBJECT

public:
    explicit BatteryStatusForm(QWidget *parent = nullptr);
    ~BatteryStatusForm();

private:
    Ui::BatteryStatusForm *ui;
};

#endif // BATTERYSTATUSFORM_H
