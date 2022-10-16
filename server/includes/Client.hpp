#ifndef CLIENTS_HPP
# define CLIENTS_HPP

# include "server_common.hpp"

class Client
{
private:
    std::string name;
    socket_t    sock;
    std::string host;
    std::string port;
    Client      *connectedClient = nullptr;
    bool        named = false;
    bool        removed = false;

public:
    Client(socket_t sock, char *host, char *port) 
    : sock(sock), host(host), port(port) {}
    ~Client() {}
    
    bool    isNamed()
    {
        return named;
    }
    void    setName(std::string name)
    {
        this->name = name;
        named = true;
    }
    bool    isConnected()
    {
        return connectedClient != nullptr;
    }
    void    connect(Client *client)
    {
        connectedClient = client;
    }
    Client  *getConnected()
    {
        return connectedClient;
    }
    
    socket_t    getSocket()
    {
        return sock;
    }
};

#endif