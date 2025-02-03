#include <iostream>
#include "include/client.h"
#include "include/server.h"
#include "include/db.h"

// void menu() {
//     std::cout << "\n===== Database Test Menu =====" << std::endl;
//     std::cout << "1. Register User" << std::endl;
//     std::cout << "2. Login User" << std::endl;
//     std::cout << "3. Save Message" << std::endl;
//     std::cout << "4. Get Messages" << std::endl;
//     std::cout << "5. Exit" << std::endl;
//     std::cout << "Choose an option: ";
// }

// int main() {
//     // Establish a connection to the database
//     PGconn* conn = connectToDatabase();
//     if (!conn) {
//         return -1; // Exit if the connection fails
//     }

//     int choice = 0;
//     while (choice != 5) {
//         menu();
//         std::cin >> choice;
//         std::cin.ignore(); // Handle newline input

//         switch (choice) {
//         case 1: {
//             // Register a new user
//             std::string username, password;
//             std::cout << "Enter username: ";
//             std::getline(std::cin, username);
//             std::cout << "Enter password: ";
//             std::getline(std::cin, password);
//             std::cout << registerUser(conn, username, password) << std::endl;
//             break;
//         }
//         case 2: {
//             // Login a user
//             std::string username, password;
//             std::cout << "Enter username: ";
//             std::getline(std::cin, username);
//             std::cout << "Enter password: ";
//             std::getline(std::cin, password);
//             std::cout << loginUser(conn, username, password) << std::endl;
//             break;
//         }
//         case 3: {
//             // Save a message
//             int senderId, receiverId;
//             std::string message;
//             std::cout << "Enter sender ID: ";
//             std::cin >> senderId;
//             std::cout << "Enter receiver ID: ";
//             std::cin >> receiverId;
//             std::cin.ignore(); // Handle newline input
//             std::cout << "Enter message: ";
//             std::getline(std::cin, message);
//             saveMessage(conn, senderId, receiverId, message);
//             break;
//         }
//         case 4: {
//             // Retrieve messages
//             int userId;
//             std::cout << "Enter user ID to retrieve messages: ";
//             std::cin >> userId;
//             getMessage(conn, userId);
//             break;
//         }
//         case 5:
//             std::cout << "Exiting program." << std::endl;
//             break;
//         default:
//             std::cout << "Invalid choice. Please try again." << std::endl;
//             break;
//         }
//     }

//     // Close the database connection before exiting
//     closeDatabase(conn);
//     return 0;
// }

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