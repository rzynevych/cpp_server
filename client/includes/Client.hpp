#ifndef CLIENTS_HPP
# define CLIENTS_HPP

# include "client_common.hpp"
# include "Writer.hpp"

# include <vector>
# include <sstream>
# include <atomic> 
# include <thread>

# include <sys/types.h>
# include <unistd.h>
# include <sys/socket.h>
# include <netdb.h>
# include <arpa/inet.h>
# include <string.h>
# include <string>

# include <poll.h>

#define BUFF_SIZE 4096

class Client
{
private:
    socket_t            sock;
    std::string         ipAddress;
    int                 port;
    std::string         name;
    std::string         targetName;
    std::string         input;
    std::atomic_bool    haveInput;
    std::atomic_bool    isOn;
    Writer              writer;
    char                buff[BUFF_SIZE];
    char                outbuff[BUFF_SIZE];

public:
    Client(std::string ipAddress, int port);
    ~Client() {}

    void    pollSocket();
    void    handleInput();
    void    handleData();
    void    handleCommand();
    void    waitInput();
    void    putMessage(std::string msg);

private:
    void    die(std::string message);


};

#endif