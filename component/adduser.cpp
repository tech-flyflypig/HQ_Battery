#include "adduser.h"
#include "ui_adduser.h"
#include <QtSql>
#include <QMessageBox>
#include <QCryptographicHash>
#include <QDateTime>

adduser::adduser(USER user, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::adduser),
    m_user(user)
{
    ui->setupUi(this);
    if(!m_user.user_name.isEmpty())
        m_type = 1;
    else
        m_type = 0;
    this->initform();
}

adduser::~adduser()
{
    delete ui;
}

void adduser::mousePressEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton)
        clickPos = e->pos();
}
void adduser::mouseMoveEvent(QMouseEvent *e)
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

    ui->cbx_privilege->clear();
    ui->cbx_privilege->addItem("普通用户", 0);
    ui->cbx_privilege->addItem("高级用户", 1);
    ui->cbx_privilege->addItem("管理员", 9);

    ui->cbx_status->clear();
    ui->cbx_status->addItem("禁用", 0);
    ui->cbx_status->addItem("正常", 1);
    ui->cbx_status->addItem("锁定", 2);

    ui->cbx_status->setCurrentIndex(1);

    if(m_type)
    {
        ui->led_user_name->setText(m_user.user_name);
        ui->led_user_name->setReadOnly(true);
        ui->led_password->setText("");
        ui->led_password->setPlaceholderText("不修改请留空");
        ui->cbx_privilege->setCurrentIndex(1 - m_user.privilege);

        QSqlQuery query;
        query.prepare("SELECT real_name, email, phone, status FROM power_user WHERE id = ?");
        query.addBindValue(m_user.rowid);

        if(query.exec() && query.next())
        {
            ui->led_real_name->setText(query.value(0).toString());
            ui->led_email->setText(query.value(1).toString());
            ui->led_phone->setText(query.value(2).toString());

            int statusIndex = ui->cbx_status->findData(query.value(3).toInt());
            if(statusIndex != -1)
            {
                ui->cbx_status->setCurrentIndex(statusIndex);
            }
        }
    }
}

bool adduser::validateForm()
{
    QString username = ui->led_user_name->text().trimmed();
    if(username.isEmpty())
    {
        QMessageBox::warning(this, "提示", "用户名不能为空");
        ui->led_user_name->setFocus();
        return false;
    }

    if(m_type == 0)
    {
        QString password = ui->led_password->text();
        if(password.isEmpty())
        {
            QMessageBox::warning(this, "提示", "密码不能为空");
            ui->led_password->setFocus();
            return false;
        }

        if(password.length() < 6)
        {
            QMessageBox::warning(this, "提示", "密码长度不能少于6个字符");
            ui->led_password->setFocus();
            return false;
        }
    }

    QString email = ui->led_email->text().trimmed();
    if(!email.isEmpty() && !email.contains("@"))
    {
        QMessageBox::warning(this, "提示", "邮箱格式不正确");
        ui->led_email->setFocus();
        return false;
    }

    return true;
}

void adduser::on_btn_sure_clicked()
{
    if(!validateForm())
    {
        return;
    }

    QString username = ui->led_user_name->text().trimmed();
    QString password = ui->led_password->text();
    QString realName = ui->led_real_name->text().trimmed();
    QString email = ui->led_email->text().trimmed();
    QString phone = ui->led_phone->text().trimmed();
    
    // 获取权限级别
    int privilege = ui->cbx_privilege->currentData().toInt();

    
    // 获取账户状态
    int status = ui->cbx_status->currentData().toInt();
    
    QSqlQuery query;
    
    // 修改用户
    if(m_type)
    {
        QString sql = "UPDATE power_user SET ";
        QStringList updateFields;

        if(!password.isEmpty())
        {
            QString passwordHash = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Md5).toHex();
            updateFields << "password_hash='" + passwordHash + "'";
        }

        updateFields << QString("privilege=%1").arg(privilege);
        updateFields << QString("real_name='%1'").arg(realName);
        updateFields << QString("email='%1'").arg(email);
        updateFields << QString("phone='%1'").arg(phone);
        updateFields << QString("status=%1").arg(status);
        updateFields << "update_time=CURRENT_TIMESTAMP";

        sql += updateFields.join(", ");
        sql += QString(" WHERE id=%1").arg(m_user.rowid);

        qDebug() << "执行SQL: " << sql;
        if(query.exec(sql))
        {
            QMessageBox::information(this, "成功", "用户信息已更新");
            emit sig_init();
            this->close();
        }
        else
        {
            QMessageBox::warning(this, "错误", "更新用户信息失败: " + query.lastError().text());
        }
    }
    // 添加新用户
    else
    {
        query.prepare("SELECT COUNT(*) FROM power_user WHERE user_name = ?");
        query.addBindValue(username);

        if(query.exec() && query.next())
        {
            int count = query.value(0).toInt();
            if(count > 0)
            {
                QMessageBox::warning(this, "错误", "用户名已存在，请换一个用户名");
                ui->led_user_name->setFocus();
                return;
            }
        }

        QString passwordHash = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Md5).toHex();

        query.prepare("INSERT INTO power_user "
                      "(user_name, password_hash, privilege, real_name, email, phone, status, create_time, update_time) "
                      "VALUES (?, ?, ?, ?, ?, ?, ?, CURRENT_TIMESTAMP, CURRENT_TIMESTAMP)");
        query.addBindValue(username);
        query.addBindValue(passwordHash);
        query.addBindValue(privilege);
        query.addBindValue(realName);
        query.addBindValue(email);
        query.addBindValue(phone);
        query.addBindValue(status);

        if(query.exec())
        {
            QMessageBox::information(this, "成功", "已成功添加用户");
            emit sig_init();
            this->close();
        }
        else
        {
            QMessageBox::warning(this, "错误", "添加用户失败: " + query.lastError().text());
        }
    }
}

void adduser::on_btn_close_clicked()
{
    this->close();
}

