#include "Client.hpp"

int main()
{
    io_service service;
    tcp::endpoint ep{tcp::v4(), 8080};
    Client client(service, ep);
    service.run();
    return 0;
}