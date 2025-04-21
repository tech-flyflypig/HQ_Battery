#ifndef CFDRECORDFORM_H
#define CFDRECORDFORM_H

#include <QWidget>
#include <QMouseEvent>

/************************************
 * @ Name   : cfdrecordform.h
 * @ Author : Wei.Liu
 * @ Created: 2021-11-24
 ***********************************/

namespace Ui
{
    class CfdRecordForm;
}

class CfdRecordForm : public QWidget
{
    Q_OBJECT

public:
    explicit CfdRecordForm(QWidget *parent = nullptr);
    ~CfdRecordForm();

private slots:
    void initForm();

    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

    void on_btn_out_excel_clicked();

    void on_btn_delete_clicked();

private:
    Ui::CfdRecordForm *ui;
    QPoint clickPos;

protected:
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
};

#endif // CFDRECORDFORM_H
