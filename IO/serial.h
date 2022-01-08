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
class serial :public QObject,public  SingleTon<serial>
{
    Q_OBJECT
public:
    serial(const serial&)=delete;
    serial& operate(const serial&)=delete;
    ~serial(){
        qserial_.close();
    }
    serial(token tk);
private:
    QThread td;
    QSerialPort qserial_;
public Q_SLOTS:
    void DataReady();
       Q_SIGNALS:
    void dataChanged(QByteArray bytes);
};

#endif // SERIAL_H
