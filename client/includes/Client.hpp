#ifndef CLIENTS_HPP
# define CLIENTS_HPP

# include "client_common.hpp"
# include "Writer.hpp"

# include <vector>
# include <sstream>
# include <atomic> 
# include <thread>

# include <boost/asio.hpp>
# include <boost/bind.hpp>

using namespace boost::asio;
using namespace boost::asio::ip;

class Client
{
private:
    io_service          &service;
    tcp::endpoint       &ep;
    tcp::socket         sock;
	streambuf           inbuff;
    streambuf           outbuff;
    std::string         name;
    std::string         targetName;
    std::string         input;
    std::atomic_bool    haveInput;
    std::atomic_bool    isOn;
    Writer              writer;

public:
    Client(io_service &service, tcp::endpoint &ep);
    ~Client() {}

    void    onRead(const boost::system::error_code & err, size_t read_bytes);
    void    onWrite(const boost::system::error_code & err, size_t n);
    void    pollSocket();
    void    handleInput();
    void    handleData();
    void    handleCommand();
    void    waitInput();
    void    putMessage(std::string msg);

private:
    void    die(std::string message);


};

#endif