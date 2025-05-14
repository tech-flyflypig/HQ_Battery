#ifndef RIGHTSTATSFORM_H
#define RIGHTSTATSFORM_H

#include <QWidget>

namespace Ui {
class RightStatsForm;
}

class RightStatsForm : public QWidget
{
    Q_OBJECT

public:
    explicit RightStatsForm(QWidget *parent = nullptr);
    ~RightStatsForm();

private:
    Ui::RightStatsForm *ui;
};

#endif // RIGHTSTATSFORM_H
