/**
 * @file echo_client.cpp
 * @brief  a sync echo-client
 * @author smartegg<lazysmartegg@gmail.com>
 * @version 1.0
 * @date Tue, 29 May 2012 16:14:08
 * @copyright Copyright (C) 2012 smartegg<lazysmartegg@gmail.com>
 */
#include <iostream>
#include <boost/asio.hpp>
#include <boost/array.hpp>

using boost::asio::ip::tcp;


int main(int argc, char const* argv[]) {
  try {
    if (argc != 3) {
      std::cerr << "Usage: client<host> <port>" << "\n";
      return 1;
    }

    boost::asio::io_service io_service;
    tcp::resolver resolver(io_service);
    tcp::resolver::query query(argv[1], argv[2]);
    tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
    tcp::resolver::iterator end;

    tcp::socket socket(io_service);

    boost::system::error_code error = boost::asio::error::host_not_found;

    while (error && endpoint_iterator != end) {
      socket.close();
      socket.connect(*endpoint_iterator, error);
    }

    if (error) {
      throw boost::system::system_error(error);
    }

    boost::array<char, 1024> buf;

    for (;;) {
      boost::system::error_code error;
      std::cin.getline(buf.data(), 1024);
      int total_write = strlen(buf.data()) + 1;
      boost::asio::write(socket, boost::asio::buffer(buf, total_write), error);

      if (error) {
        throw boost::system::system_error(error);
      }

      boost::asio::read(socket, boost::asio::buffer(buf, total_write), error);

      if (error == boost::asio::error::eof) {
        break;
      } else if (error) {
        throw boost::system::system_error(error);
      }

      std::cout << buf.data() << "\n";

    }

  } catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}


