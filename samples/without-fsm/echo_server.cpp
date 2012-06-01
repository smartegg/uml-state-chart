#include <iostream>
#include <string>
#include <boost/bind.hpp>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include "CircleBuf.h"

using boost::asio::ip::tcp;

namespace  {

class Session {
  public:
    Session(boost::asio::io_service& io_service)
      : socket_(io_service),
        buf_(5) {

    }

    virtual ~Session() {}


    tcp::socket& socket() {
      return  socket_;
    }



    void start() {
      start_read();
    }


    void handle_read(const boost::system::error_code& error,
                     size_t bytes_transferred) {
      if (!error) {
        std::cout << "bytes_readed: " << bytes_transferred << std::endl;
        buf_.adjustAfterAppend(bytes_transferred);

        if (!buf_.isEmpty()) {
          start_write();
          return;
        }

        start_read();

      } else {
        delete this;

        if (error == boost::asio::error::eof) {
          puts("client shutdown");
        }

        if (error != boost::asio::error::eof) {
          puts("handle_read error");
          throw boost::system::system_error(error);
        }
      }

    }

    void handle_write(const boost::system::error_code& error,
                      size_t bytes_transferred) {
      if (!error) {
        std::cout << "bytes_write: " << bytes_transferred << std::endl;
        buf_.adjustAfterPop(bytes_transferred);

        if (buf_.isEmpty()) {
          start_read() ;
          return;
        }

        start_write();

      } else {
   //     puts("handle_write error");
        delete this;
        throw boost::system::system_error(error);
      }
    }



  private:
    void start_read() {
      struct iovec  iov[2];
      struct iovec* p = iov;
      int cnt = 0;
      buf_.getAppendIov(p, cnt);

      std::vector<boost::asio::mutable_buffer> bufs;

      std::cout << "get free space: " << std::endl;
      for (int i = 0; i < cnt;  i++) {
        bufs.push_back(boost::asio::buffer(iov[i].iov_base, iov[i].iov_len));
        std::cout << "free space: " << i <<  "\t" << iov[i].iov_len << "\n";
      }

      if (cnt > 0) {
        socket_.async_read_some(bufs,
                                boost::bind(&Session::handle_read, this,
                                            boost::asio::placeholders::error,
                                            boost::asio::placeholders::bytes_transferred));
      }

    }

    void start_write() {
      struct iovec  iov[2];

      struct iovec* p = iov;

      int cnt = 0;

      buf_.getPopIov(p, cnt);

      std::vector<boost::asio::const_buffer> bufs;

      std::cout << "get consumed space" << std::endl;
      for (int i = 0; i < cnt;  i++) {
        bufs.push_back(boost::asio::buffer(iov[i].iov_base, iov[i].iov_len));
        std::cout << "consumed space: " << i <<  "\t" << iov[i].iov_len << "\n";
      }

      if (cnt > 0) {
        socket_.async_write_some(bufs,
                                 boost::bind(&Session::handle_write, this,
                                             boost::asio::placeholders::error,
                                             boost::asio::placeholders::bytes_transferred));
      }

    }


    Session(const Session&);
    const Session& operator=(const Session&);

    tcp::socket socket_;
    CircleBuf   buf_;
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

