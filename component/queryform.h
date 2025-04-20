#ifndef QUERYFORM_H
#define QUERYFORM_H

#include <QWidget>
#include <QMouseEvent>
namespace Ui
{
    class QueryForm;
}

class QueryForm : public QWidget
{
    Q_OBJECT

public:
    explicit QueryForm(QWidget* parent = nullptr);
    ~QueryForm();



private slots:
    void initForm();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_btn_add_power_clicked();

    void on_btn_revise_power_clicked();

    void on_btn_delete_power_clicked();

    void on_btn_out_excel_clicked();

    void on_btn_import_clicked();

signals:
    void init_sql();

private:
    Ui::QueryForm* ui;
    QPoint clickPos;

protected:
    void mousePressEvent(QMouseEvent* e);
    void mouseMoveEvent(QMouseEvent* e);
};

#endif // QUERYFORM_H
