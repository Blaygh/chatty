

//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

//------------------------------------------------------------------------------
//
// Example: WebSocket client, synchronous
//
//------------------------------------------------------------------------------

//[example_websocket_client

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <cstdlib>
#include <iostream>
#include <string>
#include <chrono>
#include <ctime>
// #include <nlohmann/json.hpp>


namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

// Sends a WebSocket message and prints the response
int main(int argc, char** argv)
{
    try
    {

        // Check command line arguments.
        if(argc != 4)
        {
            std::cerr <<
                "Usage: websocket-client-sync <host> <port> <userName >\n" <<
                "Example:\n" <<
                "    websocket-client-sync echo.websocket.org 80 Blay";
            return EXIT_FAILURE;
        }
        std::string host = argv[1];
        auto const  port = argv[2];
        std::string userName = argv[3];

        // The io_context is required for all I/O
        net::io_context ioc;

        // These objects perform our I/O
        tcp::resolver resolver{ioc};
        websocket::stream<tcp::socket> ws{ioc};

        // Look up the domain name
        auto const results = resolver.resolve(host, port);

        // time function, for logging 
        auto now = std::chrono::system_clock::now();
        std::time_t now_time = std::chrono::system_clock::to_time_t(now);
        
            

        // Make the connection on the IP address we get from a lookup
        auto ep = net::connect(ws.next_layer(), results);

        // Update the host_ string. This will provide the value of the
        // Host HTTP header during the WebSocket handshake.
        // See https://tools.ietf.org/html/rfc7230#section-5.4
        host += ':' + std::to_string(ep.port());

        // Set a decorator to change the User-Agent of the handshake
        ws.set_option(websocket::stream_base::decorator(
            [userName](websocket::request_type& req)
            {
                req.set(http::field::user_agent,
                    std::string(BOOST_BEAST_VERSION_STRING) +
                        " websocket-client-coro");

                req.set("username", userName);
            }));

        // Perform the websocket handshake
        ws.handshake(host, "/");



        std::string user_input = "\0";
        // This buffer will hold the incoming message
        beast::flat_buffer buffer;


        do{
            //get user_input 
            std::cout<<"Enter your message";
            std::cin>>user_input;

            //Send the message
            ws.write(net::buffer(std::string(user_input)));

            std::cout << std::ctime(&now_time)<<" message sent ";

// ----------------------------------------------------------------------------------------------------------------------------------------
            // Read a message into our buffer
            // ws.read(buffer);

            // The make_printable() function helps print a ConstBufferSequence
            std::cout << beast::make_printable(buffer.data()) << std::endl;

            //reset buffer 
            buffer.consume(buffer.size());

        }while (user_input != "q");



        // Close the WebSocket connection
        ws.close(websocket::close_code::normal);

        // If we get here then the connection is closed gracefully


    }
    catch(std::exception const& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

// #include <nlohmann/json.hpp>
// #include <chrono>
// #include <ctime>

// // Create a JSON object
// nlohmann::json j;

// // Add a message and a timestamp to the JSON object
// j["message"] = "Hello, world!";
// j["timestamp"] = std::time(nullptr);

// // Convert the JSON object to a string
// std::string message = j.dump();

// // Send the message over the WebSocket connection
// ws.write(boost::asio::buffer(message));

// g++ -IC:/Users/blaik/OneDrive/Desktop/cppinstall/vcpkg/installed/x64-windows/include sockclnt.cpp -o sockclnt.exe -lws2_32
//./sockclnt localhost 8080 Blay