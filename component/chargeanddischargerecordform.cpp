#include "chargeanddischargerecordform.h"
#include "ui_chargeanddischargerecordform.h"

ChargeAndDischargeRecordForm::ChargeAndDischargeRecordForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ChargeAndDischargeRecordForm)
{
    ui->setupUi(this);
}

ChargeAndDischargeRecordForm::~ChargeAndDischargeRecordForm()
{
    delete ui;
}
