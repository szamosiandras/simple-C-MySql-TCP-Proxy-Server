#pragma once

#include <boost/asio.hpp>
#include <optional>

// Enum to specify IP version (IPv4 or IPv6)
enum IPV {
	V4,
	V6
};

namespace io = boost::asio;

class TCPServer {
public:
	// Constructor: initializes the server with IP version, client listen port, DB host, and DB port
	TCPServer(IPV _ip_version, int _client_port, std::string _mysql_ip, int _mysql_port);

	// Starts the server event loop
	int run();

private:
	IPV ip_version;                // IPv4 or IPv6
	int client_port;              // Port to accept client connections
	int mysql_port;               // Target MySQL DB port
	std::string mysql_ip;         // Target MySQL DB IP

	io::io_context io_context;    // Boost.Asio IO context for async ops
	io::ip::tcp::acceptor acceptor; // TCP acceptor for incoming connections
	std::optional<io::ip::tcp::socket> socket; // Optional socket for accepting a new client

	// Starts accepting new connections
	void startAcept();
};
