#pragma once

#include <libpq-fe.h>
#include <string>

PGconn* connectToDatabase();
void closeDatabase(PGconn* conn);

std::string registerUser(PGconn* conn, const std::string& username, const std::string& password);
std::string loginUser(PGconn* conn, const std::string& username, const std::string& password);
void saveMessage(PGconn* conn, int senderId, int receiverId, const std::string& message);
void getMessages(PGconn* conn, int userId);