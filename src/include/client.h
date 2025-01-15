#pragma once
#include <vector>
#include <mutex>
#include <string>

class Client
{

private:
    std::vector<int> clients;
    std::mutex client_mutex;
public:
    void startClient(const std::string& server_ip);
};