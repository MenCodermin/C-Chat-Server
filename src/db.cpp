#include "include/db.h"
#include <iostream>

PGconn* connectToDatabase()
{
    const char* conninfo = "host = localhost dbname = chat_db user = char_user password = chat_password";
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