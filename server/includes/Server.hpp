#ifndef SERVER_HPP
# define SERVER_HPP

# include "server_common.hpp"
# include "SClient.hpp"

# include <boost/asio.hpp>
# include <boost/bind.hpp>

# include <string>
# include <iostream>
# include <memory>
# include <thread>
# include <map>
# include <set>
# include <vector>

using namespace boost::asio;
using namespace boost::asio::ip;

# define BUFF_SIZE 4096

class Server
{
private:
    io_service                                      &service;
    tcp::socket                                     sock;
    tcp::endpoint                                   tcpEndpoint;
    tcp::acceptor                                   tcpAcceptor;
    std::set<std::shared_ptr<SClient>>              allClients;
    std::map<std::string, std::shared_ptr<SClient>> namedClients;

public:
    Server(io_service &service, tcp::endpoint &tcpEndpoint);
    ~Server();

    void    acceptHandler(std::shared_ptr<SClient> client, 
                            const boost::system::error_code &ec);
    void    onRead(std::shared_ptr<SClient> client, 
                    const boost::system::error_code & err, size_t read_bytes); 
    void    onWrite(const boost::system::error_code & err, size_t n);
    void    getClientName(const std::shared_ptr<SClient> &client,
                    const boost::system::error_code & err, size_t read_bytes);
    int     remove_client();
    void    removeDisconnectedClients();

private:
    void    abortClient(std::string message);

};

#endif