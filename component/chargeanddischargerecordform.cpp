#include "chargeanddischargerecordform.h"
#include "ui_chargeanddischargerecordform.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QSqlQuery>
#include "QtXlsx/QtXlsx"

ChargeAndDischargeRecordForm::ChargeAndDischargeRecordForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ChargeAndDischargeRecordForm)
{
    ui->setupUi(this);
}

ChargeAndDischargeRecordForm::~ChargeAndDischargeRecordForm()
{
    delete ui;
}

void ChargeAndDischargeRecordForm::on_btn_search_clicked()
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


void ChargeAndDischargeRecordForm::on_btn_out_excel_clicked()
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


void ChargeAndDischargeRecordForm::on_btn_delete_clicked()
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

