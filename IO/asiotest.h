#ifndef ASIOTEST_H
#define ASIOTEST_H

#include <QObject>
#include"boost/asio.hpp"
#include"boost/bind.hpp"
#include<vector>
using boost::asio::serial_port;
using boost::asio::io_service;

/**
 * @brief The asiotest class  test asio serialport async read compare wih Qt async read ( hard to implement)
 */
class asiotest : public QObject
{

    Q_OBJECT
public:
    explicit asiotest(QObject *parent = nullptr);
private:
    boost::asio::io_service ioservice_;
    boost::asio::serial_port *serialport_;
static    void readhander(boost::system::system_error &error,std::size_t t);

public Q_SLOTS:
    void SerialPort_OPen();
    void Get_Data();
};

#endif // ASIOTEST_H
