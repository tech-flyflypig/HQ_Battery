#include "deviceinfoform.h"
#include "ui_deviceinfoform.h"

#include <QPainter>
#include <QStyleOption>

DeviceInfoForm::DeviceInfoForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DeviceInfoForm)
{
    ui->setupUi(this);

    // 直接为每个Line设置样式
    // ui->line_7->setStyleSheet("background-color: rgb(23, 54, 133);");
    // ui->line_8->setStyleSheet("background-color: rgb(23, 54, 133);");
    // ui->line_16->setStyleSheet("background-color: rgb(23, 54, 133);");
    // ui->line_17->setStyleSheet("background-color: rgb(23, 54, 133);");
    // ui->line_18->setStyleSheet("background-color: rgb(23, 54, 133);");
    // ui->line_19->setStyleSheet("background-color: rgb(23, 54, 133);");
}

DeviceInfoForm::~DeviceInfoForm()
{
    delete ui;
}

void DeviceInfoForm::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
