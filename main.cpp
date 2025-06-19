#include "mainwindow.h"
#include "myapp.h"
#include "utils/Struct.h"
#include "component/logininform.h"

#include <QApplication>
#include <QFile>
#include <QSqlDatabase>
#include <QDebug>
#include <QMessageBox>
#include <QSqlError>
#include <QTextCodec>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // // 设置编码，解决中文乱码问题
    // #if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
    //     QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
    //     QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    //     QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
    // #else
    //     QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    //     QTextCodec::setCodecForLocale(codec);
    // #endif

    myApp::init_param();
    ManageLogFile();    //管理日志
    qInstallMessageHandler(outputMessage);    // 注册MessageHandler

    // 注册BMS_1类型，使其可以在信号槽中使用
    qRegisterMetaType<BMS_1>("BMS_1");

    // 连接数据库
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("battery.db");
    if(QFile::exists("battery.db"))
    {
        if(!db.open())
        {
            QMessageBox::critical(nullptr, "数据库错误", "无法打开数据库: " + db.lastError().text());
            return -1;
        }
        qInfo() << "数据库连接成功";
    }
    else
    {
        if(QMessageBox::critical(nullptr, "警告", "数据库文件丢失，请联系管理员") == QMessageBox::Ok)
        {
            return -1;
        }
    }

    // 创建主窗口但不显示
    MainWindow *mainWindow = new MainWindow();

    // 创建登录窗口
    LoginInForm *loginForm = new LoginInForm();

    // 连接登录成功信号到槽函数
    QObject::connect(loginForm, &LoginInForm::loginSuccess,
                     [mainWindow, loginForm](const QString & username, int privilege)
    {
        qDebug() << "用户" << username << "登录成功，权限级别:" << privilege;

        // 根据权限设置主窗口的权限
        mainWindow->setUserPrivilege(privilege);
        mainWindow->setCurrentUser(username);

        // 显示主窗口
        mainWindow->show();

        // 关闭登录窗口
        loginForm->close();
        loginForm->deleteLater();
    });

    // 显示登录窗口
    loginForm->show();

    return a.exec();
}
