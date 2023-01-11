#include "Client.hpp"

int main()
{
    io_service service;
    tcp::endpoint ep{ip::address::from_string("127.0.0.1"), 8080};
    Client client(service, ep);
    service.run();
    return 0;
}