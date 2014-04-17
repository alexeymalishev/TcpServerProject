/***************************************************************************
*                                                                          *
*  connection.cpp  connection processing module                            *
*                                                                          *
*  Copyright (C) 2013, Alexey Malishev (Moscow, Russia)                    *
*   All rights reserved.                                                   *
*                                                                          *
****************************************************************************/

#include "connection.hpp"
#include <vector>
#include <boost/bind.hpp>



namespace tcpserver {

  class foo : public IParser
  {
  public:
    foo() {};
    virtual ~foo() {};

    virtual outbuffer_ptr Parse(boost::weak_ptr<Connection> connection_ptr,
      InArrayIterator in_data_start,
      InArrayIterator in_data_end) {
      return outbuffer_ptr();
    };
  };

  Connection::Connection(boost::asio::io_service& io_service, ParserFabric parser_fabric,
    size_t out_buffer_size, boost::posix_time::seconds connection_timeout)
  : strand_(io_service)
  , socket_(io_service)
  , out_buffer_size_(out_buffer_size)
  , parser_(parser_fabric())
  , activity_(false)
  , dead_timer_(io_service)
  , connection_timeout_(connection_timeout)
{
  if (!parser_) {
    throw std::exception("Can't create parser object\n");
  }
}

boost::asio::ip::tcp::socket& Connection::GetSocket()
{
  return socket_;
}

void Connection::Start()
{
	boost::asio::ip::tcp::no_delay option(true);
	socket_.set_option(option);

	dead_timer_.expires_from_now(connection_timeout_);

	dead_timer_.async_wait(
	  strand_.wrap(
	    boost::bind(&Connection::HandleActivityCheck, shared_from_this(),
	      boost::asio::placeholders::error)));

	socket_.async_read_some(boost::asio::buffer(input_buffer_),
      strand_.wrap(
        boost::bind(&Connection::HandleRead, shared_from_this(),
          boost::asio::placeholders::error,
          boost::asio::placeholders::bytes_transferred)));
}

void Connection::HandleRead(const boost::system::error_code& e,
    std::size_t bytes_transferred)
{
  if (!e)
  {
    /// set activity flag
    activity_ = true;

  try
    {
      outbuffer_ptr outbuffer = parser_->Parse(shared_from_this(), 
                                             input_buffer_.begin(),
                                             input_buffer_.begin() + bytes_transferred);

      socket_.async_read_some(boost::asio::buffer(input_buffer_),
        strand_.wrap(boost::bind(&Connection::HandleRead, shared_from_this(),
        boost::asio::placeholders::error,
        boost::asio::placeholders::bytes_transferred)));

      if (!outbuffer->empty())
      {
        socket_.async_send(boost::asio::buffer(*outbuffer),
          strand_.wrap(boost::bind(&Connection::HandleWrite, shared_from_this(),
          outbuffer, boost::asio::placeholders::bytes_transferred,
          boost::asio::placeholders::error)));
      }
    }
    catch (std::exception& e)
    {
      BOOST_LOG_TRIVIAL(error) << e.what();
      // close connection
      dead_timer_.cancel();
    }
    catch (...)
    {
      BOOST_LOG_TRIVIAL(warning) << "unexpected exception when parse data in Connection::handle_read";
      // close connection
      dead_timer_.cancel();
    }
  }
  else
  {
    BOOST_LOG_TRIVIAL(warning) << e.message();

	  //cancel timer for free connection pointer
	  dead_timer_.cancel();

    // If an error occurs then no new asynchronous operations are started. This
    // means that all shared_ptr references to the connection object will
    // disappear and the object will be destroyed automatically after this
    // handler returns. The connection class's destructor closes the socket.
  }
}

void Connection::HandleWrite(outbuffer_ptr& /*out_buffer_list*/,
          std::size_t /*bytes_transferred*/,
          const boost::system::error_code& e)
{
  if (!e)
  {
    // Initiate graceful connection closure.
    boost::system::error_code ignored_ec;
  //   socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
  } else {
     BOOST_LOG_TRIVIAL(warning) << e.message();
  }
}

void Connection::HandleActivityCheck(const boost::system::error_code& e)
{
	if (!e)
	{
		if (activity_)
		{
			activity_ = false;
			//dead_timer_.expires_at(dead_timer_.expires_at() + connection_timeout_);
			dead_timer_.expires_from_now(connection_timeout_);

			dead_timer_.async_wait(
				strand_.wrap(boost::bind(&Connection::HandleActivityCheck, shared_from_this(),
				boost::asio::placeholders::error)));

		} else {
			// Initiate graceful connection closure.
			BOOST_LOG_TRIVIAL(warning) << e.message();
			boost::system::error_code ignored_ec;
			socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
		}
	} else {
		BOOST_LOG_TRIVIAL(warning) << e.message();
	}
}


void Connection::AsyncSend(outbuffer_ptr& outbuffer)
{
  if (!outbuffer->empty()) {
    socket_.async_send(boost::asio::buffer(*outbuffer),
      strand_.wrap(
      boost::bind(&Connection::HandleWrite, shared_from_this(),
      outbuffer, boost::asio::placeholders::bytes_transferred,
      boost::asio::placeholders::error)));
  }
}

void Connection::Close()
{
//      BOOST_LOG_TRIVIAL(warning) << e.message();
		boost::system::error_code ignored_ec;
		socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
}

} // namespace tcpserver

