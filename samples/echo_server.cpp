#include <iostream>
#include <string>
#include <boost/bind.hpp>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/array.hpp>

using boost::asio::ip::tcp;

namespace  {

class Session {
  public:
    Session(boost::asio::io_service& io_service) : socket_(io_service) {

    }

    virtual ~Session() {}


    tcp::socket& socket() {
      return  socket_;
    }


    void start() {
      socket_.async_read_some(boost::asio::buffer(buf_),
                              boost::bind(&Session::handle_read, this,
                                          boost::asio::placeholders::error,
                                          boost::asio::placeholders::bytes_transferred));
    }


    void handle_read(const boost::system::error_code& error,
                     size_t bytes_transferred) {
      if (!error) {
        boost::asio::async_write(socket_, boost::asio::buffer(buf_, bytes_transferred),
                                 boost::bind(&Session::handle_write, this, boost::asio::placeholders::error));

      } else {
        delete this;
        if (error != boost::asio::error::eof) {
          puts("handle_read error");
          throw boost::system::system_error(error);
        }
      }

    }

    void handle_write(const boost::system::error_code& error) {
      if (!error) {
        socket_.async_read_some(boost::asio::buffer(buf_),
                                boost::bind(&Session::handle_read, this, boost::asio::placeholders::error,
                                            boost::asio::placeholders::bytes_transferred));

      } else {
        puts("handle_write error");
        delete this;
        throw boost::system::system_error(error);
      }
    }



  private:
    Session(const Session&);
    const Session& operator=(const Session&);

    tcp::socket socket_;
    boost::array<char, 1024> buf_;
};


class EchoServer {
  public:
    enum {
      PORT = 9877
    };

    EchoServer(boost::asio::io_service& io_service)
      : acceptor_(io_service, tcp::endpoint(tcp::v4(), PORT)) {
      start_accept();
    }

    virtual ~EchoServer() {}

    void start_accept() {
      Session* session = new Session(acceptor_.get_io_service());
      acceptor_.async_accept(session->socket(), boost::bind(&EchoServer::handle_accept, this,
                                                            session, boost::asio::placeholders::error));
    }

    void handle_accept(Session* _session, const boost::system::error_code& error) {
      if (!error) {
        _session->start();
        start_accept();

      } else {
        delete _session;
      }
    }



  private:
    EchoServer(const EchoServer&);
    const EchoServer& operator=(const EchoServer&);

    tcp::acceptor acceptor_;
};


} //namespace

int main(int argc, char const* argv[]) {
  try {
    boost::asio::io_service io_service;
    EchoServer  server(io_service);
    io_service.run();

  } catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}

