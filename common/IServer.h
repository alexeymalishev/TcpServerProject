/***************************************************************************
*                                                                          *
*  Server Interface file                                                   *
*                                                                          *
*  Copyright (C) 2013, Alexey Malishev (Moscow, Russia)                    *
*   All rights reserved.                                                   *
*                                                                          *
****************************************************************************/

#pragma once

#ifndef ISERVER_H_
#define ISERVER_H

#include <string>
#include <boost\asio.hpp>
#include <boost\noncopyable.hpp>
#include <boost\weak_ptr.hpp>
#include <boost\shared_array.hpp>
#include <boost\system\error_code.hpp>

#include "IParser.h"

namespace tcpserver {

class Connection;

typedef void(*open_connection_callback)(const boost::weak_ptr<Connection> connection_ptr,
                                        const std::string &address,
                                        const std::string &port,
                                        const int &connection_id,
                                        const boost::system::error_code& ec);

class IServer
    : private boost::noncopyable
{
public:
	virtual ~IServer() {};

    /// Send message by connection pointer callback, check connection existance
    virtual bool DataSend(boost::weak_ptr<Connection> connection_ptr,
                           outbuffer_ptr out_buffer_list) = 0;

    /// close connection by pointer callback, check connection existance
    virtual bool CloseConnection(boost::weak_ptr<Connection> connection_ptr) =0;

    /// try to open connection from library
    virtual void OpenConnection(const open_connection_callback callback,
                                 const std::string &address,
                                 const std::string &port,
                                 const int &connection_id) = 0;
};

} // namespace tcpserver {

#endif // #define ISERVER_H