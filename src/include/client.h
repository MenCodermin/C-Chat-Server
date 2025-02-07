#pragma once
#include <mutex>
#include <string>
#include <vector>

class Client
{
   private:
    std::vector<int> clients;
    std::mutex client_mutex;
    std::string choice, username, password;
    std::string authMessage;

   public:
    void startClient(const std::string& server_ip);
    void authenticateUser(int client_socket);
    void startMessageReceiving(int client_socket);
    void startMessaging(int client_socket);
    
};