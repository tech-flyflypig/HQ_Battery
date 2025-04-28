#include "batterylistform.h"
#include "ui_batterylistform.h"

BatteryListForm::BatteryListForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::BatteryListForm)
{
    ui->setupUi(this);
}

BatteryListForm::~BatteryListForm()
{
    delete ui;
}
