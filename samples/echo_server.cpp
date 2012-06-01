#include <iostream>
#include <string>
#include <boost/bind.hpp>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include "CircleBuf.h"
#include "../TransitionTable.hpp"
#include "../StateMachine.hpp"

using boost::asio::ip::tcp;

namespace  {

using namespace ndsl::fsm;


class Session {
  public:
    enum {
      BUF_SIZE = 6
    };

    Session(boost::asio::io_service& io_service);

    virtual ~Session() {}


    tcp::socket& socket() {
      return  socket_;
    }



    void start();


    void handle_read(const boost::system::error_code& error,
                     size_t bytes_transferred);

    void handle_write(const boost::system::error_code& error,
                      size_t bytes_transferred);



    void start_read();

    void start_write();


  private:
    Session(const Session&);
    const Session& operator=(const Session&);

    tcp::socket socket_;
    CircleBuf   buf_;
    StateMachine fsm_;
};



//define the UML states
struct InitState : State {} initState;
struct WaitReadState : State {} waitReadState;
struct WaitWriteState : State {} waitWriteState;
struct CloseState : State {} closeState;

//define the UML events
struct RdEvent : Event {} rdEvent;
struct WdEvent : Event {} wdEvent;
struct EofEvent : Event {} eofEvent;



class Session;

///define the UML Actions
struct RegReadAction : Action {
  void run(void* pData) {
    Session* session = reinterpret_cast<Session*>(pData) ;
    session->start_read();
  }
} regReadAction;


struct RegWriteAction : Action {
  void run(void* pData) {
    Session* session = reinterpret_cast<Session*>(pData) ;
    session->start_write();
  }
} regWriteAction;

struct NoneAction : Action {
  void run(void* pData) {
  }
} noneAction;

struct NoneGuard : Guard {
  bool run() {
    return true;
  }
} noneGuard;


Transition  transitions[] = {
  {initState, waitReadState, rdEvent, regReadAction, noneGuard},
  {waitReadState, closeState, eofEvent, noneAction, noneGuard},
  {waitReadState, waitWriteState, wdEvent, regWriteAction, noneGuard},
  {waitWriteState, waitWriteState, wdEvent, regWriteAction, noneGuard},
  {waitWriteState, waitReadState, rdEvent, regReadAction, noneGuard}
};



Session::Session(boost::asio::io_service& io_service)
  : socket_(io_service),
    buf_(BUF_SIZE) ,
    fsm_(transitions, 5, initState) {

}

void Session::start() {
  fsm_.start();
  fsm_.processEvent(rdEvent);
}


void Session::handle_read(const boost::system::error_code& error,
                          size_t bytes_transferred) {
  if (!error) {
    std::cout << "bytes_readed: " << bytes_transferred << std::endl;
    buf_.adjustAfterAppend(bytes_transferred);

    if (!buf_.isEmpty()) {
      start_write();
      fsm_.processEvent(wdEvent);
      return;
    }

  } else {
    delete this;

    if (error == boost::asio::error::eof) {
      fsm_.processEvent(eofEvent);
    }

    if (error != boost::asio::error::eof) {
      puts("handle_read error");
      throw boost::system::system_error(error);
    }
  }

}

void Session::handle_write(const boost::system::error_code& error,
                           size_t bytes_transferred) {
  if (!error) {
    std::cout << "bytes_write: " << bytes_transferred << std::endl;
    buf_.adjustAfterPop(bytes_transferred);

    if (buf_.isEmpty()) {
      fsm_.processEvent(rdEvent);

    } else {

      fsm_.processEvent(wdEvent);
    }

  } else {
    //     puts("handle_write error");
    delete this;
    throw boost::system::system_error(error);
  }
}



void Session::start_read() {
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

void Session::start_write() {
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

