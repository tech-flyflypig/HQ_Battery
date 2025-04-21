#ifndef BMS1INFOSHOWFORM_H
#define BMS1INFOSHOWFORM_H

#include <QWidget>

namespace Ui {
class BMS1InfoShowForm;
}

class BMS1InfoShowForm : public QWidget
{
    Q_OBJECT

public:
    explicit BMS1InfoShowForm(QWidget *parent = nullptr);
    ~BMS1InfoShowForm();

private:
    Ui::BMS1InfoShowForm *ui;
};

#endif // BMS1INFOSHOWFORM_H
