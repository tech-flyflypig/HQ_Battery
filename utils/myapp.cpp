#include "myapp.h"
#include <QDebug>
#include <QFile>
#include <QApplication>
#include <QSqlQuery>
#include <QTimer>
QString myApp::AppPath = "";
QString myApp::HostName = "127.0.0.1";
int myApp::Port = 5432;
QString myApp::DatabaseName = "battery";
QString myApp::UserName = "postgres";
QString myApp::Password = "123";
QString myApp::BackGroundPath = "";


QList<battery_info> myApp::back_up_battery;



void myApp::init_param()
{
    if(AppPath.isEmpty())
    {
        AppPath = QApplication::applicationDirPath();
    }
    QString fileName = myApp::AppPath + "/config.ini";

    qInfo() << "myApp::init_param:" << fileName;

    // 如果配置文件不存在,则以初始值继续运行
    if (!QFile(fileName).exists())
    {
        WriteConfig();
        return;
    }
    qInfo() << "Read All Config Data!";
    ReadConfig();
}

// 读取配置文件
void myApp::ReadConfig()
{
    QString fileName = myApp::AppPath + "/config.ini";

    qInfo() << "ReadConfig:filename=" << fileName;

    // 如果配置文件不存在,则以初始值继续运行
    if (!QFile(fileName).exists())
    {
        return;
    }
    //TODO : 参数保存
    // QSettings *set = new QSettings(fileName, QSettings::IniFormat);
    // set->setIniCodec("UTF-8");
    // set->beginGroup("SYSTEM");
    // myApp::BackGroundPath = set->value("BackGroundPath").toString();
    // myApp::ModbusIp = set->value("ModbusIp").toString();
    // set->endGroup();
}

// 写入配置文件
void myApp::WriteConfig()
{
    QString fileName = myApp::AppPath + "/config.ini";

    // 如果配置文件不存在,则以初始值继续运行
    //    if(!myHelper::FileIsExist(fileName)) {
    //        return;
    //    }
    // QSettings *set = new QSettings(fileName, QSettings::IniFormat);
    // set->setIniCodec("UTF-8");
    // set->beginGroup("SYSTEM");
    // set->setValue("BackGroundPath", myApp::BackGroundPath);
    // set->setValue("ModbusIp", myApp::ModbusIp);
    // set->endGroup();
}

void myApp::SaveSql()
{
    foreach (battery_info bainfo, back_up_battery)
    {
        QString sql =  QString("update power_source set  x=%1,y=%2 where power_id='%3';").arg(bainfo.pos.x()).arg(bainfo.pos.y()).arg(bainfo.power_id);
        QSqlQuery query;
        if(query.exec(sql))
        {
            Sleep(100);
        }

    }
}

void myApp::Sleep(int msec)
{
    //非阻塞方式延时,现在很多人推荐的方法
    QEventLoop loop;
    QTimer::singleShot(msec, &loop, SLOT(quit()));
    loop.exec();
}
