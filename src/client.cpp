#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
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
#include <string>

#define PORT 8080

class Client {
public:
    void startClient(const std::string& server_ip);
};

void Client::startClient(const std::string& server_ip) {
#ifdef _WIN32
    // Initialize Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Failed to initialize Winsock. Error code: " << WSAGetLastError() << std::endl;
        return;
    }
#endif

    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
#ifdef _WIN32
        std::cerr << "Failed to create socket. Error code: " << WSAGetLastError() << std::endl;
#else
        std::cerr << "Failed to create socket. Error: " << strerror(errno) << std::endl;
#endif
        return;
    }

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, server_ip.c_str(), &server_addr.sin_addr) <= 0) {
        std::cerr << "Invalid server IP address." << std::endl;
#ifdef _WIN32
        closesocket(client_socket);
        WSACleanup();
#else
        close(client_socket);
#endif
        return;
    }

    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
#ifdef _WIN32
        std::cerr << "Failed to connect to the server. Error code: " << WSAGetLastError() << std::endl;
        closesocket(client_socket);
        WSACleanup();
#else
        std::cerr << "Failed to connect to the server. Error: " << strerror(errno) << std::endl;
        close(client_socket);
#endif
        return;
    }

    std::cout << "Connected to the server! Type your messages below." << std::endl;

    std::thread receiveThread([&]() {
        char buffer[1024];
        while (true) {
            memset(buffer, 0, sizeof(buffer));
            int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
            if (bytes_received <= 0) {
                std::cerr << "Disconnected from server." << std::endl;
                break;
            }
            std::cout << "Server: " << buffer << std::endl;
        }
    });

    std::string message;
    while (true) {
        std::getline(std::cin, message);
        if (message == "exit") break;
        send(client_socket, message.c_str(), message.size(), 0);
    }

    receiveThread.join();
#ifdef _WIN32
    closesocket(client_socket);
    WSACleanup();
#else
    close(client_socket);
#endif
}
