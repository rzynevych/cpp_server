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
        memset(buff, 0, 4096);
        int bytesReceived = recv(clientSocket, buff, BUFF_SIZE, 0);
        std::cout << "Client name received" << std::endl;
        if (bytesReceived == -1)
        {
            abortClient(clientSocket, "Cannot get response from server to set client name.");
            continue ;
        }
        if (bytesReceived < 4)
        {
            abortClient(clientSocket, "Received client name is too short");
            continue ;
        }
        int sendRes = send(clientSocket, "ok", 2, 0);
        if (sendRes == -1)
        {
            abortClient(clientSocket, "Could not send responce to server after setting name");
            continue ;
        }
        std::cout << "Responce has been sent" << std::endl;
        {
            std::lock_guard<std::mutex> guard(mtx);
            allClients.emplace(clientSocket, SClient(clientSocket, host, service, buff));
            namedClients.emplace(std::string(buff), &(allClients.at(clientSocket)));
            pollfd pfd;
            pfd.fd = clientSocket;
            pfd.events = POLLIN;
            pollfds.emplace_back(pfd);
            fdCount++;
        }
    }
}

void    Server::abortClient(socket_t clientSocket, std::string message)
{
    close(clientSocket);
    std::cout << message << std::endl;
}

void    Server::pollSockets()
{
    while (isOn.load())
    {
        removeDisconnectedClients();
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
            {
                SClient *client;
                {
                    std::lock_guard<std::mutex> guard(mtx);
                    client = &(allClients.at(pollfds[i].fd));
                }
                receiveData(client);
            }
        }
    }
}

void     Server::removeDisconnectedClients()
{
    std::lock_guard<std::mutex> guard(mtx);
    std::set<int> rfds;  
    for (int i = 0; i < pollfds.size(); ++i)
    {
        if (allClients.at(pollfds[i].fd).isRemoved())
        {
            rfds.emplace(pollfds[i].fd);
            namedClients.erase(allClients.at(pollfds[i].fd).getName());
            allClients.erase(pollfds[i].fd);
        }
    }
    std::vector<pollfd>::iterator newEnd  = std::remove_if(pollfds.begin(), pollfds.end(), 
        [rfds](pollfd pfd){ return rfds.find(pfd.fd) != rfds.end();});
    pollfds.erase(newEnd, pollfds.end());
    fdCount.store(pollfds.size());
}


int     Server::receiveData(SClient *client)
{
    memset(buff, 0, BUFF_SIZE);
    int count = recv(client->getSocket(), buff, 4096, 0);
    int targNameLength = buff[0];
    if (count == 0)
    {
        close(client->getSocket());
        client->remove();
        return 0;
    }

    if (targNameLength < 0)
    {
        std::cout << "Received invalid data: name length < 0" << std::endl;
        return -1;
    }
    std::string targetName(buff + 1, buff[0]);
    socket_t targetSock;
    {
        std::lock_guard<std::mutex> guard(mtx);
        auto it = namedClients.find(targetName);
        if (it == namedClients.end())
        {
            std::cout << "Target client doesn't exist" << std::endl;
            return -1;
        }
        targetSock = it->second->getSocket();
    }
    memset(outbuff, 0, BUFF_SIZE);
    outbuff[0] = client->getName().length();
    const char *clientName = client->getName().c_str();
    int nameLength = client->getName().length();
    std::copy(clientName, clientName + nameLength, outbuff + 1);
    std::copy(buff + targNameLength + 1, buff + count, outbuff + nameLength + 1);
    send(targetSock, outbuff , count - targNameLength + nameLength, 0);
    std::cout << "Message sent" << std::endl;
    return 0;
}