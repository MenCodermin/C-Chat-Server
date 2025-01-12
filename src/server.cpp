#ifdef _WIN32
#include <winsock.h>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif

#include <iostream>
#include <thread>
#include <cstring>
#include <algorithm>
#include "include/server.h"

#define PORT 8000
#define MAX_CLIENTS 10


void Server::broadcastMessage(const std::string& message, int sender_fd)
{
    std::lock_guard<std::mutex> lock(client_mutex);
    for(int client : clients)
    {
        if(client != sender_fd)
        {
            send(client, message.c_str(), message.size(), 0);
        }
    }
}

void Server::handleClient(int client_socket)
{
    char buffer[1024];
    while(true)
    {
        memset(buffer, 0, sizeof(buffer));
        int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
        if(bytes_received <= 0)
        {
            std::lock_guard<std::mutex> lock(client_mutex);
            
            clients.erase(std::remove(clients.begin(),clients.end(),client_socket),clients.end());
#ifdef _WIN32
            closesocket(client_socket);
#else 
            close(client_socket);
#endif
            break;
        }
        std::string message(buffer);
        broadcastMessage(message, client_socket);
    }
}