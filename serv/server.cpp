/***************************************************************************
*                                                                          *
*  Server.cpp: source file of connections server.                          *
*                                                                          *
*  Copyright (C) 2013, Alexey Malishev (Moscow, Russia)                    *
*   All rights reserved.                                                   *
*                                                                          *
****************************************************************************/

#include "server.hpp"
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <vector>

namespace tcpserver 
{

Server::Server(const std::string& address, const std::string& port,
    std::size_t thread_pool_size, ParserFabric parser_fabric,
    size_t out_buffers_size, boost::posix_time::seconds connection_timeout)
  : IServer()
  , thread_pool_size_(thread_pool_size)
  , signals_(io_service_)
  , acceptor_(io_service_)
  , new_accept_connection_()
  , parser_fabric_(parser_fabric)
  , out_buffers_size_(out_buffers_size)
  , connection_timeout_(connection_timeout)
{
  // Register to handle the signals that indicate when the server should exit.
  // It is safe to register for the same signal multiple times in a program,
  // provided all registration for the specified signal is made through Asio.
  signals_.add(SIGINT);
  signals_.add(SIGTERM);
#if defined(SIGQUIT)
  signals_.add(SIGQUIT);
#endif // defined(SIGQUIT)
  signals_.async_wait(boost::bind(&Server::HandleStop, this));

  // Open the acceptor with the option to reuse the address (i.e. SO_REUSEADDR).
  boost::asio::ip::tcp::resolver resolver(io_service_);
  boost::asio::ip::tcp::resolver::query query(address, port);
  boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve(query);
  acceptor_.open(endpoint.protocol());
  acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
  acceptor_.bind(endpoint);
  acceptor_.listen(boost::asio::socket_base::max_connections);

  StartAccept();
} // Server::Server

void Server::WaitStop()
{
  // Wait for all threads in the pool to exit.
  for (std::size_t i = 0; i < threads_pool_.size(); ++i)
    threads_pool_[i]->join();
} // void Server::WaitStop()

void Server::Run()
{
  // Create a pool of threads to run all of the io_services.
  for (std::size_t i = 0; i < thread_pool_size_; ++i)
  {
    boost::shared_ptr<std::thread> thread(new std::thread(
          boost::bind(&boost::asio::io_service::run, &io_service_)));
    threads_pool_.push_back(thread);
  }
} // void Server::Run()

void Server::StartAccept()
{
  new_accept_connection_.reset(new Connection(io_service_, parser_fabric_, out_buffers_size_, connection_timeout_));
  acceptor_.async_accept(new_accept_connection_->GetSocket(),
      boost::bind(&Server::HandleAccept, this,
        boost::asio::placeholders::error));
} // void Server::StartAccept()

void Server::HandleAccept(const boost::system::error_code& e)
{
  if (!e)
  {
    new_accept_connection_->Start();
  }
  else
  {
     BOOST_LOG_TRIVIAL(warning) << e.message();
  }

  StartAccept();
} // void Server::HandleAccept(const boost::system::error_code& e)

void Server::HandleStop()
{
  io_service_.stop();
}

///
///  request to send data connection from library
///

bool Server::DataSend(boost::weak_ptr<Connection> connection_ptr,
                       tcpserver::outbuffer_ptr out_buffer_list)
{
  if(!connection_ptr.expired())
  {
    ConnectionPtr connection = connection_ptr.lock();

    if(connection) 
    {
      connection->AsyncSend(out_buffer_list);
      return true;
    }
  }
  return false;
}

///
///  request to close connection from library
///

bool Server::CloseConnection(boost::weak_ptr<Connection> connection_ptr)
{
  if(!connection_ptr.expired()) 
  {
    ConnectionPtr connection = connection_ptr.lock();

    if(connection) 
    {
      connection->Close();
      return true;
    }
  }

  return false;
} // bool Server::CloseConnection

///
/// try to open connection request from DLL
///
void Server::OpenConnection(const open_connection_callback callback,
                             const std::string &address,
                             const std::string &port,
                             const int &connection_id)
{
    boost::asio::ip::tcp::resolver resolver(io_service_);
    boost::asio::ip::tcp::resolver::query query(address, port);

    ConnectionPtr con_ptr(new Connection(io_service_, parser_fabric_, out_buffers_size_, connection_timeout_));

    boost::asio::async_connect(con_ptr->GetSocket(),
                               resolver.resolve(query),
                               boost::bind(&Server::HandleConnect,
                                           this,
                                           con_ptr,
                                           callback,
                                           address,
                                           port,
                                           connection_id,
                                           boost::asio::placeholders::error
                                          )
                              );

}  // bool Server::OpenConnection

void Server::HandleConnect(ConnectionPtr con_ptr,
                            const open_connection_callback callback,
                            const std::string &address,
                            const std::string &port,
                            const int &connection_id,
                            const boost::system::error_code &ec)
{
    if (!ec)
    {
        con_ptr->Start();
    }
    else
    {
        BOOST_LOG_TRIVIAL(warning) << ec.message();
    }

    // callback to parser, report to DLL for connection
    if (nullptr != callback)
    {
        callback(con_ptr, address, port, connection_id, ec);
    }
} // void Server::HandleConnect

} // namespace tcpserver
