#ifndef ADDUSERFORM_H
#define ADDUSERFORM_H

#include <QWidget>
#include <QMouseEvent>
#include <QResizeEvent>

namespace Ui
{
    class AddUserForm;
}

class USER;

class AddUserForm : public QWidget
{
    Q_OBJECT

public:
    explicit AddUserForm(QWidget* parent = nullptr);
    ~AddUserForm();

signals:
    void init_sql();

private slots:
    void on_pushButton_2_clicked();
    void on_pushButton_clicked();
    void on_btn_add_user_clicked();
    void on_btn_revise_user_clicked();
    void on_btn_delete_user_clicked();

protected:
    void mousePressEvent(QMouseEvent* e) override;
    void mouseMoveEvent(QMouseEvent* e) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    void initForm();
    void adjustColumnWidths();
    Ui::AddUserForm *ui;
    QPoint clickPos;
};

#endif // ADDUSERFORM_H
