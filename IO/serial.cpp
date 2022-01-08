#include "serial.h"

serial::serial(token tk)
{
    QVector<QSerialPortInfo> serialInfos=  QSerialPortInfo::availablePorts();
    for(auto &com:serialInfos){
    LOG(INFO)<<com.portName().toStdString();
    }
    setPortName("COM3");
    setBaudRate(Baud19200);
    setDataBits(DataBits::Data8);
    setStopBits(StopBits::OneStop);
    setParity(Parity::NoParity);
    open(ReadWrite);
    connect(this,&serial::readyRead,this,&serial::DataReady);
    connect(this,&serial::errorOccurred,this,[](int error){
        int erno=error;
        LOG(INFO)<<erno;
    });
}


void serial::DataReady()
{   int len=bytesAvailable();

    LOG(INFO)<<len;
    QByteArray bytes=readAll();
    QString hex=bytes.toHex(' ');
    LOG(INFO)<<hex.toStdString();
    Q_EMIT   dataChanged(bytes);

}

