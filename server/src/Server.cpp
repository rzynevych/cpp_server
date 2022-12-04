#include "Server.hpp"

Server::Server(io_service &service, tcp::endpoint &tcpEndpoint) : 
        service(service), tcpEndpoint(tcpEndpoint),
        tcpAcceptor(service, tcpEndpoint), sock(service)
{
    tcpAcceptor.listen();
    auto newClient = allClients.emplace(std::make_shared<SClient>(service)).first.operator*();
    tcpAcceptor.async_accept(newClient->getSocket(), 
        boost::bind(&Server::acceptHandler, this, newClient, _1));
    std::cout << "Server started" << std::endl;
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
        std::cout << "nameLength: " << nameLength << std::endl;
        std::cout << "read_bytes: " << read_bytes << std::endl;
        std::string targetName(data + 1, nameLength);
        int messageLength = read_bytes - nameLength - 2;
        std::cout << "messageLength: " << messageLength << std::endl;
        std::string message(data + nameLength + 1, messageLength);
        auto it = namedClients.find(targetName);
        if (it == namedClients.end())
        {
            std::cout << "Target client doesn't exist" << std::endl;
        }
        else
        {
            outbuff.sputc(client->getName().length());
            outbuff.sputn(client->getName().c_str(), targetName.length());
            outbuff.sputn(message.c_str(), message.length() + 1);
            it->second->getSocket().async_write_some(outbuff.data(), boost::bind(&Server::onWrite, this, _1, _2));
        }
        client->getBuff().consume(client->getBuff().size());
        // now, wait for the next read from the same client
        async_read_until(client->getSocket(), client->getBuff(), 0, boost::bind(&Server::onRead, this, client, _1, _2));
    }
    else if (err == error::eof || err == error::connection_reset)
    {
        client->getSocket().shutdown(tcp::socket::shutdown_both);
        client->getSocket().close();
        namedClients.erase(client->getName());
        allClients.erase(client);
    }
    else
    {
        std::cout << "Error in onRead: " << err.message() << std::endl;
    }
}

void        Server::onWrite(const boost::system::error_code & err, size_t n) 
{
    std::cout << "Answer sent" << std::endl;
    outbuff.consume(outbuff.size());
}

void        Server::getClientName(const std::shared_ptr<SClient> &client,
                    const boost::system::error_code & err, size_t read_bytes)
{
    char *data = (char *) client->getBuff().data().data();
    int nameLength = data[0];
    std::cout << "nameLength: " << nameLength << std::endl;
    std::string clientName(data + 1, nameLength);
    client->setName(clientName);
    namedClients.emplace(clientName, client);
    client->getBuff().consume(client->getBuff().size());
    async_read_until(client->getSocket(), client->getBuff(), 0, 
                boost::bind(&Server::onRead, this, client, _1, _2));
}
