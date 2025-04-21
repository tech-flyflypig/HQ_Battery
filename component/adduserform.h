#ifndef ADDUSERFORM_H
#define ADDUSERFORM_H

#include <QWidget>
#include <QMouseEvent>
namespace Ui
{
    class AddUserForm;
}

class AddUserForm : public QWidget
{
    Q_OBJECT

public:
    explicit AddUserForm(QWidget* parent = nullptr);
    ~AddUserForm();

private slots:
    void initForm();

    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

    void on_btn_add_user_clicked();

    void on_btn_revise_user_clicked();

    void on_btn_delete_user_clicked();

private:
    Ui::AddUserForm* ui;
    QPoint clickPos;

protected:
    void mousePressEvent(QMouseEvent* e);
    void mouseMoveEvent(QMouseEvent* e);

};

#endif // ADDUSERFORM_H
