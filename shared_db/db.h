// Shared PostgreSQL connection helper.
// Reads the connection string from the PG_CONN_STRING environment
// variable. No credentials are ever hardcoded in this file.
// Date: 29/09/2026

#pragma once

#include <pqxx/pqxx>
#include <cstdlib>
#include <string>
#include <stdexcept>

inline std::string getConnectionString() {
    const char* envConn = std::getenv("PG_CONN_STRING");
    if (envConn != nullptr) {
        return std::string(envConn);
    }
    throw std::runtime_error("PG_CONN_STRING environment variable not set");
}
