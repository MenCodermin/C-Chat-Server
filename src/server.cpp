#ifdef _WIN32
#include <winsock2.h>
#include <winsock.h>
#ifdef _MSC_VER
#pragma comment(lib, "ws2_32.lib")
#endif
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

#define PORT 8080
#define MAX_CLIENTS 10

#ifdef _WIN32
    typedef int socklen_t;
#endif

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

void Server::startServer()
{
#ifdef _WIN32
    WSADATA wsaData;
    if(WSAStartup(MAKEWORD(2,2), &wsaData) != 0)
    {
        std::cerr << "Failed to initialize Winsock" << std::endl;
        return;
    }
#endif 

    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(server_socket == -1)
    {
        std::cerr << "Failed to initialize socket." << std::endl;
        return;
    }

    sockaddr_in server_addr{};   
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if(bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
    {
        std::cerr << "Failed to bind socket" << std::endl;
        return;
    }

    if(listen(server_socket, MAX_CLIENTS) < 0)
    {
        std::cerr << "Failed to listen on socket." << std::endl;
        return;
    }

    std::cout << "Server is running on port " << PORT << std::endl;

    while(true)
    {
        sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_len);
        if(client_socket < 0)
        {
            std::cerr << "Failed to accept client." << std::endl;
            continue;
        }

        {
            std::lock_guard<std::mutex> lock(client_mutex);
            clients.push_back(client_socket);
        }

        std::thread([this, client_socket](){
          handleClient(client_socket);  
        }).detach();
    }
#ifdef _WIN32
    WSACleanup();
#endif
}   