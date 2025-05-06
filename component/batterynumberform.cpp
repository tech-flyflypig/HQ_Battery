#include "batterynumberform.h"
#include "ui_batterynumberform.h"

BatteryNumberForm::BatteryNumberForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::BatteryNumberForm)
{
    ui->setupUi(this);
}

BatteryNumberForm::~BatteryNumberForm()
{
    delete ui;
}
