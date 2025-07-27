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
