#ifndef PROTECTSTATUSFORM_H
#define PROTECTSTATUSFORM_H

#include <QWidget>

namespace Ui {
class ProtectStatusForm;
}

class ProtectStatusForm : public QWidget
{
    Q_OBJECT

public:
    explicit ProtectStatusForm(QWidget *parent = nullptr);
    ~ProtectStatusForm();

private:
    Ui::ProtectStatusForm *ui;
};

#endif // PROTECTSTATUSFORM_H
