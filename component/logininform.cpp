#include "logininform.h"
#include "ui_logininform.h"
#include <QDebug>
#include <QPaintEvent>
#include <QPainter>
#include <QStyleOption>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QPushButton>
#include <QPixmap>
#include <QSqlQuery>
#include <QSqlError>
#include <QCryptographicHash>
#include <QDateTime>

// 最大允许的登录失败次数
const int MAX_LOGIN_ATTEMPTS = 5;

LoginInForm::LoginInForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LoginInForm)
{
    ui->setupUi(this);

    // 设置窗体无边框，以及一些其他属性
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground);

    // 设置用户名输入框布局
    QHBoxLayout *userLayout = new QHBoxLayout(ui->lineEdit_user);
    userLayout->setSpacing(0);
    userLayout->setContentsMargins(5, 0, 0, 0);

    // 创建用户名图标按钮
    QPushButton *userIcon = new QPushButton(this);
    userIcon->setFixedSize(24, 24);
    userIcon->setFocusPolicy(Qt::NoFocus);
    userIcon->setFlat(true);
    userIcon->setStyleSheet("background: transparent; border: none;");
    QPixmap userPix(":/image/用户.png");
    userIcon->setIcon(userPix);
    userIcon->setIconSize(QSize(15, 15));

    // 添加图标到布局左侧
    userLayout->addWidget(userIcon);
    userLayout->addStretch();

    // 设置文本边距，使文本不会被图标遮挡
    ui->lineEdit_user->setTextMargins(30, 0, 0, 0);

    // 设置密码输入框布局
    QHBoxLayout *passLayout = new QHBoxLayout(ui->lineEdit_password);
    passLayout->setSpacing(0);
    passLayout->setContentsMargins(5, 0, 0, 0);

    // 创建密码图标按钮
    QPushButton *passIcon = new QPushButton(this);
    passIcon->setFixedSize(24, 24);
    passIcon->setFocusPolicy(Qt::NoFocus);
    passIcon->setFlat(true);
    passIcon->setStyleSheet("background: transparent; border: none;");
    QPixmap passPix(":/image/password.png");
    passIcon->setIcon(passPix);
    passIcon->setIconSize(QSize(20, 20));

    // 添加图标到布局左侧
    passLayout->addWidget(passIcon);
    passLayout->addStretch();

    // 设置文本边距，使文本不会被图标遮挡
    ui->lineEdit_password->setTextMargins(30, 0, 0, 0);

    // 设置输入框样式
    QString normalStyle = "QLineEdit{border-image: url(:/image/登录框_非选中.png);"
                          "color: #ffffff;"
                          "font: 12pt '微软雅黑';}";

    QString focusStyle = "QLineEdit{border-image: url(:/image/登录框.png);"
                         "color: #ffffff;"
                         "font: 12pt '微软雅黑';}";

    // 保存样式表以便在事件过滤器中使用
    m_normalStyle = normalStyle;
    m_focusStyle = focusStyle;

    // 初始状态设置为非选中状态
    ui->lineEdit_user->setStyleSheet(normalStyle);
    ui->lineEdit_password->setStyleSheet(normalStyle);

    // 设置密码框回显方式为密码
    ui->lineEdit_password->setEchoMode(QLineEdit::Password);

    // 安装事件过滤器
    ui->lineEdit_user->installEventFilter(this);
    ui->lineEdit_password->installEventFilter(this);

    // 连接登录按钮的点击信号
    connect(ui->pushButton, &QPushButton::clicked, this, &LoginInForm::onLoginButtonClicked);

    // 打印调试信息
    //qDebug() << "登录窗口初始化完成，样式已设置";

    // 设置回车键触发登录
    ui->lineEdit_user->setPlaceholderText("请输入用户名");
    ui->lineEdit_password->setPlaceholderText("请输入密码");
}

LoginInForm::~LoginInForm()
{
    delete ui;
}

void LoginInForm::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

// 登录按钮点击事件处理
void LoginInForm::onLoginButtonClicked()
{
    QString username = ui->lineEdit_user->text().trimmed();
    QString password = ui->lineEdit_password->text().trimmed();

    // 前端验证：检查输入项是否为空
    if (username.isEmpty())
    {
        QMessageBox::warning(this, "提示", "请输入用户名");
        ui->lineEdit_user->setFocus();
        return;
    }

    if (password.isEmpty())
    {
        QMessageBox::warning(this, "提示", "请输入密码");
        ui->lineEdit_password->setFocus();
        return;
    }

    // 查询数据库中的用户
    QSqlQuery query;
    query.prepare("SELECT id, password_hash, privilege, status, login_attempts, real_name FROM power_user WHERE user_name = ?");
    query.addBindValue(username);

    if (!query.exec())
    {
        QMessageBox::critical(this, "错误", "数据库查询失败：" + query.lastError().text());
        qDebug() << "SQL查询错误：" << query.lastError().text();
        return;
    }

    if (query.next())
    {
        int userId = query.value(0).toInt();
        QString passwordHash = query.value(1).toString();
        int userPrivilege = query.value(2).toInt();
        int userStatus = query.value(3).toInt();
        int loginAttempts = query.value(4).toInt();
        QString realName = query.value(5).toString();

        // 检查账户状态
        if (userStatus != 1)   // 1表示正常状态
        {
            QString statusMsg;
            switch (userStatus)
            {
                case 0:
                    statusMsg = "账户已禁用";
                    break;
                case 2:
                    statusMsg = "账户已锁定，请联系管理员";
                    break;
                default:
                    statusMsg = "账户状态异常，请联系管理员";
            }
            QMessageBox::warning(this, "登录失败", statusMsg);
            return;
        }

        // 检查是否超过最大尝试次数
        if (loginAttempts >= MAX_LOGIN_ATTEMPTS)
        {
            // 锁定账户
            QSqlQuery lockQuery;
            lockQuery.prepare("UPDATE power_user SET status = 2 WHERE id = ?");
            lockQuery.addBindValue(userId);
            lockQuery.exec();

            QMessageBox::warning(this, "账户锁定",
                                 "由于多次输入错误密码，您的账户已被锁定，请联系管理员解锁。");
            return;
        }

        // 计算输入密码的哈希值
        QString inputHash = QString(QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Md5).toHex());

        qDebug() << "用户名: " << username << ", 权限: " << userPrivilege;
        qDebug() << "用户真实姓名: " << (realName.isEmpty() ? "未设置" : realName);
        qDebug() << "尝试登录次数: " << loginAttempts;

        // 比较密码哈希值
        if (passwordHash == inputHash)
        {
            // 登录成功
            QString welcomeMsg = "欢迎 ";
            welcomeMsg += realName.isEmpty() ? username : realName;
            welcomeMsg += " 登录系统！";
            QMessageBox::information(this, "登录成功", welcomeMsg);

            // 更新登录信息
            QSqlQuery updateQuery;
            updateQuery.prepare("UPDATE power_user SET "
                                "last_login_time = CURRENT_TIMESTAMP, "
                                "login_attempts = 0, "
                                "update_time = CURRENT_TIMESTAMP "
                                "WHERE id = ?");
            updateQuery.addBindValue(userId);
            if (!updateQuery.exec())
            {
                qDebug() << "更新登录信息失败：" << updateQuery.lastError().text();
            }

            // 发送登录成功信号，并携带用户名和权限级别
            emit loginSuccess(username, userPrivilege);
            this->close();
        }
        else
        {
            // 密码错误，增加失败次数
            int newAttempts = loginAttempts + 1;
            QSqlQuery attemptQuery;
            attemptQuery.prepare("UPDATE power_user SET login_attempts = ? WHERE id = ?");
            attemptQuery.addBindValue(newAttempts);
            attemptQuery.addBindValue(userId);
            attemptQuery.exec();

            int remainingAttempts = MAX_LOGIN_ATTEMPTS - newAttempts;
            QString errorMsg;

            if (remainingAttempts > 0)
            {
                errorMsg = QString("密码错误，您还有 %1 次尝试机会。").arg(remainingAttempts);
            }
            else
            {
                errorMsg = "密码错误，您的账户将被锁定。";
            }

            QMessageBox::warning(this, "登录失败", errorMsg);
            ui->lineEdit_password->clear();
            ui->lineEdit_password->setFocus();
        }
    }
    else
    {
        QMessageBox::warning(this, "登录失败", "用户名不存在，请检查后重试！");
        ui->lineEdit_user->setFocus();
    }
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
        // 添加回车键处理
        else if (event->type() == QEvent::KeyPress)
        {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter)
            {
                // 如果用户名已填，则跳到密码框
                if (!ui->lineEdit_user->text().trimmed().isEmpty())
                {
                    ui->lineEdit_password->setFocus();
                    return true;
                }
            }
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
        // 添加回车键登录功能
        else if (event->type() == QEvent::KeyPress)
        {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter)
            {
                onLoginButtonClicked();
                return true;
            }
        }
    }

    // 继续传递事件
    return QWidget::eventFilter(watched, event);
}
