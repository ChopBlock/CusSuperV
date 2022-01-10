#ifndef SERIAL_H
#define SERIAL_H

#include <QObject>
#include<QIODevice>
#include<QSerialPort>
#include<QSerialPortInfo>
#include"glog/logging.h"
#include"../template/singleton.h"
#include<QThread>
#include<QDebug>
#include<QThread>
class serial :public QSerialPort{
    Q_OBJECT
public:
     std::atomic<bool> atomic_read=true;
    ~serial();
    serial();


private:

public Q_SLOTS:
 void Get_data();

};
class serialtest:public QObject,public  SingleTon<serialtest>

{Q_OBJECT
public:
    serialtest(const serialtest&)=delete;
    serialtest& operator=(const serialtest&)=delete;

    serialtest(token tk);
    ~serialtest();
 Q_SIGNALS:
    void serial_open();
private:
    serial serial;
    QThread td;
};
#endif // SERIAL_H
