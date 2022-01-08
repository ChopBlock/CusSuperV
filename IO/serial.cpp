#include "serial.h"

serial::serial(token tk)
{
    QVector<QSerialPortInfo> serialInfos=  QSerialPortInfo::availablePorts();
    for(auto &com:serialInfos){
    LOG(INFO)<<com.portName().toStdString();
    }

    qserial_.moveToThread(&td);

    qserial_.setPortName("COM3");
    qserial_.setBaudRate(QSerialPort::Baud19200);
    qserial_.setDataBits(QSerialPort::DataBits::Data8);
    qserial_.setStopBits(QSerialPort::StopBits::OneStop);
    qserial_.setParity(QSerialPort::Parity::NoParity);
    qserial_.open(QSerialPort::ReadWrite);

    connect(&qserial_,&QSerialPort::readyRead,this,&serial::DataReady,Qt::AutoConnection);
    connect(&qserial_,&QSerialPort::errorOccurred,this,[](int error){
        int erno=error;
        LOG(INFO)<<erno;
    });
      td.start();


}


void serial::DataReady()
{   int len=qserial_.bytesAvailable();

    LOG(INFO)<<len;
    QByteArray bytes=qserial_.readAll();
    QString hex=bytes.toHex(' ');
  //  LOG(INFO)<<hex.toStdString();
    qDebug()<<hex<<QThread::currentThreadId();
    Q_EMIT   dataChanged(bytes);

}

