#ifndef CHARGEANDDISCHARGERECORDFORM_H
#define CHARGEANDDISCHARGERECORDFORM_H

#include <QWidget>

namespace Ui
{
    class ChargeAndDischargeRecordForm;
}

class ChargeAndDischargeRecordForm : public QWidget
{
    Q_OBJECT

public:
    explicit ChargeAndDischargeRecordForm(QWidget *parent = nullptr);
    ~ChargeAndDischargeRecordForm();

private slots:
    void on_btn_search_clicked();

    void on_btn_out_excel_clicked();

    void on_btn_delete_clicked();

private:
    Ui::ChargeAndDischargeRecordForm *ui;
};

#endif // CHARGEANDDISCHARGERECORDFORM_H
