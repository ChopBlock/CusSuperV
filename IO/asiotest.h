#ifndef ASIOTEST_H
#define ASIOTEST_H

#include <QObject>
#include"boost/asio.hpp"
#include"boost/bind.hpp"
#include<vector>
#include"boost/thread.hpp"
#include"boost/atomic.hpp"
#include"fmt/format.h"
#include<QByteArray>
#include"boost/format.hpp"

using namespace boost::asio;
using namespace boost::detail;
/**
 * @brief The asiotest class  test asio serialport async read compare wih Qt async read ( hard to implement)
 */
class asiotest : public QObject
{

    Q_OBJECT
public:
    explicit asiotest(QObject *parent = nullptr);
    ~asiotest();
     boost::atomic<bool> atomic_read=true;
         char readbits_[1024];
private:
    boost::asio::io_service ioservice_;
    boost::asio::serial_port *serialport_;
    void readhander(const boost::system::error_code &error,const std::size_t t);
    void syncreadhander(const boost::system::error_code &error);

    void      read_timeout();

public Q_SLOTS:
    void SerialPort_OPen();
    void get_data();
    void asyncgetdata();
};

#endif // ASIOTEST_H
