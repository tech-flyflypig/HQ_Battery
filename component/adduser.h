#ifndef ADDUSER_H
#define ADDUSER_H

#include <QWidget>
#include "Struct.h"
#include <QMouseEvent>
namespace Ui
{
    class adduser;
}

class adduser : public QWidget
{
    Q_OBJECT

public:
    explicit adduser(USER user, QWidget* parent = nullptr);
    ~adduser();
private slots:
    void on_btn_sure_clicked();

    void on_btn_close_clicked();

private:
    void initform();

signals:
    void sig_init();
private:
    Ui::adduser* ui;
    USER m_user;
    int m_type;
    QPoint clickPos;

protected:
    void mousePressEvent(QMouseEvent* e);
    void mouseMoveEvent(QMouseEvent* e);
};

#endif // ADDUSER_H
