#ifndef CLIENTS_HPP
# define CLIENTS_HPP

# include "server_common.hpp"
# include <boost/asio.hpp>

using namespace boost::asio;
using namespace boost::asio::ip;

class SClient
{
private:
    tcp::socket     sock;
	streambuf       buff; // reads the answer from the client
    std::string     name;
    bool            removed = false;

public:
    SClient(io_service &s) 
    : sock(s), name(name) {}
    ~SClient() {}
    
    const std::string   &getName()
    {
        return name;
    }

    void            setName(const std::string &_name)
    {
        name = _name;
    }

    void            remove()
    {
        removed = true;
    }

    bool            isRemoved()
    {
        return removed;
    }

    streambuf       &getBuff()
    {
        return buff;
    }
    
    tcp::socket     &getSocket()
    {
        return sock;
    }
};

#endif