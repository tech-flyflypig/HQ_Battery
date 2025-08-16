#ifndef RECORDMANAGER_H
#define RECORDMANAGER_H

#include <QObject>
#include <QSqlQuery>
#include <QDateTime>

/************************************
 * @ Name   : RecordManager.h
 * @ Author : Wei.Liu
 * @ Created: 2021-11-24
 ***********************************/

class RecordManager : public QObject
{
    Q_OBJECT

public:
    // 单例模式
    static RecordManager* instance();
    
    // 异常记录功能
    void exception_record(const QString &power_id, const QString &site, int type, const QString &exception_info);
    
    // 充放电记录功能
    void cfd_record(const QString &power_id, const QString &site, int status, QDateTime start_time);



private:
    // 私有构造函数，防止外部实例化
    explicit RecordManager(QObject *parent = nullptr);
    ~RecordManager();
    
    // 私有静态实例
    static RecordManager* m_instance;
    
    // 私有拷贝构造函数和赋值操作符，防止复制
    RecordManager(const RecordManager&) = delete;
    RecordManager& operator=(const RecordManager&) = delete;
};

#endif // RECORDMANAGER_H 