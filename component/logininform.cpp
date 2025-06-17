#include "logininform.h"
#include "ui_logininform.h"
#include <QDebug>

LoginInForm::LoginInForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LoginInForm)
{
    ui->setupUi(this);

    // 设置窗体无边框，以及一些其他属性
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground);

    // 手动设置登录窗体的背景，防止 UI 文件中的样式设置不生效
    this->setStyleSheet("QWidget#LoginInForm{border-image: url(:/image/登录背景_01.png);}");
    ui->loginwd->setStyleSheet("QWidget#loginwd{border-image: url(:/image/登录窗口.png);}");

    // 安装事件过滤器
    ui->lineEdit_user->installEventFilter(this);
    ui->lineEdit_password->installEventFilter(this);

    // 设置输入框样式，使用更详细的样式设置，确保背景图片显示正确
    QString normalStyle = "QLineEdit {"
                          "  border-image: url(:/image/登录框_非选中.png);"
                          "  padding-left: 10px;" // 添加左侧内边距，让文字不会贴着边框
                          "  color: #333333;"     // 文字颜色
                          "  font: 12pt '微软雅黑';" // 设置字体
                          "}";

    QString focusStyle = "QLineEdit {"
                         "  border-image: url(:/image/登录框.png);"
                         "  padding-left: 10px;"
                         "  color: #333333;"
                         "  font: 12pt '微软雅黑';"
                         "}";

    // 保存样式表以便在事件过滤器中使用
    m_normalStyle = normalStyle;
    m_focusStyle = focusStyle;

    // 初始状态设置为非选中状态
    ui->lineEdit_user->setStyleSheet(normalStyle);
    ui->lineEdit_password->setStyleSheet(normalStyle);

    // 设置密码框回显方式为密码
    ui->lineEdit_password->setEchoMode(QLineEdit::Password);

    // 打印调试信息
    qDebug() << "登录窗口初始化完成，样式已设置";
}

LoginInForm::~LoginInForm()
{
    delete ui;
}

// 事件过滤器，处理焦点事件
bool LoginInForm::eventFilter(QObject *watched, QEvent *event)
{
    // 用户名输入框焦点事件
    if (watched == ui->lineEdit_user)
    {
        if (event->type() == QEvent::FocusIn)
        {
            ui->lineEdit_user->setStyleSheet(m_focusStyle);
        }
        else if (event->type() == QEvent::FocusOut)
        {
            ui->lineEdit_user->setStyleSheet(m_normalStyle);
        }
    }

    // 密码输入框焦点事件
    if (watched == ui->lineEdit_password)
    {
        if (event->type() == QEvent::FocusIn)
        {
            ui->lineEdit_password->setStyleSheet(m_focusStyle);
        }
        else if (event->type() == QEvent::FocusOut)
        {
            ui->lineEdit_password->setStyleSheet(m_normalStyle);
        }
    }

    // 继续传递事件
    return QWidget::eventFilter(watched, event);
}
