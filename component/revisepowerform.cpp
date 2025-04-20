#include "revisepowerform.h"
#include "ui_revisepowerform.h"
#include "Struct.h"
#include <QDebug>
#include <QSerialPortInfo>
#include <QtSql>
#include <QMessageBox>

RevisePowerForm::RevisePowerForm(const battery_info &batttery, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RevisePowerForm),
    m_battery(batttery)
{
    ui->setupUi(this);
    this->initForm();
}

RevisePowerForm::~RevisePowerForm()
{
    delete ui;
}

void RevisePowerForm::mousePressEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton)
    {
        clickPos = e->pos();
    }
}
void RevisePowerForm::mouseMoveEvent(QMouseEvent *e)
{
    if(e->buttons()&Qt::LeftButton)
    {
        move(e->pos() + pos() - clickPos);
    }
}

void RevisePowerForm::initForm()
{
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setWindowTitle("电源参数修改");
    QStringList m_serialPortName;
    foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {

        m_serialPortName << info.portName();
        //qDebug() << "serialPortName:" << info.portName();
    }
    ui->cbt_com->addItems(m_serialPortName);
    if(m_battery.type == "JHJ" )
    {
        ui->comboBox->setCurrentIndex(0);
    }
    else if(m_battery.type == "UPS1")
    {
        ui->comboBox->setCurrentIndex(1);
    }
    else if(m_battery.type == "UPS2")
    {
        ui->comboBox->setCurrentIndex(2);
    }
    ui->led_power_id->setText(m_battery.power_id);
    ui->led_site->setText(m_battery.site);
    ui->cbt_com->setCurrentText(m_battery.port_name);
    ui->cbt_baudrate->setCurrentText(QString::number(m_battery.baud_rate));
    ui->cbt_databits->setCurrentText(QString::number(m_battery.data_bits));
    ui->cbt_stopbits->setCurrentIndex(m_battery.stop_bits - 1);
    ui->cbt_parity->setCurrentIndex(m_battery.parity);
}

void RevisePowerForm::on_btn_close_clicked()
{
    this->close();
    delete this;
}


void RevisePowerForm::on_cbt_com_currentIndexChanged(const QString &arg1)
{
    QString sql = QString("select count(*) from power_source where port_name='%1' and power_id!='%2';").arg(arg1).arg(m_battery.power_id);
    QSqlQuery query;
    if(query.exec(sql))
    {
        while (query.next())
        {
            if(query.value(0).toInt() == 1)
            {
                ui->lab_com_alarm->setText("该串口已占用！");
            }
            else
            {
                ui->lab_com_alarm->clear();
            }
        }
    }
}


void RevisePowerForm::on_btn_sure_clicked()
{
    QString sql = "";
    QSqlQuery query;
    sql = QString("update power_source set  site='%1',port_name='%2', baud_rate= %3,data_bits=%4,")
          .arg(ui->led_site->text())
          .arg(ui->cbt_com->currentText()).arg(ui->cbt_baudrate->currentText().toUInt())
          .arg(ui->cbt_databits->currentText().toUInt());
    if(ui->cbt_stopbits->currentIndex() == 0)
    {
        sql += "stop_bits=1,";
    }
    else if(ui->cbt_stopbits->currentIndex() == 1)
    {
        sql += "stop_bits=2,";
    }
    else if(ui->cbt_stopbits->currentIndex() == 2)
    {
        sql += "stop_bits=3,";
    }
    if(ui->cbt_parity->currentIndex() == 0)
    {
        sql += "parity=0 ";
    }
    else if(ui->cbt_parity->currentIndex() == 1)
    {
        sql += "parity=1 ";
    }
    else if(ui->cbt_parity->currentIndex() == 2)
    {
        sql += "parity=2 ";
    }
    else if(ui->cbt_parity->currentIndex() == 3)
    {
        sql += "parity=3 ";
    }
    sql += QString("where power_id='%1';").arg(m_battery.power_id);
    qDebug() << sql;
    if(query.exec(sql))
    {
        QMessageBox box(QMessageBox::Information, QString("通知"), QString("保存成功！！"), QMessageBox::Ok);
        box.exec();
        emit sig_init_sql();
        this->close();
    }
    else
    {
        QMessageBox box(QMessageBox::Information, QString("通知"), QString("保存失败！！"), QMessageBox::Ok);
        box.exec();
    }
}

