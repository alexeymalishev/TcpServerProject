/***************************************************************************
*                                                                          *
*  Parser Interface file                                                   *
*                                                                          *
*  Copyright (C) 2013, Alexey Malishev (Moscow, Russia)                    *
*   All rights reserved.                                                   *
*                                                                          *
****************************************************************************/

#pragma once
#ifndef IPARSER_H
#define IPARSER_H

#include <boost\weak_ptr.hpp>
#include <boost\noncopyable.hpp>

namespace tcpserver
{
  const static int kInBufferSize = 1536;

  typedef std::shared_ptr<std::vector<char>> outbuffer_ptr;
  typedef std::array<char, kInBufferSize>::iterator InArrayIterator;

  class Connection;

  class IParser
    : private boost::noncopyable
  {
  public:
    IParser() {};
    virtual ~IParser() {};
    virtual tcpserver::outbuffer_ptr Parse(boost::weak_ptr<tcpserver::Connection> connection_ptr,
                                          tcpserver::InArrayIterator in_data_start,
                                          tcpserver::InArrayIterator in_data_end) = 0;
  }; // class IParser

} // namespace tcpserver

#endif // IPARSER_H
