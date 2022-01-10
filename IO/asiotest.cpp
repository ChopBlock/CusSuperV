#include "asiotest.h"
#include"glog/logging.h"
asiotest::asiotest(QObject *parent)
    : QObject{parent}
{try {

    serialport_=new boost::asio::serial_port(ioservice_,"COM3");
    serialport_->set_option(boost::asio::serial_port::baud_rate(9600));
    serialport_->set_option(boost::asio::serial_port::stop_bits(serial_port::stop_bits::one));
    serialport_->set_option(boost::asio::serial_port::character_size(8));
    serialport_->set_option(boost::asio::serial_port::parity(serial_port::parity::none));


    }  catch (std::exception &ex) {


        LOG(ERROR)<<ex.what();

}

}

void asiotest::readhander(boost::system::system_error &error, size_t t)
{
    LOG(INFO)<<error.what()<<t;
}

void asiotest::SerialPort_OPen()
{

}
void asiotest::Get_Data()
{
    while(true){
        std::vector<char> vec;

        serialport_->async_read_some(vec,readhander);
        boost::asio::deadline_timer timer(ioservice_);
        timer.expires_from_now( boost::posix_time::millisec (100 ));

    }
}

