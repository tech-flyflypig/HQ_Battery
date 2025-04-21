#include "bms1infoshowform.h"
#include "ui_bms1infoshowform.h"

BMS1InfoShowForm::BMS1InfoShowForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::BMS1InfoShowForm)
{
    ui->setupUi(this);
}

BMS1InfoShowForm::~BMS1InfoShowForm()
{
    delete ui;
}
