#include "rightstatsform.h"
#include "ui_rightstatsform.h"

RightStatsForm::RightStatsForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::RightStatsForm)
{
    ui->setupUi(this);
}

RightStatsForm::~RightStatsForm()
{
    delete ui;
}
