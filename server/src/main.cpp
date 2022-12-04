#include "Server.hpp"


int main()
{
    io_service service;
    tcp::endpoint ep{tcp::v4(), 8080};
    Server server(service, ep);
    service.run();
    // std::thread ioThread(
    //     static_cast<io_context::count_type (io_service::*)(void)>(&io_service::run),
    //     &service);
    // ioThread.join();
    return 0;
}