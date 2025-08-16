#ifndef EXCEPTIONRECORDFORM_H
#define EXCEPTIONRECORDFORM_H

#include <QWidget>

/************************************
 * @ Name   : exceptionrecordform.h
 * @ Author : Wei.Liu
 * @ Created: 2021-11-24
 ***********************************/

namespace Ui
{
    class ExceptionRecordForm;
}

class ExceptionRecordForm : public QWidget
{
    Q_OBJECT

public:
    explicit ExceptionRecordForm(QWidget *parent = nullptr);
    ~ExceptionRecordForm();

private slots:
    void initForm();

    void on_btn_search_clicked();

    void on_btn_out_excel_clicked();

    void on_btn_delete_clicked();

private:
    Ui::ExceptionRecordForm *ui;
};

#endif // EXCEPTIONRECORDFORM_H 