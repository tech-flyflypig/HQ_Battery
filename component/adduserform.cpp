#include "adduserform.h"
#include "ui_adduserform.h"
#include <QtSql>
#include "adduser.h"
#include "Struct.h"
#include <QMessageBox>
AddUserForm::AddUserForm(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::AddUserForm)
{
    ui->setupUi(this);
    this->initForm();
}

AddUserForm::~AddUserForm()
{
    delete ui;
}
void AddUserForm::mousePressEvent(QMouseEvent* e)
{
    if(e->button() == Qt::LeftButton)
        clickPos = e->pos();
}
void AddUserForm::mouseMoveEvent(QMouseEvent* e)
{
    if(e->buttons()&Qt::LeftButton)
        move(e->pos() + pos() - clickPos);
}


void AddUserForm::on_pushButton_2_clicked()
{
    this->close();
}

void AddUserForm::initForm()
{
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setWindowTitle("用户管理");
    QStringList header;
    header <<  "序号" << "用户名" << "密码"  << "用户等级" << "创建时间" << "修改时间";

    ui->tableWidget->setColumnCount(header.size());
    ui->tableWidget->setHorizontalHeaderLabels(header);
    ui->tableWidget->setColumnWidth(0, 50);
//    for (int i = 0; i < header.size(); i++)
//    {
//        ui->tableWidget->setColumnWidth(i, 170);
//    }
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
//    ui->tableWidget->setAlternatingRowColors(true);
    //ui->tableWidget->resizeColumnsToContents();
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);//根据容器调整大小
    ui->tableWidget->verticalHeader()->setHidden(true);
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->on_pushButton_clicked();
//    ui->dtt_st->setDateTime(QDateTime::currentDateTime().addMonths(-1));
//    ui->dtt_et->setDateTime(QDateTime::currentDateTime());

}

void AddUserForm::on_pushButton_clicked()
{
    QSqlQuery query;
    QString sql = QString( "select * from power_user");
    if(!ui->lineEdit->text().isEmpty())
        sql += QString(" where user_name like '%1%'").arg(ui->lineEdit->text());
//    if(!ui->cbx_cd->isChecked())
//        sql += " and charge_status!=1";
//    if(!ui->cbx_fd->isChecked())
//        sql += " and charge_status!=0";
    sql += ";";
    qDebug() << sql;
    query.exec(sql);
    ui->tableWidget->clearContents();
    int j = 0;
    query.last();
    int nRow = query.at() + 1;

    ui->tableWidget->setRowCount(nRow);
    int nColumn = ui->tableWidget->columnCount();

    query.first();

    while (j < nRow)
    {
        for (int i = 0; i < nColumn; i++)
        {
            if(i == 4 || i == 5)
            {
                QDateTime time = query.value(i).toDateTime();
                ui->tableWidget->setItem(j, i, new QTableWidgetItem(time.toString("yyyy-MM-dd hh:mm:ss")));
            }
            else
            {
                ui->tableWidget->setItem(j, i, new QTableWidgetItem(query.value(i).toString()));
            }

            ui->tableWidget->item(j, i)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        }
        j++;
        query.next();
    }
}


void AddUserForm::on_btn_add_user_clicked()
{
    USER  user;
    adduser* add = new adduser(user);
    connect(add, &adduser::sig_init, this, &AddUserForm::on_pushButton_clicked);
    add->show();
}


void AddUserForm::on_btn_revise_user_clicked()
{
    int row = ui->tableWidget->currentRow();
    if(row < 0)return;
    USER  user;
    user.rowid = ui->tableWidget->item(row, 0)->text().toInt();
    user.user_name = ui->tableWidget->item(row, 1)->text();
    user.password = ui->tableWidget->item(row, 2)->text();
    user.privilege = ui->tableWidget->item(row, 3)->text().toInt();
    adduser* add = new adduser(user);
    connect(add, &adduser::sig_init, this, &AddUserForm::on_pushButton_clicked);
    add->show();
}


void AddUserForm::on_btn_delete_user_clicked()
{
    int row = ui->tableWidget->currentRow();
    if(row < 0)return;
    if(QMessageBox::question(nullptr, "请求", "确定删除？") == QMessageBox::Yes)
    {
        QSqlQuery query;
        QString sql = QString( "delete from power_user where user_name='%1';").arg(ui->tableWidget->item(row, 1)->text());
        if(query.exec(sql))
        {
            QMessageBox::information(nullptr, "提示", "删除成功");
            this->on_pushButton_clicked();
        }
        else
        {
            QMessageBox::warning(nullptr, "提示", "删除失败，请重试！");
        }
    }
}

