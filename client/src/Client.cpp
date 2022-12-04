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
    outbuff.sputc(name.length());
    outbuff.sputn(name.c_str(), name.length() + 1);
    sock.write_some(outbuff.data());
    outbuff.consume(outbuff.size());
    writer.println("Your name is: " + name);
    writer.printe(name + ": ");
    std::thread inputThread(&Client::waitInput, this);
    inputThread.detach();
    async_read_until(sock, inbuff, 0, boost::bind(&Client::onRead, this, _1, _2));
}

void    Client::onRead(const boost::system::error_code & err, size_t read_bytes)
{
    char *data = (char *) inbuff.data().data();
    int nameLength = data[0];
    std::string senderName(data + 1, nameLength);
    int messageLength = data[nameLength + 1];
    std::string message(data + nameLength + 2, messageLength);
    inbuff.consume(read_bytes);
    putMessage(senderName + ": \n" 
        + std::string(data + nameLength + 1, messageLength));
    if (isOn.load())
        async_read_until(sock, inbuff, 0, boost::bind(&Client::onRead, this, _1, _2));
}

void    Client::onWrite(const boost::system::error_code & err, size_t n)
{
    outbuff.consume(outbuff.size());
}

void    Client::handleInput()
{

    if (input[0] == '/')
    {
        handleCommand();
        return ;
    }
    if (targetName.length() == 0)
    {
        putMessage("Target client or room isn't set");
        return ;
    }
    outbuff.sputc(targetName.length());
    outbuff.sputn(targetName.c_str(), targetName.length());
    outbuff.sputn(input.c_str(), input.length());
    outbuff.sputc(0);
    sock.async_write_some(outbuff.data(), boost::bind(&Client::onWrite, this, _1, _2));
    writer.println("------------------------------");
    writer.printe(name + ": ");
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
    sock.shutdown(socket_base::shutdown_both);
    sock.close();
    service.stop();
    writer.println("Exiting...");
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