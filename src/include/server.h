#pragma once

#include <vector>
#include <mutex>
#include <string>

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
};