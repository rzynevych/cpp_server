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
    bool        isNamed = false;
    bool        isConnected = false;

public:
    Client(socket_t sock, char *host, char *port);
    ~Client();
};

Client::Client(socket_t sock, char *host, char *port) 
    : sock(sock), host(host), port(port) {}
Client::~Client() {}



#endif