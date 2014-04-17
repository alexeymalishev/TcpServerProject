/***************************************************************************
*                                                                          *
*  Server.hpp: header file of connections server.                          *
*                                                                          *
*  Copyright (C) 2013, Alexey Malishev (Moscow, Russia)                    *
*   All rights reserved.                                                   *
*                                                                          *
****************************************************************************/

#pragma once

#ifndef __TCP_SERVER_SERVER_HPP__
#define __TCP_SERVER_SERVER_HPP__

#include "stdafx.h"

#include <string>
#include <vector>
#include <thread>

#include <boost\asio.hpp>
#include <boost\shared_ptr.hpp>

#include "connection.hpp"
#include ".\..\common\IServer.h"

namespace tcpserver {

/// The top-level class of the HTTP server.
class Server 
  : public IServer
{
public:
  /// Construct the server to listen on the specified TCP address and port, and
  /// serve up files from the given directory.
  explicit Server(const std::string& address,
                  const std::string& port,
                  std::size_t thread_pool_size,
                  ParserFabric parser_fabric,
                  size_t out_buffers_size,
                  boost::posix_time::seconds connection_timeout);

  /// wait while all thread stoped
  void WaitStop();

  /// Run the server's io_service loop.
  void Run();

  /// Closing server. Handle a request to stop the server.
  void HandleStop();

  /// Send message by connection pointer callback, check connection existance
  virtual bool DataSend(boost::weak_ptr<Connection> connection_ptr, 
                         tcpserver::outbuffer_ptr out_buffer_list);

  /// close connection by pointer callback, check connection existance
  virtual bool CloseConnection(boost::weak_ptr<Connection> connection_ptr);

  /// try to open connection from library
  virtual void OpenConnection(const open_connection_callback callback,
                               const std::string &address,
                               const std::string &port,
                               const int &connection_id);

private:
  /// Initiate an asynchronous accept operation.
  void StartAccept();

  /// Handle completion of an asynchronous accept operation.
  void HandleAccept(const boost::system::error_code &e);

  /// Handle a coonection request from parser
  void HandleConnect(ConnectionPtr con_ptr,
                      const open_connection_callback callback,
                      const std::string &address,
                      const std::string &port,
                      const int &connection_id,
                      const boost::system::error_code &ec);

  /// The number of threads that will call io_service::run().
  std::size_t thread_pool_size_;

  /// The io_service used to perform asynchronous operations.
  boost::asio::io_service io_service_;

  /// The signal_set is used to register for process termination notifications.
  boost::asio::signal_set signals_;

  /// Acceptor used to listen for incoming connections.
  boost::asio::ip::tcp::acceptor acceptor_;

  /// The next connection to be accepted.
  ConnectionPtr new_accept_connection_;

  /// Incoming data processing parser object generator function
  ParserFabric parser_fabric_;

  /// Input and output Data buffers size
  size_t out_buffers_size_;

  /// inactive connection close timeout
  boost::posix_time::seconds connection_timeout_;

  /// server data processing thread pool
  std::vector<boost::shared_ptr<std::thread> > threads_pool_;
};

} // namespace tcpserver


#endif // __TCP_SERVER_SERVER_HPP__
