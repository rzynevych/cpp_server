#include "Server.hpp"


int main()
{
    io_service service;
    Server server(service, 8080);
    std::thread ioThread(
        static_cast<io_context::count_type (io_service::*)(void)>(&io_service::run),
        &service);
    ioThread.join();
    return 0;
}