#include <iostream>
#include "TCPServer.h"
#include "ProxySession.h"

using boost::asio::ip::tcp;

// Constructor: initializes acceptor to listen on specified IP version and port
TCPServer::TCPServer(IPV _ip_version, int _client_port, std::string _mysql_ip, int _mysql_port)
	: ip_version(_ip_version),
	client_port(_client_port),
	mysql_ip(_mysql_ip),
	mysql_port(_mysql_port),
	acceptor(io_context, tcp::endpoint(ip_version == IPV::V4 ? tcp::v4() : tcp::v6(), client_port))
{}

// Runs the server: starts accepting and enters the Boost.Asio event loop
int TCPServer::run()
{
	try {
		startAcept();          // Begin accepting client connections
		io_context.run();      // Run event loop
	}
	catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		return -1;
	}
	return 0;
}

// Begins asynchronous accept of a new client connection
void TCPServer::startAcept()
{
	socket.emplace(io_context);  // Create socket for next incoming connection

	acceptor.async_accept(*socket, [this](const boost::system::error_code& error) {
		if (!error) {
			// On successful client connection, create and start a proxy session
			auto proxy_session = ProxySession::create(
				std::move(*socket), io_context, mysql_ip, mysql_port
			);
			proxy_session->start();
		}
		startAcept(); // Continue accepting next connection
		});
}
