#include "runningparamform.h"
#include "ui_runningparamform.h"

RunningParamForm::RunningParamForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::RunningParamForm)
{
    ui->setupUi(this);
}

RunningParamForm::~RunningParamForm()
{
    delete ui;
}
