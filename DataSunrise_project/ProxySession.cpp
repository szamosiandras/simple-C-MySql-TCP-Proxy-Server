#include "ProxySession.h"
#include <fstream>
#include <iostream>
#include <regex>

// Constructor: initializes client socket, DB socket, and connection parameters
ProxySession::ProxySession(boost::asio::ip::tcp::socket client_socket_,
    boost::asio::io_context& io_context,
    const std::string& db_host_, int db_port_)
    : client_socket(std::move(client_socket_)),
    db_socket(io_context),
    db_host(db_host_),
    db_port(db_port_)
{}

// Entry point to start the proxy session
void ProxySession::start()
{
    connectToDB();  // Begin async DB connection
}

// Resolves and connects to the MySQL DB, then initiates DB→Client relay (handshake)
void ProxySession::connectToDB()
{
    auto self = shared_from_this();
    tcp::resolver resolver(client_socket.get_executor());

    resolver.async_resolve(db_host, std::to_string(db_port),
        [this, self](const boost::system::error_code& ec, tcp::resolver::results_type endpoints) {
            if (!ec) {
                boost::asio::async_connect(db_socket, endpoints,
                    [this, self](boost::system::error_code ec, tcp::endpoint) {
                        if (!ec) {
                            startDBToClientRelay();  // Start relaying DB handshake first
                        }
                        else {
                            handleError("MySQL connect", ec);
                        }
                    });
            }
            else {
                handleError("MySQL resolve", ec);
            }
        });

    std::cout << "[Proxy] Connecting to MySQL DB: " << db_host << ":" << db_port << std::endl;
}

// Reads data from client, logs query, and forwards it to DB
void ProxySession::startClientToDBRelay()
{
    auto self = shared_from_this();
    client_socket.async_read_some(boost::asio::buffer(client_buffer),
        [this, self](boost::system::error_code ec, std::size_t bytes_transferred) {
            if (!ec) {
                std::string query(client_buffer.data(), bytes_transferred);
                logSQLQuery(query);  // Log the SQL query if valid

                boost::asio::async_write(db_socket,
                    boost::asio::buffer(client_buffer.data(), bytes_transferred),
                    [this, self](boost::system::error_code ec, std::size_t) {
                        if (!ec)
                            startClientToDBRelay();  // Continue relaying
                        else
                            handleError("Client to DB write", ec);
                    });
            }
            else {
                handleError("Clien to DB read", ec);
            }
        });
}

// Reads data from DB and forwards it to client; triggers client relay once handshake is received
void ProxySession::startDBToClientRelay()
{
    auto self = shared_from_this();
    db_socket.async_read_some(boost::asio::buffer(db_buffer),
        [this, self](boost::system::error_code ec, std::size_t bytes_transferred) {
            if (!ec) {
                boost::asio::async_write(client_socket,
                    boost::asio::buffer(db_buffer.data(), bytes_transferred),
                    [this, self, bytes_transferred](boost::system::error_code ec, std::size_t) {
                        if (!ec) {
                            static bool started = false;
                            if (!started) {
                                started = true;
                                startClientToDBRelay();  // Delay client→DB until DB handshake completes
                            }
                            startDBToClientRelay();  // Continue DB→Client relay
                        }
                        else {
                            handleError("DB to Client write", ec);
                        }
                    });
            }
            else {
                handleError("DB to Client read", ec);
            }
        });
}

// Logs raw queries to file if they match a SQL keyword
void ProxySession::logSQLQuery(const std::string& query)
{
    static const std::regex sql_keywords(R"((SELECT|INSERT|UPDATE|DELETE|REPLACE|CREATE|DROP|ALTER|USE)\s+)",
        std::regex::icase);
    if (std::regex_search(query, sql_keywords)) {
        std::ofstream log("sql_queries.log", std::ios::app);
        log << query << std::endl;
    }
}

// Handles socket errors and gracefully shuts down both client and DB sockets
void ProxySession::handleError(const std::string& origin, boost::system::error_code ec)
{
    std::cerr << "[Error] " << origin << ": " << ec.message() << std::endl;

    boost::system::error_code ignored;
    client_socket.shutdown(tcp::socket::shutdown_both, ignored);
    client_socket.close(ignored);
    db_socket.shutdown(tcp::socket::shutdown_both, ignored);
    db_socket.close(ignored);
}
