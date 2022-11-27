#include "Server.hpp"

Server::Server(io_service &service, tcp::endpoint &tcpEndpoint) : 
        service(service), tcpEndpoint(tcpEndpoint),
        tcpAcceptor(service, tcpEndpoint), sock(service)
{
    tcpAcceptor.listen();
    auto newClient = allClients.emplace(std::make_shared<SClient>(service)).first.operator*();
    tcpAcceptor.async_accept(newClient->getSocket(), 
        boost::bind(&Server::acceptHandler, this, newClient, _1));
}

Server::~Server()
{
    sock.shutdown(tcp::socket::shutdown_both);
    sock.close();
}

void    Server::acceptHandler(std::shared_ptr<SClient> client, 
                            const boost::system::error_code &ec)
{
    if (!ec)
    {
        async_read_until(client->getSocket(), client->getBuff(), 0,
                    boost::bind(&Server::getClientName, this, client, _1, _2));
        auto newClient = allClients.emplace(std::make_shared<SClient>(service)).first.operator*();
        tcpAcceptor.async_accept(newClient->getSocket(), boost::bind(&Server::acceptHandler, this, newClient, _1));
    }
    else
    {

    }
}

void        Server::onRead(std::shared_ptr<SClient> client, 
                const boost::system::error_code & err, size_t read_bytes) 
{
    if (!err)
    {
        char *data = (char *) client->getBuff().data().data();
        int nameLength = data[0];
        std::string targetName(data + 1, nameLength);
        int messageLength = data[nameLength + 1];
        std::string message(data + nameLength + 2, messageLength);
        auto it = namedClients.find(targetName);
        if (it == namedClients.end())
        {
            std::cout << "Target client doesn't exist" << std::endl;
        }
        else
            it->second->getSocket().async_write_some(buffer(message), boost::bind(&Server::onWrite, this, _1, _2));
        // now, wait for the next read from the same client
        async_read_until(client->getSocket(), client->getBuff(), 0, boost::bind(&Server::onRead, this, client, _1, _2));
    }
    else if (err == error::eof || err == error::connection_reset)
    {
        namedClients.erase(client->getName());
        allClients.erase(client);
    }
    else
    {
        std::cout << err.message() << std::endl;
    }
}

void        Server::onWrite(const boost::system::error_code & err, size_t n) 
{
    std::cout << "Answer sent" << std::endl;
}

void        Server::getClientName(const std::shared_ptr<SClient> &client,
                    const boost::system::error_code & err, size_t read_bytes)
{
    char *data = (char *) client->getBuff().data().data();
    int nameLength = data[0];
    std::string clientName(data + 1, nameLength);
    client->setName(clientName);
    namedClients.emplace(clientName, client);
    client->getBuff().consume(read_bytes);
    async_read_until(client->getSocket(), client->getBuff(), 0, 
                boost::bind(&Server::onRead, this, client, _1, _2));
}

void    Server::abortClient(std::string message)
{
    std::cout << message << std::endl;
}

void     Server::removeDisconnectedClients()
{
   
}
