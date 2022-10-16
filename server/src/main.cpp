#include "Server.hpp"

int main()
{
    Server server(8080);
    std::thread waitThread(&Server::waitConnections, &server);
    std::thread pollThread(&Server::pollSockets, &server);
    waitThread.join();
    pollThread.join();
    return 0;
}