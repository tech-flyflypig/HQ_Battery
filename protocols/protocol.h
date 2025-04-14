#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <QObject>

class Protocol : public QObject
{
    Q_OBJECT
public:
    explicit Protocol(QObject *parent = nullptr);
    virtual ~Protocol();


protected:
    virtual int read_data() = 0;

    virtual int send_data() = 0;

    virtual int  deal_data(const QByteArray &buffer) = 0;


signals:

private:

};

#endif // PROTOCOL_H
