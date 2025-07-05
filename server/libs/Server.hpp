#pragma once

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/asio/dispatch.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/asio/strand.hpp>
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>

void psleep(unsigned int seconds);

#ifdef __WIN32
//   #include <window.h>

  void psleep(unsigned int seconds) {
    Sleep(seconds * 1000);
  }

#elseif          
  #include <unistd.h>
    void psleep(unsigned int seconds)
{
  sleep(seconds);
}
#endif

#define MAX_BUFF 1024

namespace net = boost::asio;
namespace beast = boost::beast;
namespace websocket = beast::websocket;
namespace net = boost::asio;
namespace ssl = boost::asio::ssl;

using tcp = net::ip::tcp;

namespace Server{

class Session : public std::enable_shared_from_this<Session> {
    public:
    static void fail(beast::error_code ec, char const* what) {
        std::cerr << what << ": " << ec.message() <<"\n";
    }

  Session(tcp::socket&& socket, ssl::context& ctx)
        : ws_(std::move(socket), ctx)
    {
    }
    void run();
    
    void on_run();
    
    void on_handshake(beast::error_code ec);
    void on_accept(beast::error_code ec) ;
    void do_read() ;
    void on_read(beast::error_code ec, std::size_t bytes_transferred);
    void on_write(beast::error_code ec, std::size_t bytes_transferred);
        
    private:
        websocket::stream<ssl::stream<beast::tcp_stream>> ws_;
        beast::flat_buffer buffer;
};
}