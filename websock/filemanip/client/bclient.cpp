#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/config.hpp>
#include <boost/asio/connect.hpp>
#include <boost/beast.hpp>
#include <iostream>
#include <string>
#include <fstream>
#include<cmath>




namespace http = boost::beast::http;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;

int file_upload(std:: string);
int file_size(std::ifstream & );


int main() {

    while (true){

        int user_input;
        printf("enter file name\n");
        std::string file_input;
        std::cout<<"Enter 1 to or 2 to upload file"<<std::endl;
        std::cin>>user_input;

        switch (user_input)
        {
            case 1:
            std::cin>>file_input;
            file_upload(file_input);
            break;

            case 2:
            break;
        }
    }





}

// ...


int file_upload(std::string filename ){

    try{
    // Set up an I/O context and a TCP resolver
    boost::asio::io_context ioc;
    tcp::resolver resolver{ioc};

    
    // Resolve the domain name to an IP address
    // auto const results = resolver.resolve("host.docker.internal", "5000"); //to be used in docker only 

    auto const results = resolver.resolve("localhost", "5000"); //for outside docke use only 

    // Set up a TCP socket and connect to the server
    tcp::socket socket{ioc};
    net::connect(socket, results.begin(), results.end());

    //open  file
    std::ifstream file(filename, std::ios::binary);

    if (!file.is_open()){
        std::cerr<<"failed to open file"<<std::endl;
        return 1;
    }

    const size_t chunk_size = 1024;

    //write the file to buffer
    std::vector<char> buffer(chunk_size);

    //get total chunk number 
    int chunk_count = 1;

    printf("file_size%d\n", file_size(file));
    int total_chunks = std::ceil(static_cast<double>(file_size(file)) / chunk_size);
    printf("total_chunks%d\n",total_chunks);



do {
    file.read(buffer.data(), chunk_size);
    std::streamsize bytes_read = file.gcount();

    if (bytes_read > 0) {
        net::connect(socket, results.begin(), results.end());

        // Create a file_body request
        http::request<http::string_body> req;

        // Setup the http request
        req.method(http::verb::post);
        req.target("/chunkfile");
        req.version(11);
        req.set(http::field::host, "localhost");
        req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
        req.set(http::field::content_type, "application/octet-stream");
        req.set("chunk-n", std::to_string(chunk_count));
        req.set("total-chunk",std::to_string(total_chunks));

        req.body().assign(std::string(buffer.begin(), buffer.begin() + bytes_read));

        //prepare message payload field
        req.prepare_payload();

        // Write the request to the socket
        http::write(socket, req);
        printf("chunk_count%d\n",chunk_count);
        printf("total_chunks%d\n",total_chunks);

        // // Step 5: Receive and print the HTTP response
        boost::beast::flat_buffer res_buffer;
        http::response<http::dynamic_body> response;
        http::read(socket, res_buffer, response);
        std::cout << "Response: " << response<< std::endl;

        chunk_count++;
    }
} while (!file.eof());

    // Gracefully close the socket
    boost::beast::error_code ec_shut;
    socket.shutdown(tcp::socket::shutdown_both, ec_shut);

    // not_connected happens sometimes so don't bother reporting it.
    if(ec_shut && ec_shut != net::error::not_connected)
        throw boost::beast::system_error{ec_shut};
    return 0;

    }catch(const std::exception& e){
        std::cerr<<"Error: "<<e.what()<<std::endl;
        return 1;
    }


}


int file_size(std::ifstream & file){
    
    // gettig file total chunk size 
    file.seekg(0,file.end);
    int file_size  = file.tellg();
    file.seekg(0,file.beg);

    return file_size;
}






// g++ -IC:/Users/blaik/OneDrive/Desktop/cppinstall/vcpkg/installed/x64-windows/include bclient.cpp -o bclient.exe -lws2_32

//"pdffilesend.pdf"

//