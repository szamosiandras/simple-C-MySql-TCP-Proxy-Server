#pragma once

#include <boost/asio.hpp>
#include <memory>

namespace io = boost::asio;
using io::ip::tcp;

// A single client-DB proxy session that relays traffic and logs SQL queries
class ProxySession : public std::enable_shared_from_this<ProxySession> {
public:
    using pointer = std::shared_ptr<ProxySession>;

    // Factory method to create and wrap ProxySession in a shared_ptr
    static pointer create(boost::asio::ip::tcp::socket client_socket,
        boost::asio::io_context& io_context,
        const std::string& db_host, int db_port) {
        return pointer(new ProxySession(std::move(client_socket), io_context, db_host, db_port));
    }

    // Starts the proxy session (connects to DB and begins relaying)
    void start();

private:
    // Constructor: sets up sockets and DB connection params
    ProxySession(boost::asio::ip::tcp::socket client_socket,
        boost::asio::io_context& io_context,
        const std::string& db_host, int db_port);

    // Initiates connection to the actual MySQL server
    void connectToDB();

    // Relays traffic from client to DB and logs SQL queries
    void startClientToDBRelay();

    // Relays traffic from DB to client
    void startDBToClientRelay();

    // Logs only valid SQL queries to a file
    void logSQLQuery(const std::string& query);

    // Handles socket errors and closes both connections
    void handleError(const std::string& origin, boost::system::error_code);

    tcp::socket client_socket;     // Socket for client connection
    tcp::socket db_socket;         // Socket for DB connection

    std::array<char, 8192> client_buffer; // Buffer for reading from client
    std::array<char, 8192> db_buffer;     // Buffer for reading from DB

    std::string db_host;           // MySQL host
    int db_port;                   // MySQL port
};
