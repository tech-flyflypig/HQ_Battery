#ifndef BATTERYLISTFORM_H
#define BATTERYLISTFORM_H

#include <QWidget>

namespace Ui {
class BatteryListForm;
}

class BatteryListForm : public QWidget
{
    Q_OBJECT

public:
    explicit BatteryListForm(QWidget *parent = nullptr);
    ~BatteryListForm();

private:
    Ui::BatteryListForm *ui;
};

#endif // BATTERYLISTFORM_H
