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

struct ReadOnlyState;
struct ReadOnlyWaitState;
struct WriteOnlyWaitState;
struct ReadWriteWaitState;
struct ShuttingState;
struct CloseState;

struct RegReadEvent;
struct WriteDataFullEvent;
struct WriteDataEvent;
struct ReadDataEvent;
struct ReadDataFullEvent;
struct ReadEofEvent;
struct FinishWriteAllEvent;

struct RegReadAction;
struct RegWriteAction;
struct RegReadWriteAction;
struct CloseAction;
struct RegWriteAllAction;
struct NoneAction;
struct NoneGuard;



class Session {
  public:
    enum {
      BUF_SIZE = 6
    };

    Session(boost::asio::io_service& io_service);

    virtual ~Session() ;


    tcp::socket& socket() {
      return  socket_;
    }



    void start();


    void handle_read(const boost::system::error_code& error,
                     size_t bytes_transferred);

    void handle_write(const boost::system::error_code& error,
                      size_t bytes_transferred);


    void handle_write_all(const boost::system::error_code& error);


    void start_read();

    void start_write();

    void start_write_all();

    void close();


  private:
    Session(const Session&);
    const Session& operator=(const Session&);

    void buildFSM();
    void destroyFSM();

    tcp::socket socket_;
    CircleBuf   buf_;
    StateMachine* fsm_;

    //the UML element defined here
    ReadOnlyState* readonly;
    ReadOnlyWaitState* readonly_wait;
    WriteOnlyWaitState* writeonly_wait;
    ReadWriteWaitState* readwrite_wait;
    ShuttingState* shutting;
    CloseState* sclose;

    RegReadEvent* RegRead;
    WriteDataFullEvent* WriteDataFull;
    WriteDataEvent* WriteData;
    ReadDataEvent* ReadData;
    ReadDataFullEvent* ReadDataFull;
    ReadEofEvent* ReadEof;
    FinishWriteAllEvent* FinishWriteAll;

    RegReadAction* regRead;
    RegWriteAction* regWrite;
    RegReadWriteAction* regReadWrite;
    CloseAction* aclose;
    RegWriteAllAction* regWriteAll;
    NoneAction* noneAction;
    NoneGuard* noneGuard;

    bool  readSet_;
    bool  writeSet_;
};


///define the UML states
struct ReadOnlyState : State {} ;
struct ReadOnlyWaitState : State {} ;
struct WriteOnlyWaitState : State {} ;
struct ReadWriteWaitState : State {} ;
struct ShuttingState : State {} ;
struct CloseState : State {} ;

///define the UML events
struct MyEvent : Event {
  Session* s;
};
struct RegReadEvent : MyEvent {} ;
struct WriteDataFullEvent : MyEvent {} ;
struct ReadDataFullEvent : MyEvent {} ;
struct ReadDataEvent : MyEvent {} ;
struct WriteDataEvent : MyEvent {} ;
struct ReadEofEvent : MyEvent {} ;
struct FinishWriteAllEvent : MyEvent {}  ;

///define the UML Actions
struct RegReadAction : Action {
  void run(Event* ev) {
    MyEvent* rv = dynamic_cast<MyEvent*>(ev);

    if (rv == 0) {
      exit(1);
    }

    Session* session = rv->s;
#include<typeinfo>
    std::cout << "typeid: " << typeid(*rv).name() << std::endl;
    session->start_read();
  }
} ;


struct RegWriteAction : Action {
  void run(Event* ev) {
    MyEvent* rv = dynamic_cast<MyEvent*>(ev);

    if (rv == 0) {
      exit(1);
    }

    Session* session = rv->s;
    session->start_write();
  }
} ;

struct RegReadWriteAction : Action {
  void run(Event* ev) {
    MyEvent* rv = dynamic_cast<MyEvent*>(ev);

    if (rv == 0) {
      exit(1);
    }

    Session* session = rv->s;
    session->start_read();
    session->start_write();
  }
} ;


struct CloseAction : Action {
  void run(Event* ev) {
    MyEvent* rv = dynamic_cast<MyEvent*>(ev);

    if (rv == 0) {
      exit(1);
    }

    Session* session = rv->s;
    session->close();
    puts("fsm close");
    delete this;
  }
} ;

struct RegWriteAllAction : Action {
  void run(Event* ev) {
    MyEvent* rv = dynamic_cast<MyEvent*>(ev);

    if (rv == 0) {
      exit(1);
    }

    Session* session = rv->s;
    session->start_write_all();
  }
};


struct NoneAction : Action {
  void run(Event* ev) {
  }
};

struct NoneGuard : Guard {
  bool run() {
    return true;
  }
};


Session::~Session() {
  destroyFSM();
}

void Session::destroyFSM() {
  delete readonly;
  delete readonly_wait;
  delete writeonly_wait;
  delete readwrite_wait;
  delete shutting;
  delete sclose;

  delete RegRead;
  delete WriteDataFull;
  delete WriteData;
  delete ReadData;
  delete ReadDataFull;
  delete ReadEof;
  delete FinishWriteAll;

  delete regRead;
  delete regWrite;
  delete regReadWrite;
  delete aclose;
  delete regWriteAll;
  delete noneAction;
  delete noneGuard;

  delete fsm_;
}


void Session::buildFSM() {
  readonly = new ReadOnlyState();
  readonly_wait = new ReadOnlyWaitState();
  writeonly_wait = new WriteOnlyWaitState();
  readwrite_wait = new ReadWriteWaitState();
  shutting = new ShuttingState();
  sclose = new CloseState();

  RegRead = new RegReadEvent();
  WriteDataFull = new WriteDataFullEvent();
  WriteData = new WriteDataEvent();
  ReadData = new ReadDataEvent();
  ReadDataFull = new ReadDataFullEvent();
  ReadEof = new ReadEofEvent();
  FinishWriteAll = new FinishWriteAllEvent();

  regRead = new RegReadAction();
  regWrite = new RegWriteAction();
  regReadWrite = new RegReadWriteAction();
  aclose = new CloseAction();
  regWriteAll = new RegWriteAllAction();
  noneAction = new NoneAction();
  noneGuard = new NoneGuard();

  RegRead->s = this;
  WriteDataFull->s  = this;
  ReadDataFull->s = this;
  ReadData->s = this;
  WriteData->s = this;
  ReadEof->s = this;
  FinishWriteAll->s = this;




  Transition  transitions[] = {
    {readonly, readonly_wait, RegRead, regRead, noneGuard},

    {readonly_wait, writeonly_wait, ReadDataFull, regWrite, noneGuard},
    {readonly_wait, sclose, ReadEof, aclose, noneGuard},
    {readonly_wait, readwrite_wait, ReadData, regReadWrite, noneGuard},
    {readonly_wait, readonly_wait, WriteData, noneAction, noneGuard},


    {readwrite_wait, readwrite_wait, WriteData, regWrite, noneGuard},
    {readwrite_wait, readwrite_wait, ReadData, regRead, noneGuard},
    {readwrite_wait, writeonly_wait, ReadDataFull, regWrite, noneGuard},
    {readwrite_wait, shutting, ReadEof, regWriteAll, noneGuard },
    {readwrite_wait, readonly_wait, WriteDataFull, regRead, noneGuard },


    {writeonly_wait, readonly_wait, WriteDataFull, regRead, noneGuard},
    {writeonly_wait, readwrite_wait, WriteData, regWrite, noneGuard},
    {writeonly_wait, writeonly_wait, ReadData, noneAction, noneGuard},

    {shutting, shutting, WriteData, regWrite, noneGuard},
    {shutting, sclose, WriteDataFull, aclose, noneGuard}
  };

  fsm_ = new StateMachine(transitions, 15, *readonly);
}



Session::Session(boost::asio::io_service& io_service)
  : socket_(io_service),
    buf_(BUF_SIZE),
    readSet_(false),
    writeSet_(false){
  puts("Create a new Session");

  buildFSM();
  fsm_->start();
  std::cout << __LINE__ << "\t" << fsm_->currentState().name() << std::endl;
}

void Session::start() {
  fsm_->processEvent(*RegRead);
  std::cout << __LINE__ << "\t" << fsm_->currentState().name() << std::endl;
}


void Session::handle_read(const boost::system::error_code& error,
                          size_t bytes_transferred) {
  std::cout << "invoke_read: " << bytes_transferred << std::endl;
  std::cout << __LINE__ << "\t" << fsm_->currentState().name() << std::endl;
  readSet_ = false;

  if (!error) {
    buf_.adjustAfterAppend(bytes_transferred);

    if (!buf_.isFull()) {
      fsm_->processEvent(*ReadData);

    } else {
      fsm_->processEvent(*ReadDataFull);
    }

    std::cout << __LINE__ << "\t" << fsm_->currentState().name() << std::endl;

  } else {

    if (error == boost::asio::error::eof) {
      fsm_->processEvent(*ReadEof);
    }

    if (error != boost::asio::error::eof) {
      delete this;
      puts("handle_read error");
      throw boost::system::system_error(error);
    }
  }

}

void Session::handle_write(const boost::system::error_code& error,
                           size_t bytes_transferred) {
  std::cout << "invoke_write: " << bytes_transferred << std::endl;
  std::cout << __LINE__ << "\t" << fsm_->currentState().name() << std::endl;
  writeSet_ = false;
  if (!error) {
    buf_.adjustAfterPop(bytes_transferred);

    if (buf_.isEmpty()) {
      fsm_->processEvent(*WriteDataFull);

    } else {
      fsm_->processEvent(*WriteData);
    }

    std::cout << __LINE__ << "\t" << fsm_->currentState().name() << std::endl;

  } else {
    delete this;
    throw boost::system::system_error(error);
  }
}

void Session::handle_write_all(const boost::system::error_code& error) {
  std::cout << "invoke_write_all: "  << std::endl;
  std::cout << __LINE__ << "\t" << fsm_->currentState().name() << std::endl;

  if (!error) {
    fsm_->processEvent(*FinishWriteAll);
    std::cout << __LINE__ << "\t" << fsm_->currentState().name() << std::endl;

  } else {
    delete this;
    throw boost::system::system_error(error);
  }
}


void Session::start_read() {
  if (readSet_)
    return;
  readSet_ = true;

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
  if (writeSet_)
    return;

  writeSet_ = true;
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



void Session::start_write_all() {
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
    boost::asio::async_write(socket_, bufs,
                             boost::bind(&Session::handle_write_all, this,
                                         boost::asio::placeholders::error));
  }

}

void Session::close() {
  std::cout << "Session close" << std::endl;
  socket_.close();
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
      puts("get a new accept");
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

