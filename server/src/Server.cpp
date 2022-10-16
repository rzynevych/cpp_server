#include "Server.hpp"

Server::Server(uint16_t port) : serverPort(port)
{
    fdCount.store(0);
    isOn.store(true);
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1)
    {
        std::cerr << "Can't create a socket! Quitting" << std::endl;
        exit(-1);
    }
    std::cout << "Listening socket created" << std::endl;
    // Bind the ip address and port to a socket
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(port);
    inet_pton(AF_INET, "0.0.0.0", &hint.sin_addr);
    std::cout << "Port: " << hint.sin_port << std::endl;
 
    int bres = bind(serverSocket, (sockaddr*)&hint, sizeof(hint));
    std::cout << "Binded socket: " << bres << std::endl;

    // Tell Winsock the socket is for listening
    listen(serverSocket, SOMAXCONN);
    std::cout << "Listening started" << std::endl;
}

Server::~Server()
{
    close(serverSocket);
}

void    Server::waitConnections()
{
    // Wait for a connection
    while (isOn.load())
    { 
        sockaddr_in client;
        socklen_t clientSize = sizeof(client);

        socket_t clientSocket = accept(serverSocket, (sockaddr*)&client, &clientSize);
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
        std::lock_guard<std::mutex> guard(mtx);
        allClients.emplace(clientSocket, Client(clientSocket, host, service));
        pollfd pfd;
        pfd.fd = clientSocket;
        pfd.events = POLLIN;
        pollfds.emplace_back(pfd);
        fdCount++;
    }
}

void    Server::pollSockets()
{
    while (isOn.load())
    {
        int size = fdCount.load();
        int poll_response = poll(pollfds.data(), size, POLL_TIMEOUT);
        if (poll_response < 0)
        {
            std::cerr << "Poll returned -1" << std::endl;
            exit(1);
        }
        if (poll_response == 0)
            continue ;
        for (int i = 0; i < size; ++i)
        {
            if (pollfds[i].revents & POLLIN)
            receiveData(allClients.at(pollfds[i].fd));
        }
    }
}

int     Server::receiveData(Client &client)
{
    memset(buff, 0, 4096);
    int count = recv(client.getSocket(), buff, 4096, 0);
    if (!client.isNamed())
    {
        std::string name(buff);
        client.setName(name);
        namedClients.emplace(name, &client);
        std::cout << "Client named" << std::endl;
    }
    else if (!client.isConnected())
    {
        auto it = namedClients.find(buff);
        if (it != namedClients.end())
        {
            client.connect(it->second);
            it->second->connect(&client);
            std::cout << "Clients connected" << std::endl;
        }
    }
    else
    {
        socket_t sock = client.getConnected()->getSocket();
        send(sock, buff, count, 0);
        std::cout << "Message sent" << std::endl;
    }
    return 0;
}