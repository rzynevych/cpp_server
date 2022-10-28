#include "Client.hpp"

Client::Client(std::string ipAddress, int port) : 
    ipAddress(ipAddress), port(port), writer(std::cout)
{
    isOn.store(true);
    haveInput.store(false);
    // Get name
    writer.println("Enter your name:");
    std::getline(std::cin, name);
    writer.println("Your name is: ");

    //	Create a socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        perror("Cannot create socket: ");
        std::cout << std::endl;
        exit(-1);
    }
    writer.println("Client socket created");

    //	Create a hint structure for the server we're connecting with
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(port);
    inet_pton(AF_INET, ipAddress.c_str(), &hint.sin_addr);

    //	Connect to the server on the socket
    int connectRes = connect(sock, (sockaddr*)&hint, sizeof(hint));
    if (connectRes == -1)
    {
        perror("Cannot connect to server: ");
        die("");
    }
    writer.println("Connected to server");
    int sendRes = send(sock, name.c_str(), name.length(), 0);
    if (sendRes == -1)
        die("Error: could not set name, exiting..");
    int bytesReceived = recv(sock, buff, BUFF_SIZE, 0);
    if (bytesReceived == -1)
        die("Cannot get response from server after seting name.");
    if (!strcmp("ok", buff))
        std::cout << "Name is set" << std::endl;
    else
        die("Unable connect to server, wrong responce: ");
    writer.printe(name + ": ");
}

void    Client::pollSocket()
{
    while (isOn.load())
    {
        pollfd pfd;
        pfd.fd = sock;
        pfd.events = POLLIN;
        int poll_response = poll(&pfd, 1, POLL_TIMEOUT);
        if (poll_response < 0)
        {
            std::cerr << "Poll returned -1" << std::endl;
            continue ;
        }
        if (pfd.revents & POLLIN)
            handleData();
        if (haveInput.load())
            handleInput();
    }
    close(sock);
}

void    Client::handleData()
{
    memset(buff, 0, BUFF_SIZE);
    int bytesReceived = recv(sock, buff, BUFF_SIZE, 0);
    if (bytesReceived == -1)
    {
        writer.println("There was an error getting response from server");
        return ;
    }
    int nameLength = buff[0];
    if (nameLength < 0)
    {
        writer.println("Received invalid data: name length < 0");
        return ;
    }
    std::string senderName(buff + 1, nameLength);
    putMessage(senderName + ": \n" 
        + std::string(buff + nameLength + 1, bytesReceived - nameLength - 1));
}

void    Client::handleInput()
{
    if (input[0] == '/')
    {
        handleCommand();
        haveInput.store(false);
        return ;
    }
    if (targetName.length() == 0)
    {
        putMessage("Target client or room isn't set");
        haveInput.store(false);
        return ;
    }
    outbuff[0] = targetName.length();
    std::copy(targetName.begin(), targetName.end(), outbuff + 1);
    std::copy(input.begin(), input.end(), outbuff + targetName.length() + 1);
    int sendRes = send(sock, outbuff, targetName.length() + input.length() + 1, 0);
    if (sendRes == -1)
    {
        putMessage("Could not send to server! Whoops!");
    }
    writer.println("------------------------------");
    writer.printe(name + ": ");
    haveInput.store(false);
}

void    Client::handleCommand()
{
    std::stringstream           inputStream(input);
    std::vector<std::string>    parts;
    std::string                 part;
    while (std::getline(inputStream, part, ' '))
    {
        parts.push_back(part);
    }
    if (parts.size() == 0)
        parts.push_back(input);

    if (parts[0].compare("/client") == 0 && parts.size() == 2)
    {
        if (parts[1].length() < 4)
        {
            putMessage("Client name too short");
            return ;
        }
        targetName = parts[1];
        putMessage("Target name changed to " + targetName);
        return ;
    }
    if (parts[0].compare("/exit") == 0)
    {
        isOn.store(false);
        writer.println("Exiting...");
        return ;
    }
    putMessage("Unknown command: " + input);
}


void    Client::waitInput()
{
    while(isOn.load())
    {
        std::string tmp;
        std::getline(std::cin, tmp);
        input = tmp;
        haveInput.store(true);
    }
}

void    Client::die(std::string message)
{
    writer.println(message);
    close(sock);
    exit(-1);
}

void    Client::putMessage(std::string msg)
{
    writer.eraseLast();
    writer.println(msg);
    writer.println("------------------------------");
    writer.printe(name + ": ");
}