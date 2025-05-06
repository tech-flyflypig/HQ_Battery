#include "faultalarmform.h"
#include "ui_faultalarmform.h"

FaultAlarmForm::FaultAlarmForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::FaultAlarmForm)
{
    ui->setupUi(this);
}

FaultAlarmForm::~FaultAlarmForm()
{
    delete ui;
}
