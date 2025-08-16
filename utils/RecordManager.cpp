#include "RecordManager.h"
#include "myapp.h"
#include <QDebug>
#include <QSqlError>

/*
 @ Name   : RecordManager.cpp
 @ Author : Wei.Liu
 @ brief  : 记录管理器单例类实现
 @ Created: 2022-05-30
*/

// 静态实例指针初始化
RecordManager* RecordManager::m_instance = nullptr;

RecordManager* RecordManager::instance()
{
    if (!m_instance)
    {
        m_instance = new RecordManager();
    }
    return m_instance;
}

RecordManager::RecordManager(QObject *parent)
    : QObject(parent)
{
    qDebug() << "RecordManager singleton created";
}

RecordManager::~RecordManager()
{
    qDebug() << "RecordManager singleton destroyed";
}

void RecordManager::exception_record(const QString &power_id, const QString &site, int type, const QString &exception_info)
{
    QSqlQuery query;
    
    // 使用参数化查询防止SQL注入
    QString checkSql = "SELECT COUNT(*) FROM power_exception_record WHERE power_id=? AND exception_type=? AND exception_info=? AND generate_time >?";
    query.prepare(checkSql);
    query.addBindValue(power_id);
    query.addBindValue(type);
    query.addBindValue(exception_info);
    query.addBindValue(QDateTime::currentDateTime().addSecs(-3600).toString("yyyy-MM-dd hh:mm:ss"));
    
    if(query.exec())
    {
        query.next();
        if(query.value(0).toInt() == 0)
        {
            // 没有重复记录，执行插入
            QString insertSql = "INSERT INTO power_exception_record (power_id, site, exception_type, exception_info, generate_time) VALUES (?, ?, ?, ?, ?)";
            query.prepare(insertSql);
            query.addBindValue(power_id);
            query.addBindValue(site);
            query.addBindValue(type);
            query.addBindValue(exception_info);
            query.addBindValue(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
            
            if(query.exec())
            {
                qDebug() << "异常记录插入成功:" << power_id << exception_info;
            }
            else
            {
                qDebug() << "异常记录插入失败:" << query.lastError().text();
            }
        }
        else
        {
            qDebug() << "异常记录已存在，跳过插入:" << power_id << exception_info;
        }
    }
    else
    {
        qDebug() << "查询异常记录失败:" << query.lastError().text();
    }


}

void RecordManager::cfd_record(const QString &power_id, const QString &site, int status, QDateTime start_time)
{
    QDateTime stop_time = QDateTime::currentDateTime();

    // 计算时间间隔
    int total_sec = start_time.secsTo(stop_time);
    int day = total_sec / (24 * 60 * 60);
    int hour = total_sec % (24 * 60 * 60) / 3600;
    int min = (total_sec % (24 * 60 * 60) % 3600) / 60;
    int sec = total_sec % (24 * 60 * 60) % 3600 % 60;
    QString interval = QString("%1天%2时%3分%4秒").arg(day).arg(hour).arg(min).arg(sec);
    
    QSqlQuery query;
    // 使用参数化查询防止SQL注入
    QString sql = "INSERT INTO power_status_record (power_id, site, charge_status, start_time, generate_time, interval) VALUES (?, ?, ?, ?, ?, ?)";
    query.prepare(sql);
    query.addBindValue(power_id);
    query.addBindValue(site);
    query.addBindValue(status);
    query.addBindValue(start_time.toString("yyyy-MM-dd hh:mm:ss"));
    query.addBindValue(stop_time.toString("yyyy-MM-dd hh:mm:ss"));
    query.addBindValue(interval);
    
    if(query.exec())
    {
        qDebug() << "充放电记录插入成功:" << power_id << "状态:" << status << "间隔:" << interval;
    }
    else
    {
        qDebug() << "充放电记录插入失败:" << query.lastError().text();
    }
} 