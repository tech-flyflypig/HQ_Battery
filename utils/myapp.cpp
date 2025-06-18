#include "myapp.h"
#include <QDebug>
#include <QFile>
#include <QApplication>
#include <QSqlQuery>
#include <QTimer>
#include <QDir>
#include <QMutex>
#include <iostream>
QString myApp::AppPath = "";
QString myApp::HostName = "127.0.0.1";
int myApp::Port = 5432;
QString myApp::DatabaseName = "battery";
QString myApp::UserName = "postgres";
QString myApp::Password = "123";
QString myApp::BackGroundPath = "";
int     myApp::LogFileCount = 30;       /* 默认30天 */
QString myApp::CurrentLogFileName = ""; /* 当前日志文件名 */

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

// 此函数里不能打印日志信息
void ManageLogFile()
{
    int i = 0;
    QString str = myApp::AppPath + "//log//";
    QDir    dir(str);
    if (dir.exists() == false) /* 不存在 */
    {
        dir.mkdir(str);
    }
    QString tmpstr;
    QStringList logfilelist;

    logfilelist.clear();
    foreach(QFileInfo mfi, dir.entryInfoList())
    {
        if (mfi.isFile())
        {
            tmpstr = mfi.fileName();

            if (tmpstr.count() > 4)
            {
                if (tmpstr.right(4) == ".txt")
                {
                    logfilelist.append(mfi.fileName());
                }
            }
        }
    }
    logfilelist.sort();

    if (logfilelist.count() > myApp::LogFileCount)
    {
        for (i = 0; i < (logfilelist.count() - myApp::LogFileCount); i++)
        {
            QFile::remove(str + "/" + logfilelist.at(i));
        }
    }
}

// 此函数里不能打印日志信息
void outputMessage(QtMsgType                 type,
                   const QMessageLogContext &context,
                   const QString            &msg)
{
    static QMutex mutex;

    mutex.lock();
    QString text;

    switch (type)
    {
        case QtDebugMsg:
            text = QString("Debug:");
            break;

        case QtWarningMsg:
            text = QString("Warning:");
            break;

        case QtCriticalMsg:
            text = QString("Critical:");
            break;

        case QtFatalMsg:
            text = QString("Fatal:");
    }
    QString context_info = QString("File:(%1) Line:(%2)").arg(QString(
                               context.file)).
                           arg(context.line);
    QString current_time = QTime::currentTime().toString("hh:mm:ss");

    // QString message = QString("[%1] %2 {%3
    // %4}").arg(current_time).arg(msg).arg(text).arg(context_info);
    QString message = QString("[%1] %2").arg(current_time).arg(msg);
    QString logfilename = QDate::currentDate().toString("yyyyMMdd") + ".txt";

    if (logfilename != myApp::CurrentLogFileName)
    {
        ManageLogFile();
        myApp::CurrentLogFileName = logfilename;
    }

    //    QString str = myApp::FILEPATH;
    //QString str = myApp::AppPath + "log/";
    QFile   file(myApp::AppPath + "//log//" + logfilename);

    file.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream text_stream(&file);
    text_stream << message << "\r\n";
    // 使用toLocal8Bit()将Unicode字符串转换为本地编码
    std::cout << message.toLocal8Bit().constData() << std::endl;
    file.flush();
    file.close();
    mutex.unlock();
}
