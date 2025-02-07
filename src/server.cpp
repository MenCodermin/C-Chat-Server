#ifdef _WIN32
#include <winsock.h>
#include <winsock2.h>
#ifdef _MSC_VER
#pragma comment(lib, "ws2_32.lib")
#endif
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

#include <algorithm>
#include <cstring>
#include <iostream>
#include <thread>

#include "include/db.h"
#include "include/server.h"

#define PORT 8080
#define MAX_CLIENTS 10

#ifdef _WIN32
typedef int socklen_t;
#endif

PGconn* dbConn;


bool Server::authenticateClient(int client_socket)
{
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));
    recv(client_socket, buffer, sizeof(buffer),0);
    std::string authRequest(buffer);

    size_t firstColon = authRequest.find(":");
    size_t secondColon = authRequest.find(":",firstColon + 1);

    if(firstColon == std::string::npos || secondColon == std::string::npos)
    {
        std::cerr << "Invalid authentication request format" << std::endl;
        return false;
    }

    std::string authType = authRequest.substr(0,firstColon);
    std::string username = authRequest.substr(firstColon + 1, secondColon - firstColon - 1);
    std::string password = authRequest.substr(secondColon + 1);

    bool authSuccess = false;

    if(authType == "signup")
    {
        std::string result = registerUser(dbConn, username, password);
        authSuccess = (result.find("successfull") != std::string::npos);
    }
    else if(authType == "signin")
    {
        std::string result = loginUser(dbConn,username,password);
        authSuccess = (result.find("successfull") != std::string::npos);
    }

    std::string response = authSuccess ? "AUTH_SUCCESS" : "AUTH_FAILED";
    send(client_socket, response.c_str(), response.size(), 0);
    return authSuccess;
}

void Server::processClientMessages(int client_socket)
{
    char buffer[1024];

    while(true)
    {
        memset(buffer, 0, sizeof(buffer));
        int bytes_received = recv(client_socket,buffer,sizeof(buffer),0);
        if(bytes_received <= 0)
        {
            std::lock_guard<std::mutex> lock(client_mutex);
            clients.erase(std::remove(clients.begin(),clients.end(), client_socket),clients.end());
#ifdef _WIN32
            closesocket(client_socket);
#else
            close(client_socket);
#endif
            break;
        }

        std::string message(buffer);
        std::cout << "Received:" << message << std::endl;

        size_t firstColon = message.find(":");
        size_t secondColon = message.find(":",firstColon + 1);

        if(firstColon == std::string::npos || secondColon == std::string::npos)
        {
            std::cerr << "Error: Message format incorrect. Expected 'sender_id:receiver_id:message'" << std::endl;
            return;
        }

        try
        {
            int senderId = std::stoi(message.substr(0,firstColon));
            int receiverId = std::stoi(message.substr(firstColon + 1,secondColon - firstColon - 1));
            std::string chatMessage = message.substr(secondColon + 1);

            std::cout << "Sender: " << senderId << ", Receiver: " << receiverId << ", Message: " << chatMessage << std::endl;

            if(!chatMessage.empty())
            {
                saveMessage(dbConn, senderId, receiverId, chatMessage);
                broadcastMessage(message, client_socket);
            }
            else
            {
                std::cerr << "Error: Empty message received!\n";

            }
        }
        catch(const std::exception& e)
        {
            std::cerr << "Exception while parsing message: " << e.what() << std::endl;
        }
    }
}

int Server::setupServerSocket() {
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        std::cerr << "Failed to create socket.\n";
        return -1;
    }

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(8080);

    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Failed to bind socket.\n";
        return -1;
    }

    if (listen(server_socket, 10) < 0) {
        std::cerr << "Failed to listen on socket.\n";
        return -1;
    }

    std::cout << "Server is running and listening on port 8080...\n";
    return server_socket;
}

void Server::acceptClients(int server_socket)
{
    while(true)
    {
        sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_len);
        if(client_socket < 0)
        {
            std::cerr << "Failed to accept client.\n";
            continue;
        }

        {
            std::lock_guard<std::mutex> lock(client_mutex);
            clients.push_back(client_socket);
        }

        std::thread(&Server::handleClient, this, client_socket).detach();
    }
}

void Server::handleClient(int client_socket) {
    if (!authenticateClient(client_socket)) {
        std::cerr << "Authentication failed! Disconnecting client.\n";
#ifdef _WIN32
        closesocket(client_socket);
#else
        close(client_socket);
#endif
        return;
    }
    
    processClientMessages(client_socket);
}

void Server::startServer() {
    dbConn = connectToDatabase();
    if (!dbConn) {
        std::cerr << "Failed to connect to the database.\n";
        return;
    }

    int server_socket = setupServerSocket();
    if (server_socket == -1) {
        return;
    }

    acceptClients(server_socket);

    closeDatabase(dbConn);
}

void Server::broadcastMessage(const std::string& message, int sender_fd) {
    std::lock_guard<std::mutex> lock(client_mutex); // Protect shared resource

    for (int client : clients) {
        if (client != sender_fd) { // Don't send back to sender
            send(client, message.c_str(), message.size(), 0);
        }
    }
}
