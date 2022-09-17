#ifndef SERVER_HPP
# define SERVER_HPP

# include "server_common.hpp"
# include "Client.hpp"
# include "ClientsConnection.hpp"
# include <map>
# include <set>
# include <vector>
# include <thread>
# include <mutex>
# include <atomic>

# include <sys/types.h>
# include <unistd.h>
# include <sys/socket.h>
# include <netdb.h>
# include <arpa/inet.h>
# include <string.h>
# include <string>
# include <poll.h>

class Server
{
private:
    std::map<int, Client>               singleClients;
    std::map<int, ClientsConnection>    clientConnections;
    std::set<std::string>               clientNames;
    std::vector<pollfd>                 pollfds;
    int                                 fdCount = 0;
    uint16_t                            port;
    socket_t                            sock;
    std::mutex                          mtx;
    std::atomic_bool                    is_on = true;

public:
    Server(uint16_t _port);
    ~Server();

    void    waitConnections();
    void    pollSockets();
    int     addClient(socket_t socket);
    int     removeClient(socket_t socket);
    int     nameClient(Client &client);
    int     connectClients(Client &client1, Client &client2);
};



#endif