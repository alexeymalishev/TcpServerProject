// BO_Exchange.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"

#include "DBInterface.h"
#include "Parser.h"
#include "../common/dllexport_types.h"

extern "C" __declspec(dllexport) void CALLBACK init()
{
  ///force DBInterface creation
  parser::CDBInterface::Instance();
}

extern "C" __declspec(dllexport) void CALLBACK close()
{
  const_cast<parser::CDBInterface&>(parser::CDBInterface::Instance()).Close();
}

__declspec(dllexport) void CALLBACK registrate_server(const std::weak_ptr<tcpserver::IServer> srv)
{

}

__declspec(dllexport) std::unique_ptr<tcpserver::IParser> CALLBACK parser_fabric()
{
  return std::unique_ptr<tcpserver::IParser>(new CParser());
}
