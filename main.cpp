#include "mainwindow.h"

#include <QApplication>
#include <QFile>
#include <QSqlDatabase>
#include <QDebug>
#include <QMessageBox>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
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
    MainWindow w;
    w.show();
    return a.exec();
}
