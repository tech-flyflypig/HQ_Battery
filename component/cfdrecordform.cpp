#include "cfdrecordform.h"
#include "ui_cfdrecordform.h"
#include <QtSql>
#include "QtXlsx/QtXlsx"
#include <QFileDialog>
#include <QMessageBox>
/*
 @ Name   : cfdrecordform.cpp
 @ Author : Wei.Liu
 @ brief  : 充放电记录查询界面
 @ Created: 2022-05-30
*/

CfdRecordForm::CfdRecordForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CfdRecordForm)
{
    ui->setupUi(this);
    this->initForm();
}

CfdRecordForm::~CfdRecordForm()
{
    delete ui;
}
void CfdRecordForm::mousePressEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton)
    {
        clickPos = e->pos();
    }
}
void CfdRecordForm::mouseMoveEvent(QMouseEvent *e)
{
    if(e->buttons()&Qt::LeftButton)
    {
        move(e->pos() + pos() - clickPos);
    }
}
void CfdRecordForm::initForm()
{
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setWindowTitle("充放电记录查询");
    QStringList header;
    header <<  "序号" << "设备ID" << "安装地点" << "设备操作"  << "开始时间" << "结束时间" << "时间间隔";

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

    ui->dtt_st->setDateTime(QDateTime::currentDateTime().addMonths(-1));
    ui->dtt_et->setDateTime(QDateTime::currentDateTime());

}
void CfdRecordForm::on_pushButton_2_clicked()
{
    this->close();
}

void CfdRecordForm::on_pushButton_clicked()
{
    QSqlQuery query;
    QString sql = QString( "select * from power_status_record where generate_time>='%1' and generate_time<'%2'").arg(ui->dtt_st->dateTime().toString("yyyy-MM-dd hh:mm:ss")).arg(ui->dtt_et->dateTime().toString("yyyy-MM-dd hh:mm:ss"));
    if(!ui->lineEdit->text().isEmpty())
    {
        sql += QString(" and power_id like '%1%'").arg(ui->lineEdit->text());
    }
    if(!ui->cbx_cd->isChecked())
    {
        sql += " and charge_status<>1";
    }
    if(!ui->cbx_fd->isChecked())
    {
        sql += " and charge_status<>0";
    }

    sql += " ORDER BY ID DESC LIMIT 100 OFFSET 0;";
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
            if(i == 3)
            {
                if(query.value(i).toInt() == 0)
                {
                    ui->tableWidget->setItem(j, i, new QTableWidgetItem("放电"));
                }
                else
                {
                    ui->tableWidget->setItem(j, i, new QTableWidgetItem("充电"));
                };
            }
            else if(i == 4 || i == 5)
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

/**
 * @brief  on_btn_out_excel_clicked
 * @author Wei.Liu
 * @date   2022-02-24
 * @script 列表导出
 */
void CfdRecordForm::on_btn_out_excel_clicked()
{
    QString filepath = QFileDialog::getSaveFileName(this, tr("保存充放电记录"), QString("充放电记录_%1~%2").arg(ui->dtt_st->dateTime().toString("yyyyMMdd-hhmmss")).arg(ui->dtt_et->dateTime().toString("yyyyMMdd-hhmmss")), tr(" (*.xlsx)"));
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
        if(!xlsx.selectSheet("充放电记录")) /*在当前打开的xlsx文件中，找一个名字为充放电记录的sheet*/
        {
            xlsx.addSheet("充放电记录");//找不到的话就添加一个名为充放电记录的sheet
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
                {
                    xlsx.write(i + 2, j + 1, ui->tableWidget->item(i, j)->text(), format2);
                }
            }
        }
        xlsx.saveAs(QDir::toNativeSeparators(filepath));/*保存*/
    }
}


void CfdRecordForm::on_btn_delete_clicked()
{
    QString sql = "delete from power_status_record;";
    QSqlQuery query;
    if(QMessageBox::information(nullptr, "警告", "充放电记录删除不可恢复,确认删除？", QMessageBox::Ok | QMessageBox::No) == QMessageBox::Ok)
    {
        if(query.exec(sql))
        {
            QMessageBox::information(nullptr, "提示", "充放电记录已删除成功", QMessageBox::Ok);
        }
        qInfo() << "主动将充放电记录删除";
    }
}

