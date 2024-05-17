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
// Example: WebSocket server, synchronous
//
//------------------------------------------------------------------------------

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <map>
#include <string>
#include <thread>
#include <mutex>
#include <queue>
#include <chrono>
#include <ctime>

// #include <nlohmann/json.hpp>




namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

//------------------------------------------------------------------------------

struct  ServerState {

    public:
    int user_num;
    std::map<std::string,std::queue<beast::flat_buffer>> user_state;
};

//@param buffer type
//adds sendt messages into the messages queue of each user
template <typename T>
void save_text(const T& buffer, ServerState& s_state,const std::string &userName, std::mutex&client_mtx){

    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    std::lock_guard<std::mutex> lock(client_mtx);
    
    s_state.user_state[userName].push(buffer);
   
    std::cout<<std::ctime(&now_time)<<"message saved for "<<userName;


}


//print all users being handled by server
void active_users (ServerState& s_state){
    for (auto& element: s_state.user_state){
        std::string user = element.first;
        std::cout<<user;
    }
}


//prints current time
void time_log(){
         auto now = std::chrono::system_clock::now();
        std::time_t now_time = std::chrono::system_clock::to_time_t(now);

        std::cout<<std::ctime(&now_time);
}



// Echoes back all received WebSocket messages
void
do_session(tcp::socket socket, ServerState &s_state, std::mutex &client_mtx)
{
    try
    {
        auto now = std::chrono::system_clock::now();
        std::time_t now_time = std::chrono::system_clock::to_time_t(now);

        std::cout<<std::ctime(&now_time);
        printf("session running\n");

        std::lock_guard<std::mutex> lock(client_mtx);

        // Construct the stream by moving in the socket
        websocket::stream<tcp::socket> ws{std::move(socket)};

        //stores userName field after handshake 
        std::string userName;
        

        ws.set_option(websocket::stream_base::decorator(
            [&s_state, &userName](websocket::response_type& res)
            {   
                
                res.set(http::field::server,
                    std::string(BOOST_BEAST_VERSION_STRING) +
                        " websocket-server-sync");
                        
                // Access the request
                auto& req = res.base();
                

                // Access the custom field
                userName  = std::string(req["username"]);

                if (s_state.user_state.find(userName) == s_state.user_state.end()){
                    
                    s_state.user_state[userName] = std::queue<beast::flat_buffer>();
                }

            }
        
        ));


        // Accept the websocket handshake
        ws.accept();

        now = std::chrono::system_clock::now();
        now_time = std::chrono::system_clock::to_time_t(now);
        std::cout<<std::ctime(&now_time);
        printf("handshake accepted\n%");

        

        for(;;)
        {
            // This buffer will hold the incoming message
            beast::flat_buffer buffer;

            // Read a message
            ws.read(buffer);

            std::cout << beast::make_printable(buffer.data()) << std::endl;

            save_text(buffer,s_state,userName,client_mtx);

            active_users(s_state);

            // // Echo the message back
            // ws.text(ws.got_text());
            // ws.write(buffer.data());
        }

            std::unique_lock<std::mutex> unlock(client_mtx);

    }
    catch(beast::system_error const& se)
    {   


        // This indicates that the session was closed
        if(se.code() != websocket::error::closed)
            std::cerr << "Error: " << se.code().message() << std::endl;

        for(auto &user: s_state.user_state){
            std::queue<boost::beast::flat_buffer> &messages = user.second;
            
            while(!messages.empty()) {
                beast::flat_buffer &message = messages.front();
                std::cout << beast::make_printable(message.data()) << std::endl;
                messages.pop();
            }
        }


    }
    catch(std::exception const& e)
    {   
        


        std::cerr << "Error: " << e.what() << std::endl;

        for(auto &user: s_state.user_state){
            std::queue<boost::beast::flat_buffer> &messages = user.second;
            while(!messages.empty()) {
                beast::flat_buffer &message = messages.front();
                std::cout << beast::make_printable(message.data()) << std::endl;
                messages.pop();
            }
        }
    }
    catch(...){
        std::cerr<<"Unkonwn errror";
    }

        

}



//---------------------------------------------MAIN---------------------------------

int main(int argc, char* argv[])
{
    try
    {
        ServerState server_state;
        std::mutex client_mtx;
        std::unique_lock<std::mutex> lock(client_mtx);

        // Check command line arguments.
        if (argc != 3)
        {
            std::cerr <<
                "Usage: websocket-server-sync <address> <port>\n" <<
                "Example:\n" <<
                "    websocket-server-sync 0.0.0.0 8080\n";
            return EXIT_FAILURE;
        }
        auto const address = net::ip::make_address(argv[1]);
        auto const port = static_cast<unsigned short>(std::atoi(argv[2]));

        // The io_context is required for all I/O
        net::io_context ioc{1};

        // The acceptor receives incoming connections
        tcp::acceptor acceptor{ioc, {address, port}};

        auto now = std::chrono::system_clock::now();
        std::time_t now_time = std::chrono::system_clock::to_time_t(now);

        std::cout<<ctime(&now_time); printf(" app and running \n");

        
        for(;;)
        {   

            // This will receive the new connection
            tcp::socket socket{ioc};

            // Block until we get a connection
            acceptor.accept(socket);


            now = std::chrono::system_clock::now();
            now_time = std::chrono::system_clock::to_time_t(now);

            std::cout<<std::ctime(&now_time);
            printf("Acceptor recieved\n"); 
            
            lock.unlock();
            // Launch the session, transferring ownership of the socket
            std::thread(
                &do_session,
                 std::move(socket), std::ref(server_state),std::ref(client_mtx)).detach();
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}



// g++ -IC:/Users/blaik/OneDrive/Desktop/cppinstall/vcpkg/installed/x64-windows/include sockserv.cpp -o sockserv.exe -lws2_32
