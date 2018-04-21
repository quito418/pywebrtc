//// Copyright (c) 2016-2017 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//
#include "common/root_certificates.hpp"

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <cstdlib>
#include <iostream>
#include <string>
#include <cstddef>
#include "picojson/picojson.h"

using tcp = boost::asio::ip::tcp;               // from <boost/asio/ip/tcp.hpp>
namespace ssl = boost::asio::ssl;               // from <boost/asio/ssl.hpp>
namespace websocket = boost::beast::websocket;  // from <boost/beast/websocket.hpp>

void send_websockets_message(std::string host, std::string port,
    std::string path, std::string message);
  
// Sends a WebSocket message and prints the response
int main(int argc, char** argv)
{
      if(argc != 2)
      {
          std::cerr <<
              "Usage: websocket <text>\n" <<
              "Example:\n" <<
              "    websocket \"Hello, world!\"\n";
          return EXIT_FAILURE;
      }
      auto const connection_id = argv[1];
      picojson::object message;
      message.insert(std::make_pair("type", picojson::value("kind")));
      message.insert(std::make_pair("kind", picojson::value("server")));
      message.insert(std::make_pair("connection_id", picojson::value(connection_id)));
      auto const host = "ccr-frontend-0.jemmons.us";
      auto const port = "443";
      auto const path = "ccr";
      std::string text = picojson::value(message).serialize();
      std::cout << text << std::endl;
      send_websockets_message(host, port, path, text);
      return EXIT_SUCCESS;
}

void send_websockets_message(std::string host, std::string port,
    std::string path, std::string text) {
    try
    {
        // The io_context is required for all I/O
        boost::asio::io_context ioc;

        // The SSL context is required, and holds certificates
        ssl::context ctx{ssl::context::sslv23_client};

        // This holds the root certificate used for verification
        load_root_certificates(ctx);

        // These objects perform our I/O
        tcp::resolver resolver{ioc};
        websocket::stream<ssl::stream<tcp::socket>> ws{ioc, ctx};

        // Look up the domain name
        auto const results = resolver.resolve(host, port);

        // Make the connection on the IP address we get from a lookup
        boost::asio::connect(ws.next_layer().next_layer(), results.begin(), results.end());

        // Perform the SSL handshake
        ws.next_layer().handshake(ssl::stream_base::client);

        // Perform the websocket handshake
        ws.handshake(host, path);

        // Send the message
        ws.write(boost::asio::buffer(std::string(text)));

        // This buffer will hold the incoming message
        boost::beast::multi_buffer b;

        // Read a message into our buffer
        ws.read(b);

        // The buffers() function helps print a ConstBufferSequence
        std::cout << boost::beast::buffers(b.data()) << std::endl;

        try {
            // Close the WebSocket connection
            ws.close(websocket::close_code::normal);
        }
        catch(std::exception const& e) {
           std::cout << "Close error: " << e.what() << std::endl; 
        }

        // If we get here then the connection is closed gracefully

    }
    catch(std::exception const& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }

}
