#include "TCPConnection.h"
#include <iostream>




TCPConnection::TCPConnection(tcp::socket&& _socket):
	socket(std::move(_socket))
{
	boost::system::error_code ec;
	std::stringstream name;

	name << socket.remote_endpoint(ec); 
	if (ec)
		name << "[unknown]";

	username = name.str();
}

void TCPConnection::asyncRead()
{
	io::async_read_until(socket, stream_buf, "\n", [self = shared_from_this()](boost::system::error_code ec, size_t bite_transfered) {

		self->onRead(ec, bite_transfered);
		});
}

void TCPConnection::onRead(boost::system::error_code ec, size_t bite_transfered)
{

	if (ec) {
		
		socket.close();

		_errorHandler();
		return;
	}


	std::stringstream message;

	message << username << ": " << std::istream(&stream_buf).rdbuf();
	stream_buf.consume(bite_transfered);


	_messageHandler(message.str());
	asyncRead();

}

void TCPConnection::asyncWrite()
{
	io::async_write(socket, io::buffer(outgoing_messages.front()), [self = shared_from_this()](boost::system::error_code ec, size_t bite_transfered) {

		self->onWrite(ec, bite_transfered);
		});
}

void TCPConnection::onWrite(boost::system::error_code ec, size_t bite_transfered)
{
	if (ec) {

		socket.close();

		_errorHandler();

		return;
	}

	outgoing_messages.pop();

	if (!outgoing_messages.empty()) {

		asyncWrite();
	}
}


void TCPConnection::start(messageHandler&& _message_handler, errorHandler&& _error_handler)
{
	_messageHandler = std::move(_message_handler);
	_errorHandler = std::move(_error_handler);

	asyncRead();

}

void TCPConnection::post(const std::string& message)
{
	bool queue_idle = outgoing_messages.empty();
	outgoing_messages.push(message);

	if (queue_idle) {

		asyncWrite();
	}
}

tcp::socket& TCPConnection::getSocket()
{

		return socket;
}
