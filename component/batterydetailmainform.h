#ifndef BATTERYDETAILMAINFORM_H
#define BATTERYDETAILMAINFORM_H

#include <QWidget>

namespace Ui {
class BatteryDetailMainForm;
}

class BatteryDetailMainForm : public QWidget
{
    Q_OBJECT

public:
    explicit BatteryDetailMainForm(QWidget *parent = nullptr);
    ~BatteryDetailMainForm();

private:
    Ui::BatteryDetailMainForm *ui;
};

#endif // BATTERYDETAILMAINFORM_H
