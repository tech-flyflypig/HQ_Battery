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
    // 更新表头，显示更多用户信息字段
    header << "序号" << "用户名" << "真实姓名" << "权限级别" << "状态" << "邮箱" << "电话" << "创建时间" << "最后登录时间";

    ui->tableWidget->setColumnCount(header.size());
    ui->tableWidget->setHorizontalHeaderLabels(header);

    // 设置基本的列宽比例
    int tableWidth = ui->tableWidget->width();
    int totalWidth = tableWidth - 20; // 减去一些边距

    // 设置各列的宽度比例
    ui->tableWidget->setColumnWidth(0, totalWidth * 0.05);  // 序号列 5%
    ui->tableWidget->setColumnWidth(1, totalWidth * 0.12);  // 用户名列 12%
    ui->tableWidget->setColumnWidth(2, totalWidth * 0.10);  // 真实姓名 10%
    ui->tableWidget->setColumnWidth(3, totalWidth * 0.08);  // 权限级别 8%
    ui->tableWidget->setColumnWidth(4, totalWidth * 0.08);  // 状态 8%
    ui->tableWidget->setColumnWidth(5, totalWidth * 0.14);  // 邮箱 14%
    ui->tableWidget->setColumnWidth(6, totalWidth * 0.12);  // 电话 12%
    ui->tableWidget->setColumnWidth(7, totalWidth * 0.15);  // 创建时间 15%
    ui->tableWidget->setColumnWidth(8, totalWidth * 0.15);  // 最后登录时间 15%

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

    ui->tableWidget->setColumnWidth(0, totalWidth * 0.05);  // 序号列
    ui->tableWidget->setColumnWidth(1, totalWidth * 0.12);  // 用户名列
    ui->tableWidget->setColumnWidth(2, totalWidth * 0.10);  // 真实姓名
    ui->tableWidget->setColumnWidth(3, totalWidth * 0.08);  // 权限级别
    ui->tableWidget->setColumnWidth(4, totalWidth * 0.08);  // 状态
    ui->tableWidget->setColumnWidth(5, totalWidth * 0.14);  // 邮箱
    ui->tableWidget->setColumnWidth(6, totalWidth * 0.12);  // 电话
    ui->tableWidget->setColumnWidth(7, totalWidth * 0.15);  // 创建时间
    // 最后一列会自动拉伸
}

void AddUserForm::on_pushButton_clicked()
{
    QSqlQuery query;
    QString sql = "SELECT id, user_name, real_name, privilege, status, email, phone, create_time, last_login_time FROM power_user";
    if(!ui->lineEdit->text().isEmpty())
        sql += QString(" WHERE user_name LIKE '%%1%' OR real_name LIKE '%%2%'").arg(ui->lineEdit->text()).arg(ui->lineEdit->text());
    sql += " ORDER BY id;";

    if (!query.exec(sql))
    {
        QMessageBox::warning(this, "错误", "查询失败: " + query.lastError().text());
        return;
    }

    ui->tableWidget->clearContents();

    // 获取记录数
    query.last();
    int nRow = query.at() + 1;
    query.first();

    ui->tableWidget->setRowCount(nRow);
    int nColumn = ui->tableWidget->columnCount();

    int j = 0;
    do
    {
        if (j >= nRow) break;

        // ID列
        QTableWidgetItem *idItem = new QTableWidgetItem(query.value(0).toString());
        idItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->tableWidget->setItem(j, 0, idItem);

        // 用户名列
        QTableWidgetItem *usernameItem = new QTableWidgetItem(query.value(1).toString());
        usernameItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        usernameItem->setToolTip(query.value(1).toString());
        ui->tableWidget->setItem(j, 1, usernameItem);

        // 真实姓名列
        QTableWidgetItem *realnameItem = new QTableWidgetItem(query.value(2).toString());
        realnameItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        realnameItem->setToolTip(query.value(2).toString());
        ui->tableWidget->setItem(j, 2, realnameItem);

        // 权限级别列
        QString privilegeText;
        int privilege = query.value(3).toInt();
        if (privilege == 9)
        {
            privilegeText = "管理员";
        }
        else if(privilege == 0)
        {
            privilegeText = "普通用户";
        }
        else
        {
            privilegeText = "高级用户";
        }
        QTableWidgetItem *privilegeItem = new QTableWidgetItem(privilegeText);
        privilegeItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->tableWidget->setItem(j, 3, privilegeItem);

        // 账户状态列
        QString statusText;
        int status = query.value(4).toInt();
        switch(status)
        {
            case 0:
                statusText = "禁用";
                break;
            case 1:
                statusText = "正常";
                break;
            case 2:
                statusText = "锁定";
                break;
            default:
                statusText = "未知";
        }
        QTableWidgetItem *statusItem = new QTableWidgetItem(statusText);
        statusItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->tableWidget->setItem(j, 4, statusItem);

        // 邮箱列
        QTableWidgetItem *emailItem = new QTableWidgetItem(query.value(5).toString());
        emailItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        emailItem->setToolTip(query.value(5).toString());
        ui->tableWidget->setItem(j, 5, emailItem);

        // 电话列
        QTableWidgetItem *phoneItem = new QTableWidgetItem(query.value(6).toString());
        phoneItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        phoneItem->setToolTip(query.value(6).toString());
        ui->tableWidget->setItem(j, 6, phoneItem);

        // 创建时间列
        QDateTime createTime = query.value(7).toDateTime();
        QString createTimeStr = createTime.toString("yyyy-MM-dd hh:mm:ss");
        QTableWidgetItem *createTimeItem = new QTableWidgetItem(createTimeStr);
        createTimeItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        createTimeItem->setToolTip(createTimeStr);
        ui->tableWidget->setItem(j, 7, createTimeItem);

        // 最后登录时间列
        QDateTime lastLoginTime = query.value(8).toDateTime();
        QString lastLoginTimeStr = lastLoginTime.isValid() ?
                                   lastLoginTime.toString("yyyy-MM-dd hh:mm:ss") :
                                   "尚未登录";
        QTableWidgetItem *lastLoginTimeItem = new QTableWidgetItem(lastLoginTimeStr);
        lastLoginTimeItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        lastLoginTimeItem->setToolTip(lastLoginTimeStr);
        ui->tableWidget->setItem(j, 8, lastLoginTimeItem);

        j++;
    }
    while(query.next());
}


void AddUserForm::on_btn_add_user_clicked()
{
    USER user;
    adduser *add = new adduser(user);
    connect(add, &adduser::sig_init, this, &AddUserForm::on_pushButton_clicked);
    add->show();
}


void AddUserForm::on_btn_revise_user_clicked()
{
    int row = ui->tableWidget->currentRow();
    if(row < 0)
    {
        QMessageBox::warning(this, "提示", "请先选择一个用户");
        return;
    }

    USER user;
    user.rowid = ui->tableWidget->item(row, 0)->text().toInt();
    user.user_name = ui->tableWidget->item(row, 1)->text();

    // 获取用户完整信息
    QSqlQuery query;
    query.prepare("SELECT privilege, real_name, email, phone, status FROM power_user WHERE id = ?");
    query.addBindValue(user.rowid);

    if (query.exec() && query.next())
    {
        user.privilege = query.value(0).toInt();
        user.real_name = query.value(1).toString();
        user.email = query.value(2).toString();
        user.phone = query.value(3).toString();
        user.status = query.value(4).toInt();
    }
    else
    {
        QMessageBox::warning(this, "错误", "获取用户信息失败: " + query.lastError().text());
        return;
    }

    adduser *add = new adduser(user);
    connect(add, &adduser::sig_init, this, &AddUserForm::on_pushButton_clicked);
    add->show();
}


void AddUserForm::on_btn_delete_user_clicked()
{
    int row = ui->tableWidget->currentRow();
    if(row < 0)
    {
        QMessageBox::warning(this, "提示", "请先选择一个用户");
        return;
    }

    QString username = ui->tableWidget->item(row, 1)->text();
    if(QMessageBox::question(nullptr, "请求", "确定要删除用户 '" + username + "' 吗？") == QMessageBox::Yes)
    {
        QSqlQuery query;
        query.prepare("DELETE FROM power_user WHERE id = ?");
        query.addBindValue(ui->tableWidget->item(row, 0)->text().toInt());

        if(query.exec())
        {
            QMessageBox::information(nullptr, "提示", "成功删除用户");
            this->on_pushButton_clicked();
        }
        else
        {
            QMessageBox::warning(nullptr, "提示", "删除失败: " + query.lastError().text());
        }
    }
}

