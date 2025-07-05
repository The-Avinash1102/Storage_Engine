#include "Sessions.hpp"

namespace Sessions {

    HttpSession::HttpSession(tcp::socket&& socket)
        : stream_(std::move(socket)) {}

    void HttpSession::run() {
        do_read();
    }

    void HttpSession::do_read() {
        auto self = shared_from_this();
        http::async_read(stream_, buffer_, req_,
            [self](beast::error_code ec, std::size_t bytes_transferred) {
                boost::ignore_unused(bytes_transferred);
                if (!ec)
                    self->handle_request();
            });
    }

    void HttpSession::handle_request() {
        if (req_.target() == "/sayHi") {
            http::string_body::value_type body = "{\"text\": \"Hello\"}";
            auto const size = body.size();
            http::response<http::string_body> res{http::status::ok, req_.version()};
            res.set(http::field::server, "Boost.Beast");
            res.set(http::field::content_type, "application/json");
            res.content_length(size);
            res.body() = std::move(body);
            auto self = shared_from_this();
            http::async_write(stream_, res,
                [self](beast::error_code ec, std::size_t) {
                    if (!ec)
                        self->stream_.socket().shutdown(tcp::socket::shutdown_send, ec);
                });
        } else {
            http::response<http::string_body> res{http::status::not_found, req_.version()};
            res.set(http::field::server, "Boost.Beast");
            res.set(http::field::content_type, "text/plain");
            res.body() = "Route not found";
            auto self = shared_from_this();
            http::async_write(stream_, res,
                [self](beast::error_code ec, std::size_t) {
                    if (!ec)
                        self->stream_.socket().shutdown(tcp::socket::shutdown_send, ec);
                });
        }
    }

    Listener::Listener(net::io_context& ioc, tcp::endpoint endpoint)
        : ioc_(ioc), acceptor_(net::make_strand(ioc)) {

        beast::error_code ec;
        acceptor_.open(endpoint.protocol(), ec);
        if (ec) {
            std::cerr << "Error opening acceptor: " << ec.message() << "\n";
            return;
        }
        
        acceptor_.bind(endpoint, ec);
        if (ec) {
            std::cerr << "Error binding acceptor: " << ec.message() << "\n";
            return;
        }
        
        acceptor_.listen(net::socket_base::max_listen_connections, ec);
        if (ec) {
            std::cerr << "Error listening: " << ec.message() << "\n";
            return;
        }
    }

    void Listener::run() {
        do_accept();
    }

    void Listener::do_accept() {
        acceptor_.async_accept(
            net::make_strand(ioc_),
            beast::bind_front_handler(
                &Listener::on_accept,
                shared_from_this()));
    }

    void Listener::on_accept(beast::error_code ec, tcp::socket socket) {
        if (!ec) {
            std::make_shared<HttpSession>(std::move(socket))->run();
        }
        do_accept();
    }

} 