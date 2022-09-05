#ifndef CLIENTS_CONNECTION_HPP
# define CLIENTS_CONNECTION_HPP

# include "server_common.hpp"
# include "Client.hpp"

class ClientsConnection
{
private:
    Client  first;
    Client  second;
public:
    ClientsConnection(Client &client1, Client &client2);
    ~ClientsConnection();
};

ClientsConnection::ClientsConnection(Client &client1, Client &client2) 
    : first(client1), second(client2)
{}

ClientsConnection::~ClientsConnection()
{}


#endif