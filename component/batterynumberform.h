#ifndef BATTERYNUMBERFORM_H
#define BATTERYNUMBERFORM_H

#include <QWidget>

namespace Ui {
class BatteryNumberForm;
}

class BatteryNumberForm : public QWidget
{
    Q_OBJECT

public:
    explicit BatteryNumberForm(QWidget *parent = nullptr);
    ~BatteryNumberForm();

private:
    Ui::BatteryNumberForm *ui;
};

#endif // BATTERYNUMBERFORM_H
