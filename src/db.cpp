#include "include/db.h"
#include <iostream>

PGconn* connectToDatabase()
{
    const char* conninfo = "host = localhost dbname = chat_db user = postgres password = hdfw3784";
    PGconn* conn = PQconnectdb(conninfo);

    if(PQstatus(conn) != CONNECTION_OK)
    {
        std::cerr << "Database connection failed: " << PQerrorMessage(conn) << std::endl;
        PQfinish(conn);
        return nullptr;
    }

    std::cout << "Connected to PostgreSQL database." << std::endl;
    return conn;
}

std::string registerUser(PGconn* conn, const std::string& username, const std::string& password)
{
    std::string query = "INSERT INTO users (username, password) VALUES ($1, $2) RETURNING id;";
    const char* paramValues[] = { username.c_str(), password.c_str() };

    PGresult* res = PQexecParams(
        conn,
        query.c_str(),
        2,
        NULL,
        paramValues,
        NULL,   
        NULL,
        0
    );

    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
        std::cerr << "Registration failed: " << PQerrorMessage(conn) << std::endl;
        PQclear(res);
        return "Registration failed. Username may already exist.";
    }

    std::string userId = PQgetvalue(res, 0, 0);
    PQclear(res);
    return "Registration successful! User ID: " + userId;
}

std::string loginUser(PGconn* conn, const std::string& username, const std::string& password)
{
    std::string query = "SELECT id FROM users WHERE username = $1 AND password = $2";
    const char* paramValue[]  = { username.c_str(), password.c_str()};

    PGresult* res = PQexecParams(conn, query.c_str(), 2, NULL, paramValue, NULL, NULL, 0);

    if( PQresultStatus(res) != PGRES_TUPLES_OK || PQntuples(res) == 0)
    {
        std::cerr << "Login failed: " << PQerrorMessage(conn) << std::endl;
        PQclear(res);
        return "Login failed. Invalid username or password.";
    }

    std::string userId = PQgetvalue(res, 0, 0);
    PQclear(res);
    return "Login successful! User ID: " + userId;
}

void saveMessage(PGconn* conn, int senderId, int receiverId, const std::string& message)
{
    std::string query = "INSERT INTO messages (sender_id, receiver_id, message) VALUES ($1,$2,$3)";

    const char* paramValue[] = {std::to_string(senderId).c_str(), std::to_string(receiverId).c_str(), message.c_str()};

    PGresult* res = PQexecParams(conn, query.c_str(), 3, NULL, paramValue, NULL, NULL, 0);

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        std::cerr << "Failed to save message: " << PQerrorMessage(conn) << std::endl;
    } else {
        std::cout << "Message saved successfully!" << std::endl;
    }

    PQclear(res);
}

void getMessage(PGconn* conn, int userId)
{
    std::string query = "SELECT sender_id, timestamp, message FROM messages WHERE receiver_id = $1 ORDER BY timestamp;";
    const char* paramValues[] = {std::to_string(userId).c_str()};

    PGresult* res = PQexecParams(conn, query.c_str(), 1, NULL, paramValues, NULL, NULL, 0);

    if(PQresultStatus(res) != PGRES_TUPLES_OK)
    {
        std::cerr << "Failed to retireve message: " << PQerrorMessage(conn) << std::endl;
        PQclear(res);
        return;
    }

    int rows = PQntuples(res);
    for ( int i = 0; i < rows; ++i)
    {
        std::cout << "From User " << PQgetvalue(res, i, 0) 
                  << " :" << PQgetvalue(res, i, 1)
                  << " at " << PQgetvalue(res, i, 2) << std::endl;
    }

    PQclear(res);
}

void closeDatabase(PGconn* conn)
{
    if (conn != nullptr) {
        PQfinish(conn); // Properly close the connection
        std::cout << "Database connection closed." << std::endl;
    } else {
        std::cerr << "Attempted to close a null database connection." << std::endl;
    }
}