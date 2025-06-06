#ifndef CHARGEANDDISCHARGERECORDFORM_H
#define CHARGEANDDISCHARGERECORDFORM_H

#include <QWidget>

namespace Ui {
class ChargeAndDischargeRecordForm;
}

class ChargeAndDischargeRecordForm : public QWidget
{
    Q_OBJECT

public:
    explicit ChargeAndDischargeRecordForm(QWidget *parent = nullptr);
    ~ChargeAndDischargeRecordForm();

private:
    Ui::ChargeAndDischargeRecordForm *ui;
};

#endif // CHARGEANDDISCHARGERECORDFORM_H
