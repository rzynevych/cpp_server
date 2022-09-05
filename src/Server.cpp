#include "Server.hpp"

Server::Server(uint16_t _port) : port(_port)
{
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        std::cerr << "Can't create a socket! Quitting" << std::endl;
        exit(-1);
    }
    std::cout << "Listening socket created" << std::endl;
    // Bind the ip address and port to a socket
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(54000);
    inet_pton(AF_INET, "0.0.0.0", &hint.sin_addr);
    std::cout << "Port: " << hint.sin_port << std::endl;
 
    int bres = bind(sock, (sockaddr*)&hint, sizeof(hint));
    std::cout << "Binded socket: " << bres << std::endl;

    // Tell Winsock the socket is for listening
    listen(sock, SOMAXCONN);
    std::cout << "Listening started" << std::endl;
}

Server::~Server()
{
    close(sock);
}

void    Server::acceptConnection()
{
    // Wait for a connection
    sockaddr_in client;
    socklen_t clientSize = sizeof(client);

    socket_t clientSocket = accept(sock, (sockaddr*)&client, &clientSize);
    std::cout << "Accepted connection" << std::endl;
 
    char host[NI_MAXHOST];      // Client's remote name
    char service[NI_MAXSERV];   // Service (i.e. port) the client is connect on
 
    memset(host, 0, NI_MAXHOST); // same as memset(host, 0, NI_MAXHOST);
    memset(service, 0, NI_MAXSERV);

    if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
    {
        std::cout << host << " connected on port " << service << std::endl;
    }
    else
    {
        inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
        sprintf(service, "%d", ntohs(client.sin_port));
        std::cout << host << " connected on port " << ntohs(client.sin_port) << std::endl;
    }
    singleClients.emplace(sock, Client(sock, host, service));
    pollfd pfd;
    pfd.events = POLLIN;
    pollfds.emplace_back(pfd);
    fdCount++;
}

void    Server::pollSockets()
{
    
}