#pragma once

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
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

namespace net = boost::asio;
namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
namespace ssl = boost::asio::ssl;

using tcp = net::ip::tcp;

namespace Sessions {

    class HttpSession : public std::enable_shared_from_this<HttpSession> {
        beast::tcp_stream stream_;
        beast::flat_buffer buffer_;
        http::request<http::string_body> req_;

    public:
        HttpSession(tcp::socket&& socket);
        void run();

    private:
        void do_read();
        void handle_request();
    };

    class Listener : public std::enable_shared_from_this<Listener> {
        net::io_context& ioc_;
        tcp::acceptor acceptor_;

    public:
        Listener(net::io_context& ioc, tcp::endpoint endpoint);
        void run();

    private:
        void do_accept();
        void on_accept(beast::error_code ec, tcp::socket socket);
    };

}