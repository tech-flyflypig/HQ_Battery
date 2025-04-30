#include "batterystatusform.h"
#include "ui_batterystatusform.h"

BatteryStatusForm::BatteryStatusForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::BatteryStatusForm)
{
    ui->setupUi(this);
}

BatteryStatusForm::~BatteryStatusForm()
{
    delete ui;
}
