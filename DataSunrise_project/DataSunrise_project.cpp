#include <iostream>  // For std::cout and std::cerr

#ifdef _WIN32
#define _WIN32_WINNT 0x0A00  // Set Windows version target to Windows 10
#endif

#include "TCPServer.h"  // Custom TCP proxy server class

using boost::asio::ip::tcp;

int main()
{
    // Initialize the proxy server to listen on 127.0.0.1:1339 and forward to MySQL at 127.0.0.1:3306
    TCPServer server{ IPV::V4, 1339, "127.0.0.1", 3306 };

    // Start the proxy server's async event loop
    server.run();

    return 0;
}
