# simple-C-MySql-TCP-Proxy-Server

Overview

This is an asynchronous TCP proxy written in C++ using Boost.Asio. It forwards MySQL client connections to a real MySQL server and logs all SQL queries.

    Chosen DBMS: MySQL

    Chosen Networking Library: Boost.Asio

    Encrypted connections are not supported 

Requirements

To test this proxy, you need:

    A running MySQL server on:

        IP: 127.0.0.1

        Port: 3306

    MySQL command-line client installed

    Windows system with Visual Studio and Boost libraries

How to Run

    Start your MySQL server locally on 127.0.0.1:3306

    Build and run the proxy executable (DataSunrise_project.exe)

    Open a terminal (PowerShell or CMD), and connect to the proxy using:

    mysql -h 127.0.0.1 -P 1339 -u root -p

    After connecting, you can run SQL commands such as:

    CREATE DATABASE test;
    USE test;
    CREATE TABLE users (id INT, name VARCHAR(50));
    INSERT INTO users VALUES (1, 'Alice');
    SELECT * FROM users;



   #  1. Performance Problems

During development, the main performance issue encountered was that the proxy server only accepts and handles one client connection at a time. After accepting and starting one session, the server does not maintain any global state or session pool to track or manage multiple simultaneous clients.

Additional issues:

    No reuse or pooling of MySQL connections (each client opens a new socket to the DB).

    Lack of shared data structures or statistics tracking limits visibility and coordination.

    No concurrency control or scaling beyond a single-threaded io_context.

# 2. Solutions

The following techniques were or can be applied to improve performance:

    Async accept loop: Using Boost.Asio’s asynchronous acceptor.async_accept() ensures non-blocking connection handling.

    Unordered set for session tracking: Introducing an unordered_set of shared pointers (or weak pointers) to ProxySession objects will allow the server to manage and track multiple concurrent clients.

    IO context separation: The DB and client sockets use the same io_context, which simplifies control flow and avoids blocking operations.

    Raw buffer relaying: Replacing streambuf with fixed std::array<char, 8192> avoids unnecessary dynamic allocation and improves relay throughput.

  #  3. Potential Improvements

    Thread pool support: Run multiple io_context.run() instances across CPU cores to handle high concurrency (e.g., using boost::asio::thread_pool or manual std::thread loop).

    Connection pooling: Maintain a pool of persistent MySQL connections to avoid overhead of reconnecting on each client connection.

    Backpressure control: Implement read/write throttling or buffering when client or DB stalls, to avoid overwhelming one side.

    Metrics and monitoring: Add counters or logging for active sessions, queries per second, and errors for visibility and tuning.

    Query parsing layer: Use a lightweight MySQL packet parser to extract exact SQL statements instead of relying on regex against raw buffers.

    Protocol-aware enhancements: Add support for inspecting login packets or blocking specific SQL types (like DROP, ALTER) based on policies.
