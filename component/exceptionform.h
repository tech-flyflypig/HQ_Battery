#ifndef EXCEPTIONFORM_H
#define EXCEPTIONFORM_H

#include <QWidget>
#include <QMouseEvent>

/************************************
 * @ Name   : exceptionform.h
 * @ Author : Wei.Liu
 * @ Created: 2021-11-24
 ***********************************/

namespace Ui
{
    class ExceptionForm;
}

class ExceptionForm : public QWidget
{
    Q_OBJECT

public:
    explicit ExceptionForm(QWidget* parent = nullptr);
    ~ExceptionForm();

private slots:
    void initForm();

    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

    void on_btn_out_excel_clicked();

private:
    Ui::ExceptionForm* ui;
    QPoint clickPos;

protected:
    void mousePressEvent(QMouseEvent* e);
    void mouseMoveEvent(QMouseEvent* e);
};

#endif // EXCEPTIONFORM_H
