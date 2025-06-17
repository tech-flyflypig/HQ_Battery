#include "adduserform.h"
#include "ui_adduserform.h"
#include <QtSql>
#include "adduser.h"
#include "Struct.h"
#include <QMessageBox>
AddUserForm::AddUserForm(QWidget *parent) :
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

void AddUserForm::mousePressEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton)
        clickPos = e->pos();
}

void AddUserForm::mouseMoveEvent(QMouseEvent *e)
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
    
    // 设置基本的列宽比例
    int tableWidth = ui->tableWidget->width();
    int totalWidth = tableWidth - 20; // 减去一些边距
    
    // 设置各列的宽度比例：序号占5%，用户名和密码各占15%，用户等级占10%，两个时间列各占27.5%
    ui->tableWidget->setColumnWidth(0, totalWidth * 0.05); // 序号列
    ui->tableWidget->setColumnWidth(1, totalWidth * 0.15); // 用户名列
    ui->tableWidget->setColumnWidth(2, totalWidth * 0.15); // 密码列
    ui->tableWidget->setColumnWidth(3, totalWidth * 0.10); // 用户等级列
    ui->tableWidget->setColumnWidth(4, totalWidth * 0.275); // 创建时间列
    ui->tableWidget->setColumnWidth(5, totalWidth * 0.275); // 修改时间列
    
    // 对所有列应用交互式调整模式，允许用户调整
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    
    // 设置最后一列拉伸填充剩余空间
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
    
    ui->tableWidget->verticalHeader()->setHidden(true);
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    
    // 立即调整一次列宽
    adjustColumnWidths();
    
    this->on_pushButton_clicked();
}

// 添加窗口大小改变事件处理
void AddUserForm::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    adjustColumnWidths();
}

// 添加调整列宽的辅助方法
void AddUserForm::adjustColumnWidths()
{
    int tableWidth = ui->tableWidget->width();
    int totalWidth = tableWidth - 20; // 减去一些边距
    
    ui->tableWidget->setColumnWidth(0, totalWidth * 0.05);
    ui->tableWidget->setColumnWidth(1, totalWidth * 0.15);
    ui->tableWidget->setColumnWidth(2, totalWidth * 0.15);
    ui->tableWidget->setColumnWidth(3, totalWidth * 0.10);
    ui->tableWidget->setColumnWidth(4, totalWidth * 0.275);
    // 最后一列会自动拉伸
}

void AddUserForm::on_pushButton_clicked()
{
    QSqlQuery query;
    QString sql = QString( "select * from power_user");
    if(!ui->lineEdit->text().isEmpty())
        sql += QString(" where user_name like '%1%'").arg(ui->lineEdit->text());
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
            if(i == 4 || i == 5) // 时间列
            {
                QDateTime time = query.value(i).toDateTime();
                QString timeStr = time.toString("yyyy-MM-dd hh:mm:ss");
                QTableWidgetItem *item = new QTableWidgetItem(timeStr);
                // 添加tooltip，以便鼠标悬停时显示完整内容
                item->setToolTip(timeStr);
                ui->tableWidget->setItem(j, i, item);
            }
            else
            {
                QString cellText = query.value(i).toString();
                QTableWidgetItem *item = new QTableWidgetItem(cellText);
                // 为其他列也添加tooltip，以便内容过长时显示
                item->setToolTip(cellText);
                ui->tableWidget->setItem(j, i, item);
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
    adduser *add = new adduser(user);
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
    adduser *add = new adduser(user);
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

