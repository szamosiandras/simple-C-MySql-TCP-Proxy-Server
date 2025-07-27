#pragma once


#include <boost/asio.hpp>
#include <memory>
#include <queue>

namespace io = boost::asio;
using io::ip::tcp;

using messageHandler = std::function<void(std::string)>;
using errorHandler = std::function<void()>;


class TCPConnection : public std::enable_shared_from_this<TCPConnection>
{
public: 
	using tcp_connection_pointer = std::shared_ptr<TCPConnection>;

	static tcp_connection_pointer createConnection(tcp::socket&& _socket) {

		return tcp_connection_pointer(new TCPConnection(std::move(_socket)));
	}

	void start(messageHandler&& _message_handler, errorHandler&& _error_handler);
	void post(const std::string&message);
	tcp::socket& getSocket();

	inline const std::string& getUsername() const { return username; };
private:
	//Factory
	explicit TCPConnection(tcp::socket&& _socket);

	//wait for a new message from client
	void asyncRead();

	void onRead(boost::system::error_code ec, size_t bite_transfered);

	void asyncWrite();
	void onWrite(boost::system::error_code ec, size_t bite_transfered);

	messageHandler _messageHandler;
	errorHandler _errorHandler;

	tcp::socket socket;
	std::string username;

	std::queue<std::string> outgoing_messages;
	io::streambuf stream_buf{65536};


};

