#pragma once

#include <mutex>
#include <string>
#include <vector>

class Server
{
   private:
    std::vector<int> clients;
    std::mutex client_mutex;

   public:
    void broadcastMessage(const std::string& message, int sender_fd);
    void handleClient(int client_socket);
    void startServer();
    void startClient(const std::string& server_ip);
    bool authenticateClient(int client_sokcet);
    void processClientMessages(int client_socket);
    int setupServerSocket();
    void acceptClients(int client_socket);
};