#include "queryform.h"
#include "ui_queryform.h"
#include <QtSql>
#include "addpowerform.h"
#include "revisepowerform.h"
#include <QMessageBox>
#include "QtXlsx/QtXlsx"
#include <QFileDialog>
/*
 @ Name   : queryform.cpp
 @ Author : Wei.Liu
 @ brief  : 设备管理界面
 @ Created: 2022-05-30
*/
QueryForm::QueryForm(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::QueryForm)
{
    ui->setupUi(this);
    this->initForm();
}

QueryForm::~QueryForm()
{
    delete ui;
}

void QueryForm::mousePressEvent(QMouseEvent* e)
{
    if(e->button() == Qt::LeftButton)
        clickPos = e->pos();
}
void QueryForm::mouseMoveEvent(QMouseEvent* e)
{
    if(e->buttons()&Qt::LeftButton)
        move(e->pos() + pos() - clickPos);
}
void QueryForm::initForm()
{
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setWindowTitle("设备管理");
    QStringList header;
    header <<  "设备编号" << "设备类型" << "安装地点"  << "串口号" << "波特率"
           << "数据位" << "停止位" << "校验位" << "创建时间" << "修改时间";

    ui->tableWidget->setColumnCount(header.size());
    ui->tableWidget->setHorizontalHeaderLabels(header);
    ui->tableWidget->setColumnWidth(0, 50);
    for (int i = 0; i < header.size() - 2; i++)
    {
        ui->tableWidget->setColumnWidth(i, 100);
    }
    ui->tableWidget->setColumnWidth(8, 250);
    ui->tableWidget->setColumnWidth(9, 250);
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
//    ui->tableWidget->setAlternatingRowColors(true);
    //ui->tableWidget->resizeColumnsToContents();
    //ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);//根据容器调整大小
    ui->tableWidget->verticalHeader()->setHidden(true);
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->on_pushButton_clicked();
}

void QueryForm::on_pushButton_clicked()
{
    QSqlQuery query;
    QString sql = "select power_id,type,site,port_name,baud_rate,data_bits,stop_bits,parity,create_time,update_time from power_source";
    if(!ui->lineEdit->text().isEmpty())
        sql += QString(" where power_id like '%1%';").arg(ui->lineEdit->text());

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
            if(i == 8 || i == 9)
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


void QueryForm::on_pushButton_2_clicked()
{
    this->close();
}


void QueryForm::on_btn_add_power_clicked()
{
    AddPowerForm* add = new AddPowerForm();
    connect(add, &AddPowerForm::sig_init_sql, this, &QueryForm::init_sql);
    connect(add, &AddPowerForm::sig_init_sql, this, &QueryForm::on_pushButton_clicked);
    add->show();
}


void QueryForm::on_btn_revise_power_clicked()
{
    int row = ui->tableWidget->currentRow();
    if(row < 0)return;
    battery_info battery;
    battery.power_id = ui->tableWidget->item(row, 0)->text();
    battery.type = ui->tableWidget->item(row, 1)->text();
    battery.site = ui->tableWidget->item(row, 2)->text();
    battery.port_name = ui->tableWidget->item(row, 3)->text();
    battery.baud_rate = ui->tableWidget->item(row, 4)->text().toInt();
    battery.data_bits = ui->tableWidget->item(row, 5)->text().toInt();
    battery.stop_bits = ui->tableWidget->item(row, 6)->text().toInt();
    battery.parity = ui->tableWidget->item(row, 7)->text().toInt();
    RevisePowerForm* revise = new RevisePowerForm(battery);
    connect(revise, &RevisePowerForm::sig_init_sql, this, &QueryForm::init_sql);
    connect(revise, &RevisePowerForm::sig_init_sql, this, &QueryForm::on_pushButton_clicked);
    revise->show();
}


void QueryForm::on_btn_delete_power_clicked()
{
    int row = ui->tableWidget->currentRow();
    if(row < 0)return;
    QMessageBox box(QMessageBox::Question, QString("通知"), QString("确定删除？"), QMessageBox::Ok | QMessageBox::Cancel);
    if(box.exec() == QMessageBox::Ok)
    {
        QSqlQuery query;
        if(query.exec(QString("delete from power_source where power_id='%1'").arg(ui->tableWidget->item(row, 0)->text())))
        {
            QMessageBox box(QMessageBox::Information, QString("通知"), QString("删除成功"), QMessageBox::Ok);
            box.exec();
            emit init_sql();
            this->on_pushButton_clicked();
        }
        else
        {
            QMessageBox box(QMessageBox::Information, QString("通知"), QString("删除失败，请重试！"), QMessageBox::Ok);
            box.exec();
        }
    }
}


void QueryForm::on_btn_out_excel_clicked()
{
    QString filepath = QFileDialog::getSaveFileName(this, tr("保存设备列表"), QString("设备列表_%1").arg(QDateTime::currentDateTime().toString("yyyyMMdd-hhmmss")), tr(" (*.xlsx)"));
    if(!filepath.isEmpty())
    {
        QXlsx::Document xlsx;
        QXlsx::Format format1;/*设置该单元的样式*/
        format1.setFontColor(Qt::white);/*文字为红色*/
        format1.setPatternBackgroundColor(QColor(37, 89, 144)); /*背景颜色*/
        format1.setFontSize(12);/*设置字体大小*/
        format1.setHorizontalAlignment(QXlsx::Format::AlignHCenter);/*横向居中*/
        format1.setBorderStyle(QXlsx::Format::BorderDashDotDot);/*边框样式*/
        QXlsx::Format format2;
        format2.setFontSize(12);
        format2.setHorizontalAlignment(QXlsx::Format::AlignHCenter);
        format2.setFontColor(Qt::white);/*文字为红色*/
        format2.setPatternBackgroundColor(QColor(25, 32, 48)); /*背景颜色*/
        if(!xlsx.selectSheet("设备列表")) /*在当前打开的xlsx文件中，找一个名字为充放电记录的sheet*/
        {
            xlsx.addSheet("设备列表");//找不到的话就添加一个名为充放电记录的sheet
        }
        for (int i = 0; i < ui->tableWidget->horizontalHeader()->count(); ++i)
        {
            xlsx.write(1, i + 1, ui->tableWidget->horizontalHeaderItem(i)->text(), format1);
        }
        for (int i = 0; i < ui->tableWidget->rowCount(); ++i)
        {
            for (int j = 0; j < ui->tableWidget->columnCount(); ++j)
            {

                if(ui->tableWidget->item(i, j) != NULL)
                    xlsx.write(i + 2, j + 1, ui->tableWidget->item(i, j)->text(), format2);
            }
        }
        xlsx.saveAs(QDir::toNativeSeparators(filepath));/*保存*/
    }
}

/**
 * @brief  on_btn_import_clicked
 * @author Wei.Liu
 * @date   2022-02-24
 * @script 列表导入功能
 */
void QueryForm::on_btn_import_clicked()
{
    QString file_path = QFileDialog::getOpenFileName(this, tr("导入设备列表"), ".", tr(" (*.xlsx)"));
    if(file_path.isEmpty()) return;
    if(!file_path.contains(".xlsx"))
    {
        QMessageBox::warning(this, tr("警告"), tr("文件选取错误，请重新选择!"));
        return;
    }
    ui->tableWidget->clearContents();
    QXlsx::Document xlsx(file_path);
    QXlsx::Workbook* workBook = xlsx.workbook();
    QXlsx::Worksheet* workSheet = (QXlsx::Worksheet*)(workBook->sheet(0));//第一个标签页
    QSqlQuery query;
    //ui->tableWidget->setRowCount(workSheet->dimension().rowCount() - 1);
    //qInfo() << workSheet->dimension().rowCount();
    for (int i = 2; i <= workSheet->dimension().rowCount(); i++)
    {
        QStringList list_str;
        for (int j = 1; j <= workSheet->dimension().columnCount() - 2; j++)
        {
            QXlsx::Cell* cell = workSheet->cellAt(i, j);
            QString value = cell->value().toString();
            list_str.append(value);
//            if(!value.isEmpty())
//                ui->tableWidget->setItem(i - 2, j - 1, new QTableWidgetItem(value));
        }
        QString  sql = QString("insert into power_source (power_id,type,site,port_name,baud_rate,data_bits,stop_bits,parity) values('%1','%2','%3','%4',%5,%6,%7,%8);")
                       .arg(list_str.at(0)).arg(list_str.at(1)).arg(list_str.at(2)).arg(list_str.at(3)).arg(list_str.at(4))
                       .arg(list_str.at(5)).arg(list_str.at(6)).arg(list_str.at(7));
        if(!query.exec(sql))
        {
            QMessageBox box(QMessageBox::Information, QString("通知"), QString("导入失败！！"), QMessageBox::Ok);
            box.exec();
            return;
        }
        // qInfo() << sql;
    }
    QMessageBox box(QMessageBox::Information, QString("通知"), QString("导入成功！！"), QMessageBox::Ok);
    box.exec();
    this->on_pushButton_clicked();
    emit init_sql();
}

