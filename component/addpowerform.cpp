#include "addpowerform.h"
#include "ui_addpowerform.h"
#include <QtSql>
#include <QMouseEvent>
#include <QSerialPortInfo>
#include <QMessageBox>
AddPowerForm::AddPowerForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AddPowerForm)
{
    ui->setupUi(this);
    this->initForm();
}

AddPowerForm::~AddPowerForm()
{
    delete ui;
}
void AddPowerForm::initForm()
{
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setWindowTitle("设备添加");
    this->on_comboBox_currentIndexChanged(0);

    QStringList m_serialPortName;
    foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {

        m_serialPortName << info.portName();
        //qDebug() << "serialPortName:" << info.portName();
    }
    ui->cbt_com->addItems(m_serialPortName);
    ui->comboBox->setCurrentIndex(0);
}

void AddPowerForm::mousePressEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton)
        clickPos = e->pos();
}

void AddPowerForm::mouseMoveEvent(QMouseEvent *e)
{
    if(e->buttons()&Qt::LeftButton)
    {
        if((e->pos() + pos() - clickPos) != QPoint(0, 0))
        {
            this->move(e->pos() + pos() - clickPos);
        }
    }
}

void AddPowerForm::on_btn_close_clicked()
{
    this->close();
}


void AddPowerForm::on_comboBox_currentIndexChanged(int index)
{
    QString sql, power_id = "";
    QSqlQuery query;
    switch (index)
    {
        case 0:
            //sql = "select MAX(SPLIT_PART(power_id, '_', 2))  from power_source where power_id like 'JHJ%';"; //pgsql
            sql = "select MAX(cast(substr(power_id, 5) as int))  from power_source where power_id like 'BMS1%';"; //sqlite3
            if(query.exec(sql))
            {
                while (query.next())
                {
                    power_id = "BMS1" + QString("%1").arg(query.value(0).toInt() + 1, 3, 10, QChar('0'));
                }
            }
            break;
        case 1:
            //sql = "select MAX(SPLIT_PART(power_id, '_', 2))  from power_source where power_id like 'UPS1%';";
            sql = "select MAX(cast(substr(power_id, 6) as int))  from power_source where power_id like 'UPS1%';";
            if(query.exec(sql))
            {
                while (query.next())
                {
                    power_id = QString("UPS1_%1").arg(query.value(0).toInt() + 1);
                }
            }
            break;
        case 2:
            //sql = "select MAX(SPLIT_PART(power_id, '_', 2))  from power_source where power_id like 'UPS2%';";
            sql = "select MAX(cast(substr(power_id, 6) as int))  from power_source where power_id like 'UPS2%';";
            if(query.exec(sql))
            {
                while (query.next())
                {
                    power_id = QString("UPS2_%1").arg(query.value(0).toInt() + 1);
                }
            }
            break;
    }
    qDebug() << power_id;
    ui->led_power_id->setText(power_id);
}


void AddPowerForm::on_btn_sure_clicked()
{
    QString sql = "";
    QSqlQuery query;

    sql = QString("insert into power_source (power_id,site,port_name,baud_rate,data_bits,type,stop_bits,parity) values('%1','%2','%3',%4,%5,")
          .arg(ui->led_power_id->text()).arg(ui->led_site->text())
          .arg(ui->cbt_com->currentText()).arg(ui->cbt_baudrate->currentText().toUInt())
          .arg(ui->cbt_databits->currentText().toUInt());
    if(ui->comboBox->currentIndex() == 0)
    {
        sql += "'BMS_1',";
    }
    else if(ui->comboBox->currentIndex() == 1)
    {
        sql += "'UPS1',";
    }
    else
    {
        sql += "'UPS2',";
    }
    if(ui->cbt_stopbits->currentIndex() == 0)
    {
        sql += "1,";
    }
    else if(ui->cbt_stopbits->currentIndex() == 1)
    {
        sql += "2,";
    }
    else if(ui->cbt_stopbits->currentIndex() == 2)
    {
        sql += "3,";
    }
    if(ui->cbt_parity->currentIndex() == 0)
    {
        sql += "0);";
    }
    else if(ui->cbt_parity->currentIndex() == 1)
    {
        sql += "1);";
    }
    else if(ui->cbt_parity->currentIndex() == 2)
    {
        sql += "2);";
    }
    else if(ui->cbt_parity->currentIndex() == 3)
    {
        sql += "3);";
    }
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

void AddPowerForm::on_cbt_com_currentIndexChanged(const QString &arg1)
{
    QString sql = QString("select count(*) from power_source where port_name='%1';").arg(arg1);
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



