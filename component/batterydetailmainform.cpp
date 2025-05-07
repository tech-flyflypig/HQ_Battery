#include "batterydetailmainform.h"
#include "ui_batterydetailmainform.h"

BatteryDetailMainForm::BatteryDetailMainForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::BatteryDetailMainForm)
{
    ui->setupUi(this);
}

BatteryDetailMainForm::~BatteryDetailMainForm()
{
    delete ui;
}
