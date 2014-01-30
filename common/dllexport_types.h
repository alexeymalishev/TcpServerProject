#pragma once
#ifndef DLLEXPORT_TYPES_H_
#define DLLEXPORT_TYPES_H_

#include <vector>

#include "IServer.h"
#include "IParser.h"

namespace tcpserver {
  class Connection;

  typedef void (CALLBACK *Init)(void);
  typedef void (CALLBACK *Close)(void);

  typedef void (CALLBACK *RegistrateServer)(const std::weak_ptr<IServer>);
  typedef std::unique_ptr<tcpserver::IParser> (CALLBACK *ParserFabric)(void);

}

#endif  //  DLLEXPORT_TYPES_H_
