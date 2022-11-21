#include "Server.hpp"

Server::Server(io_service &service, uint16_t port) : 
        service(service), server_port(port), 
        tcpEndpoint(tcp::v4(), 8080), tcpAcceptor(service, tcpEndpoint),
        sock(service)
{
    tcpAcceptor.listen();
    auto p = allClients.emplace(std::make_shared<SClient>(service));
    tcpAcceptor.async_accept(p.first->get()->getSocket(), 
        boost::bind(&Server::acceptHandler, this, *(p.first), _1));
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
        std::string answer{"success\n"};
        Server *p = this;
        // async_write(client->getSocket(), buffer(answer), on_write);
        async_read_until(client->getSocket(), client->getBuff(), '\n', 
                    boost::bind(&Server::onRead, this, client, _1, _2));
        tcpAcceptor.async_accept(client->getSocket(), boost::bind(&Server::acceptHandler, this, client, _1));
    }
    else if (ec == error::eof || ec == error::connection_reset)
    {

    } 
    else
    {

    }
}

void        Server::onRead(std::shared_ptr<SClient> client, 
                const boost::system::error_code & err, size_t read_bytes) 
{
	std::istream in(&client->getBuff());
	std::string msg;
	std::getline(in, msg);
	std::cout << msg << std::endl;
    auto f = &Server::onRead;
	client->getSocket().async_write_some(buffer("request_ok\n"), boost::bind(&Server::onWrite, this, _1, _2));
	// now, wait for the next read from the same client
	async_read_until(client->getSocket(), client->getBuff(), '\n', boost::bind(&Server::onRead, this, client, _1, _2));
}

void        Server::onWrite(const boost::system::error_code & err, size_t n) 
{
    std::cout << "Answer sent" << std::endl;
}

void    Server::abortClient(std::string message)
{
    std::cout << message << std::endl;
}

void     Server::removeDisconnectedClients()
{
   
}
