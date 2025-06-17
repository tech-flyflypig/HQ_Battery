#include "mainwindow.h"
#include "utils/Struct.h"

#include <QApplication>
#include <QFile>
#include <QSqlDatabase>
#include <QDebug>
#include <QMessageBox>
#include "logininform.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // 注册BMS_1类型，使其可以在信号槽中使用
    qRegisterMetaType<BMS_1>("BMS_1");

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("battery.db");
    if(QFile::exists("battery.db"))
    {
        db.open();
        qInfo() << "connect db ok";
    }
    else
    {
        if(QMessageBox::critical(NULL, "警告", "数据库文件丢失，请联系管理员") == QMessageBox::Ok)
        {
            return -1;
        }
    }
    //MainWindow w;
    LoginInForm w;
    w.show();
    return a.exec();
}
