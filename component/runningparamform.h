#ifndef RUNNINGPARAMFORM_H
#define RUNNINGPARAMFORM_H

#include <QWidget>

namespace Ui {
class RunningParamForm;
}

class RunningParamForm : public QWidget
{
    Q_OBJECT

public:
    explicit RunningParamForm(QWidget *parent = nullptr);
    ~RunningParamForm();

private:
    Ui::RunningParamForm *ui;
};

#endif // RUNNINGPARAMFORM_H
