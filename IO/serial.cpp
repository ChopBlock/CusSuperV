#include "serial.h"
#include<QApplication>
serial::serial()
{
    QVector<QSerialPortInfo> serialInfos=  QSerialPortInfo::availablePorts();
    for(auto &com:serialInfos){
    LOG(INFO)<<com.portName().toStdString();
    }

    setPortName("COM3");
    setBaudRate(QSerialPort::Baud9600);
    setDataBits(QSerialPort::DataBits::Data8);
    setStopBits(QSerialPort::StopBits::OneStop);
    setParity(QSerialPort::Parity::NoParity);
    //qserial_.open(QSerialPort::ReadWrite);


   // connect(&qserial_,&QSerialPort::readyRead,this,&serial::DataReady,Qt::DirectConnection);
    connect(this,&QSerialPort::errorOccurred,this,[](int error){
        int erno=error;
        LOG(INFO)<<erno;
    });

}



serial::~serial()
{
    close();
}

void serial::Get_data()
{    while(atomic_read){

        if(!isOpen()){
            open(QSerialPort::ReadWrite);
            QThread::msleep(1000);
            continue;
        }


            // Do whatever with the array


     if (!waitForReadyRead(500))
     {

         continue;


      }
       QByteArray qbytes = readAll();
   QString hex=qbytes.toHex(' ');

    qDebug()<<hex<<QThread::currentThreadId();

    }
}


//void serial::DataReady()
//{   int len=qserial_.bytesAvailable();

//    LOG(INFO)<<len;
//    QByteArray bytes=qserial_.readAll();
//    QString hex=bytes.toHex(' ');
//  //  LOG(INFO)<<hex.toStdString();
//    qDebug()<<hex<<QThread::currentThreadId();
//    Q_EMIT   dataChanged(bytes);

//}


serialtest::serialtest(token tk)
{
    serial.moveToThread(&td);

    connect(this,&serialtest::serial_open,&serial,&serial::Get_data);
    td.start();
   }

serialtest::~serialtest()
   {serial.atomic_read=false;
    td.quit();
    td.wait();

    serial.close();
}
