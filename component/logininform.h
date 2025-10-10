#ifndef LOGININFORM_H
#define LOGININFORM_H

#include <QWidget>
#include <QString>
#include <QKeyEvent>
/*
 * id：用户ID，主键，整型，自增  作为用户的唯一标识符
user_name：用户名，字符串，唯一约束 用于登录，需要唯一性
password_hash：密码哈希值，字符串  存储密码的MD5哈希值
privilege：用户权限级别，整型  控制用户在系统中的权限
last_login_time：最后登录时间，时间戳  记录用户上次登录的时间
create_time：账户创建时间，时间戳

update_time：账户信息更新时间，时间戳
status：账户状态，整型  0:禁用，1:正常，2:锁定等
login_attempts：登录尝试次数，整型 用于防止暴力破解
email：用户邮箱，字符串  用于找回密码等功能
phone：用户电话，字符串  可作为辅助验证方式
real_name：用户真实姓名，字符串
 */


namespace Ui
{
    class LoginInForm;
}

class LoginInForm : public QWidget
{
    Q_OBJECT

public:
    explicit LoginInForm(QWidget *parent = nullptr);
    ~LoginInForm();

signals:
    // 登录成功信号，携带用户名和权限级别
    void loginSuccess(const QString &username, int privilege = 0);

protected:
    // 事件过滤器，用于处理焦点事件
    bool eventFilter(QObject *watched, QEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    // 鼠标事件处理，实现窗口拖动
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private slots:
    // 登录按钮点击事件
    void onLoginButtonClicked();

    void on_btn_close_clicked();

private:
    // 保存记住密码设置
    void saveRememberPassword(const QString &username, const QString &password, bool remember);
    // 加载记住密码设置
    void loadRememberPassword();

    Ui::LoginInForm *ui;
    QPoint clickPos;       // 用于实现窗口拖动

    // 输入框样式
    QString m_normalStyle; // 非焦点状态样式
    QString m_focusStyle;  // 焦点状态样式
};

#endif // LOGININFORM_H
