#include <iostream>
#include "include/client.h"
#include "include/server.h"

int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        std::cerr << "Usage: ./program server | ./program client <server_ip>" << std::endl;
        return -1;
    }

    Server server;
    Client client;

    std::string mode = argv[1];

    if(mode == "server")
    {
        server.startServer();
    }
    else if( mode == "client" && argc == 3)
    {
        std::string server_ip = argv[2];
        client.startClient(server_ip);
    }
    else
    {
        std::cerr << "Invalid arguments." << std::endl;
    }
    
    return 0;
}