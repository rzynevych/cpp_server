#ifndef SERVER_HPP
# define SERVER_HPP

# include "server_common.hpp"
# include "Client.hpp"
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
    std::map<int, Client>               allClients;
    std::map<std::string, Client*>      namedClients;
    std::set<std::string>               clientNames;
    std::vector<pollfd>                 pollfds;
    uint16_t                            serverPort;
    socket_t                            serverSocket;
    std::mutex                          mtx;
    std::atomic_int                     fdCount;
    std::atomic_bool                    isOn;
    char                                buff[4096];

public:
    Server(uint16_t port);
    ~Server();

    void    waitConnections();
    void    pollSockets();
    int     removeClient(socket_t socket);
    int     receiveData(Client &client);
};



#endif