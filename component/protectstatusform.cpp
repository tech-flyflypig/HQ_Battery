#include "protectstatusform.h"
#include "ui_protectstatusform.h"

ProtectStatusForm::ProtectStatusForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ProtectStatusForm)
{
    ui->setupUi(this);
}

ProtectStatusForm::~ProtectStatusForm()
{
    delete ui;
}
