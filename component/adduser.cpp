#include "adduser.h"
#include "ui_adduser.h"
#include <QtSql>

adduser::adduser(USER user, QWidget* parent) :
    QWidget(parent),
    ui(new Ui::adduser),
    m_user(user)
{
    ui->setupUi(this);
    if(m_user.user_name != NULL)
        m_type = 1;
    else
        m_type = 0;
    this->initform();
}

adduser::~adduser()
{
    delete ui;
}

void adduser::mousePressEvent(QMouseEvent* e)
{
    if(e->button() == Qt::LeftButton)
        clickPos = e->pos();
}
void adduser::mouseMoveEvent(QMouseEvent* e)
{
    if(e->buttons()&Qt::LeftButton)
        move(e->pos() + pos() - clickPos);
}

void adduser::initform()
{
    this->setWindowFlags(Qt::FramelessWindowHint);
    if(m_type)
    {
        this->setWindowTitle("修改用户");
        ui->label_2->setText("修改用户");
    }
    else
    {
        this->setWindowTitle("添加用户") ;
        ui->label_2->setText("添加用户");
    }

    if(m_type)
    {
        ui->led_user_name->setText(m_user.user_name);
        ui->led_user_name->setReadOnly(true);
        ui->led_password->setText(m_user.password);
        ui->cbx_privilege->setCurrentIndex(1 - m_user.privilege);
    }

}

void adduser::on_btn_sure_clicked()
{
    QString sql;
    QSqlQuery query;
    if(m_type)
    {
        sql = QString("update power_user set password='%1',privilege=%2 where rowid=%3;").arg(ui->led_password->text())
              .arg(1 - ui->cbx_privilege->currentIndex()).arg(m_user.rowid);
    }
    else
    {
        sql = QString("insert into power_user (password,privilege,user_name) values('%1',%2,'%3');").arg(ui->led_password->text())
              .arg(1 - ui->cbx_privilege->currentIndex()).arg(ui->led_user_name->text());
    }
    qDebug() << sql;
    if( query.exec(sql))
    {
        emit sig_init();
        this->close();
    }
}


void adduser::on_btn_close_clicked()
{
    this->close();
}

