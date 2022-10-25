#ifndef SERVER_HPP
# define SERVER_HPP

# include "server_common.hpp"
# include "SClient.hpp"
# include <map>
# include <set>
# include <vector>
# include <thread>
# include <mutex>
# include <atomic>
# include <algorithm>

# include <sys/types.h>
# include <unistd.h>
# include <sys/socket.h>
# include <netdb.h>
# include <arpa/inet.h>
# include <string.h>
# include <string>
# include <poll.h>

# define BUFF_SIZE 4096

class Server
{
private:
    std::map<int, SClient>              allClients;
    std::map<std::string, SClient*>     namedClients;
    std::set<std::string>               clientNames;
    std::vector<pollfd>                 pollfds;
    uint16_t                            serverPort;
    socket_t                            serverSocket;
    std::mutex                          mtx;
    std::atomic_int                     fdCount;
    std::atomic_bool                    isOn;
    char                                buff[4096];
    char                                outbuff[4096];

public:
    Server(uint16_t port);
    ~Server();

    void    waitConnections();
    void    pollSockets();
    int     removeClient(socket_t socket);
    int     receiveData(SClient *client);
    void    removeDisconnectedClients();
};



#endif