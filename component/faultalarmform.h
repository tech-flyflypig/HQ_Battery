#ifndef FAULTALARMFORM_H
#define FAULTALARMFORM_H

#include <QWidget>

namespace Ui {
class FaultAlarmForm;
}

class FaultAlarmForm : public QWidget
{
    Q_OBJECT

public:
    explicit FaultAlarmForm(QWidget *parent = nullptr);
    ~FaultAlarmForm();

private:
    Ui::FaultAlarmForm *ui;
};

#endif // FAULTALARMFORM_H
