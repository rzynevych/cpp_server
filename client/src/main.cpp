#include "Client.hpp"

int main()
{
    Client *client = new Client("localhost", 8080);
    std::thread pollThread(&Client::pollSocket, client);
    std::thread inputThread(&Client::waitInput, client);
    pollThread.join();
    inputThread.join();
    delete client;
    return 0;
}