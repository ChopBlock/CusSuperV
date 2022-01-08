#ifndef SERIAL_H
#define SERIAL_H

#include <QObject>
#include<QIODevice>
#include<QSerialPort>
#include<QSerialPortInfo>
#include"glog/logging.h"
#include"../template/singleton.h"
class serial : public QSerialPort,public SingleTon<serial>
{
    Q_OBJECT
public:
    serial(const serial&)=delete;
    serial& operate(const serial&)=delete;
    ~serial(){

        close();
    }
    serial(token tk);

public Q_SLOTS:
    void DataReady();
Q_SIGNALS:
    void dataChanged(QByteArray bytes);
};

#endif // SERIAL_H
