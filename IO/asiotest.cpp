#include "asiotest.h"
#include"glog/logging.h"
asiotest::asiotest(QObject *parent)
    : QObject{parent}
{
    try {

    serialport_=new boost::asio::serial_port(ioservice_,"COM3");
    serialport_->set_option(boost::asio::serial_port::baud_rate(9600));
    serialport_->set_option(boost::asio::serial_port::stop_bits(serial_port::stop_bits::one));
    serialport_->set_option(boost::asio::serial_port::character_size(8));
    serialport_->set_option(boost::asio::serial_port::parity(serial_port::parity::none));
    std::string msg= serialport_->is_open()?"true":"false";
    LOG(INFO)<<msg;
  //  Get_Data();

    }  catch (std::exception &ex) {


        LOG(ERROR)<<ex.what();

}

}

asiotest::~asiotest()
{
    atomic_read=false;

}

void asiotest::readhander(const boost::system::error_code &error,const std::size_t t)
{
    LOG(INFO)<<error.message()<<t;
     if(!error)
        {
        asyncgetdata();
        }
        else
        {
         LOG(INFO)<<error.message();
         serialport_->close();
        }
}

void asiotest::syncreadhander(const boost::system::error_code &error)
{
     LOG(INFO)<<error.message();
}

void asiotest::read_timeout()
{static int i=0;
    LOG(INFO)<<"read timeout: "<<i++;
}

void asiotest::SerialPort_OPen()
{

}


void asiotest::get_data()
{
    boost::thread th([&](){while(atomic_read){
    LOG(INFO)<<"GET_DATA";
    memset(&readbits_,0,sizeof(readbits_));
       serialport_->read_some(boost::asio::buffer(readbits_));

    //LOG(INFO)<<(boost::format("%x")%readbits_).str();

     //  LOG(INFO)<<fmt::format("{0:#X}",readbits_);
       QByteArray byte(readbits_);
       LOG(INFO)<<"GET_DATA_END\n"<<byte.toHex(' ').toStdString();
    }});
    th.detach();
}

void asiotest::asyncgetdata()
{
    LOG(INFO)<<"GET_DATA";
     serialport_->async_read_some(boost::asio::buffer(readbits_),boost::bind(&asiotest::readhander,this,boost::asio::placeholders::error,boost::asio::placeholders::bytes_transferred));

}

