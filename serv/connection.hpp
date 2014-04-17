/***************************************************************************
*                                                                          *
*  connection.hpp connection processing module                             *
*                                                                          *
*  Copyright (C) 2013, Alexey Malishev (Moscow, Russia)                    *
*   All rights reserved.                                                   *
*                                                                          *
****************************************************************************/

#pragma once

#ifndef __TCP_SERVER_CONNECTION_HPP__
#define __TCP_SERVER_CONNECTION_HPP__

#include "stdafx.h"

#include <boost\asio.hpp>
#include <array>
#include <boost\noncopyable.hpp>
#include <boost\shared_ptr.hpp>
#include <boost\shared_array.hpp>
#include <boost\enable_shared_from_this.hpp>
#include ".\..\common\dllexport_types.h"
#include ".\..\common\IParser.h"
#include ".\..\common\IServer.h"

namespace tcpserver {

///
/// Represents a single connection from a client.
///
class Connection
  : public boost::enable_shared_from_this<Connection>
  , private boost::noncopyable
{
public:
  /// Construct a connection with the given io_service.
  explicit Connection(boost::asio::io_service& io_service,
              ParserFabric parser_fabric,
              size_t out_buffer_size, boost::posix_time::seconds connection_timeout);

  /// Get the socket associated with the connection.
  boost::asio::ip::tcp::socket& GetSocket();

  /// Start the first asynchronous operation for the connection.
  void Start();

  /// Asyng send data to host
  void AsyncSend(outbuffer_ptr& outbuffer);

  /// close connection
  void Close();

private:
  /// Handle completion of a read operation.
  void HandleRead(const boost::system::error_code& e,
          std::size_t bytes_transferred);

  /// Handle completion of a write operation.
  void HandleWrite(outbuffer_ptr& /*out_buffer_list*/,
          std::size_t /*bytes_transferred*/,
          const boost::system::error_code& e);

  /// Handle for check connection activity and close dead connection
  void HandleActivityCheck(const boost::system::error_code& e);

  /// Strand to ensure the connection's handlers are not called concurrently.
  boost::asio::io_service::strand strand_;

  /// Socket for the connection.
  boost::asio::ip::tcp::socket socket_;

  /// out_buffers size
  size_t out_buffer_size_;

  /// Buffer for incoming data.
  std::array<char, kInBufferSize> input_buffer_;

  /// The parser for the incoming data
  std::unique_ptr<IParser> parser_;

  /// Connection activity flag, reset by timer, set after activity
  bool activity_;

  /// Timer for dead connection sweeping
  boost::asio::deadline_timer dead_timer_;

  /// temporary parameter connection timeout
  boost::posix_time::seconds connection_timeout_;
};

typedef boost::shared_ptr<Connection> ConnectionPtr;

} // namespace tcpserver


#endif // __TCP_SERVER_CONNECTION_HPP__
