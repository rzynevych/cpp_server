#ifndef ROOM_HPP
# define ROOM_HPP

# include "server_common.hpp"
# include "Client.hpp"

# include <set>

class Room
{
private:
    std::set<Client*> clients;

public:
    Room()
    {}
    ~Room()
    {}
    void addClient(Client *client)
    {
        clients.emplace(client);
    }
    void removeClient(Client *client)
    {
        clients.erase(client);
    }
    std::set<Client*>::iterator begin()
    {
        return clients.begin();
    }
    std::set<Client*>::iterator end()
    {
        return clients.end();
    }
    
};

#endif