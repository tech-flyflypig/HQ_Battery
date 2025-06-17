#ifndef LOGININFORM_H
#define LOGININFORM_H

#include <QWidget>
#include <QString>

namespace Ui {
class LoginInForm;
}

class LoginInForm : public QWidget
{
    Q_OBJECT

public:
    explicit LoginInForm(QWidget *parent = nullptr);
    ~LoginInForm();

protected:
    // 事件过滤器，用于处理焦点事件
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    Ui::LoginInForm *ui;
    QPoint clickPos;       // 用于实现窗口拖动
    QString m_normalStyle; // 存储普通状态的样式表
    QString m_focusStyle;  // 存储焦点状态的样式表
};

#endif // LOGININFORM_H
