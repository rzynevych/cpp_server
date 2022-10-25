#ifndef CLIENTS_HPP
# define CLIENTS_HPP

# include "server_common.hpp"

class SClient
{
private:
    socket_t    sock;
    std::string name;
    std::string host;
    std::string port;
    bool        removed = false;

public:
    SClient(socket_t sock, char *host, char *port, char *name) 
    : sock(sock), host(host), port(port), name(name) {}
    ~SClient() {}
    
    const std::string   &getName()
    {
        return name;
    }

    void        remove()
    {
        removed = true;
    }

    bool        isRemoved()
    {
        return removed;
    }
    
    socket_t    getSocket()
    {
        return sock;
    }
};

#endif