#include "Client.hpp"

Client::Client(io_service &service, tcp::endpoint &ep) : 
    service(service), ep(ep), sock(service), writer(std::cout)
{
    isOn.store(true);
    sock.connect(ep);
    writer.println("Connected to server");

    // Get name
    writer.println("Enter your name:");
    std::getline(std::cin, name);
    writer.println("Your name is: ");
    outbuff.sputc(name.length());
    outbuff.sputn(name.c_str(), name.length());
    sock.write_some(outbuff);
    std::thread inputThread(&Client::waitInput, this);
    inputThread.detach();
    async_read_until(sock, inbuff, 0, boost::bind(&Client::onRead, this, _1, _2));
}

void    Client::handleData()
{
    char *data;
    int nameLength = data[0];
    int messageLength = data[nameLength + 1];
    if (nameLength < 0)
    {
        writer.println("Received invalid data: name length < 0");
        return ;
    }
    std::string senderName(data + 1, nameLength);
    putMessage(senderName + ": \n" 
        + std::string(data + nameLength + 1, messageLength));
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
    outbuff.sputc(targetName.length());
    outbuff.sputn(targetName.c_str(), targetName.length());
    outbuff.sputn(input.c_str(), input.length() + 1);
    sock.async_write_some(outbuff, boost::bind(&Client::onWrite, this, _1, _2));
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
        handleInput();
    }
}

void    Client::die(std::string message)
{
    writer.println(message);
    exit(-1);
}

void    Client::putMessage(std::string msg)
{
    writer.eraseLast();
    writer.println(msg);
    writer.println("------------------------------");
    writer.printe(name + ": ");
}