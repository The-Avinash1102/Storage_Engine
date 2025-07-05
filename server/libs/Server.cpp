#include "Server.hpp"



namespace Server {

    void Session::fail(beast::error_code ec, char const* what) {
        std::cerr << what << ": " << ec.message() <<"\n";
    }

  
    void Session::run()
    {  //bind_front_handler creates a new callable object
        //get_executor returns the i/o executor associated with our stream.
        net::dispatch(ws_.get_executor(), beast::bind_front_handler( &Session::on_run, shared_from_this()));
    }
    void Session::on_run()
    {
        beast::get_lowest_layer(ws_).expires_after(std::chrono::seconds(30));

        ws_.next_layer().async_handshake(ssl::stream_base::server, beast::bind_front_handler( &Session::on_handshake, shared_from_this()));
    }

    void Session::on_handshake(beast::error_code ec) {
        if (ec)
            return fail(ec, "handshake");

        ws_.async_accept(beast::bind_front_handler(&Session::on_accept,shared_from_this()));
    }

    void Session::on_accept(beast::error_code ec) {
        if (ec)
            return fail(ec, "accept");

        do_read();
    }

    void Session::do_read() {
        ws_.async_read(
            buffer,
            beast::bind_front_handler(
                &Session::on_read,
                shared_from_this()));
    }

    void Session::on_read(beast::error_code ec, std::size_t bytes_transferred) {
        boost::ignore_unused(bytes_transferred);

        if (ec == websocket::error::closed)
            return;
        if (ec)
            fail(ec, "read");

        ws_.text(ws_.got_text());
        ws_.async_write(
            buffer.data(),
            beast::bind_front_handler(
                &Session::on_write,
                shared_from_this()));
    }

    void Session::on_write(beast::error_code ec, std::size_t bytes_transferred) {
        boost::ignore_unused(bytes_transferred);

        if (ec)
            return fail(ec, "write");

        buffer.consume(buffer.size());

        do_read();
    }
};