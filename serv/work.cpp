/***************************************************************************
*                                                                          *
*  work.cpp main working functions for command line processing             *
*                                                                          *
*  Copyright (C) 2013, Alexey Malishev (Moscow, Russia)                    *
*   All rights reserved.                                                   *
*                                                                          *
****************************************************************************/

#include "stdafx.h"
#include "stddef.h"

#include <iostream>
#include <string>
#include <list>
#include <boost\asio.hpp>
#include <boost\bind.hpp>

#include <boost\shared_array.hpp>

#include "report.h"
#include ".\..\common\parameters.h"
#include "stdfunc.h"
#include "work.h"

#include "server.hpp"

#include ".\..\common\dllexport_types.h"

namespace work
{
	void SetWorkDirectory();

	HANDLE hStopEvent;
  HANDLE hConnectionsClosedEvent;
  HANDLE hEndEvent;
  HANDLE hParametersChangedEvent;

  bool bTerminate;
}

///
/// GlobalInit 
///
bool work::GlobalInit()
{	
  // All init procedures in Init()
	return true;
}  // bool work::GlobalInit()

///
/// GlobalEnd stop operation
///
void work::GlobalEnd()
{
}

///
/// Init 
///
bool work::Init(DWORD* pErr, DWORD* pSpecErr)
{	
	*pErr = 0;
	*pSpecErr = 0;	

	if (!report::RegisterAsSource())
	{
		*pErr = GetLastError();
		return false;
	}

	hStopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	if (!hStopEvent)
	{
		*pErr = GetLastError();
		report::Deregister();
		return false;
	}

  hEndEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

  if (!hEndEvent)
  {
    *pErr = GetLastError();
    report::Deregister();
    return false;
  }

  hParametersChangedEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

  if (!hParametersChangedEvent)
  {
    *pErr = GetLastError();
    report::Deregister();
    return false;
  }

  hConnectionsClosedEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

  if (!hConnectionsClosedEvent)
  {
    *pErr = GetLastError();
    report::Deregister();
    return false;
  }

  bTerminate = false;

  return true;
} // bool work::Init(DWORD* pErr, DWORD* pSpecErr)

//
/// Run - service runing procedure
///
void work::Run()
{

  BOOST_LOG_TRIVIAL(info) << "Service starting";

  typedef std::pair<std::string, HMODULE> module_name_handle;
  std::list<module_name_handle> module_list;

  std::list<std::shared_ptr<tcpserver::Server> > srv_list;

  typedef std::pair<std::string, std::string> port_proto_pair;
  std::list<port_proto_pair> connection_list;

  // Get number of CPU 
  _SYSTEM_INFO system_info;
  GetSystemInfo(&system_info);

  while (!bTerminate){
    try{
      DWORD nNumberOfParsers = 0;
      parameters::GetParam("System\\CurrentControlSet\\Services\\TcpServer\\parameters", "NumberOfParsers", &nNumberOfParsers);

      if (0 == nNumberOfParsers) {
        throw std::exception("Parsers not defined in registry.");
      }

      std::string lpProtocol;
      std::string lpPortNumber;
      std::string lpLibraryName;
      std::string lpFunctionName;
      std::string lpCallbackName;
      std::string lpTimeOut;

      for (DWORD ii = 0; ii < nNumberOfParsers; ++ii) {
        std::stringstream lpKey;
        lpKey << "System\\CurrentControlSet\\Services\\TcpServer\\parameters\\Parser_" << ii;
        parameters::GetParam(lpKey.str(), "Protocol", &lpProtocol);
        parameters::GetParam(lpKey.str(), "PortNumber", &lpPortNumber);
        parameters::GetParam(lpKey.str(), "LibraryName", &lpLibraryName);
        parameters::GetParam(lpKey.str(), "ParserName", &lpFunctionName);
        parameters::GetParam(lpKey.str(), "CallbackName", &lpCallbackName);
        parameters::GetParam(lpKey.str(), "TimeOut", &lpTimeOut);
        int timeout = atoi(lpTimeOut.c_str());

        BOOST_LOG_TRIVIAL(info) << "Trying to attach"
          << "\n\tlibrary: " << lpLibraryName
          << "\n\tFunction: " << lpFunctionName
          << "\n\tCallback: " << lpCallbackName
          << "\n\tProtocol: " << lpProtocol
          << "\n\tPort: " << lpPortNumber
          //      << "\n\tBufferSize: " << buffer_size 
          << "\n\tTimeOut: " << timeout;

        /// check port & protocol, maybe already in list
        port_proto_pair pp(lpPortNumber, lpProtocol);
        for (auto cl_it = connection_list.begin(); cl_it != connection_list.end(); ++cl_it) {
          if ((pp.first == cl_it->first) &&
            (pp.second == cl_it->second)) {
            std::stringstream errorstr;
            errorstr << "Dublicate of port " << lpPortNumber << " Protocol " << lpProtocol;
            throw std::exception(errorstr.str().c_str());
          }
        }

        connection_list.push_back(pp);

        /// seek library, maybe it already opened
        auto module_it = module_list.begin();

        while (module_it != module_list.end())
        {
          if (module_it->first == lpLibraryName)
            break;

          ++module_it;
        }

        HMODULE hLibHandle;

        if (module_list.end() == module_it) {
          hLibHandle = LoadLibraryA(lpLibraryName.c_str());          /// open library

          if (0 == hLibHandle) {
            std::stringstream errorstr;
            errorstr << "Can't load library " << lpLibraryName;
            throw std::exception(errorstr.str().c_str());
          }

          module_list.push_back(module_name_handle(lpLibraryName, hLibHandle));

          /// Load function from library
          tcpserver::Init init =
            (tcpserver::Init)GetProcAddress(hLibHandle, "_init@0");
          if (init) {
            init();
          }
          else {
            BOOST_LOG_TRIVIAL(warning) << "Init procedure _init@0 not found in library: " << lpLibraryName;
          }

        }
        else {
          hLibHandle = module_it->second;
        }

        tcpserver::ParserFabric parser_fabric = (tcpserver::ParserFabric)GetProcAddress(hLibHandle, lpFunctionName.c_str());

        if (0 == parser_fabric) {
          std::stringstream errorstr;
          errorstr << "Can't load function: " << lpFunctionName << " in library: " << lpLibraryName;
          throw std::exception(errorstr.str().c_str());
        }

        /// Initialise the server.
        std::shared_ptr<tcpserver::Server> srv(new tcpserver::Server("0.0.0.0",
          lpPortNumber,
          system_info.dwNumberOfProcessors,
          parser_fabric,
          2048,
          boost::posix_time::seconds(timeout)));

        srv_list.push_back(std::shared_ptr<tcpserver::Server>(srv));

        tcpserver::RegistrateServer reg_srv =
          (tcpserver::RegistrateServer)GetProcAddress(hLibHandle, lpCallbackName.c_str());
        if (reg_srv) {
          reg_srv(srv);
        }
      }

      /// Run the servers until stopped.
      for (auto srv_it = srv_list.begin(); srv_it != srv_list.end(); ++srv_it) {
        srv_it->get()->Run();
      }

      report::Information(MSG_STARTED);

      HANDLE hEvs[] = { hStopEvent, hParametersChangedEvent };
      
      //wait signal for end 
      WaitForMultipleObjects(2, hEvs, false, INFINITE);

      BOOST_LOG_TRIVIAL(warning) << "Closing";

      for (auto srv_it = srv_list.begin(); srv_it != srv_list.end(); ++srv_it) {
        srv_it->get()->HandleStop();
      }

      /// Wait untill all servers stoped
      for (auto srv_it = srv_list.begin(); srv_it != srv_list.end(); ++srv_it) {
        srv_it->get()->WaitStop();
      }

      report::Information(MSG_STOPPED);
    }
    catch (const std::exception& e) {
      BOOST_LOG_TRIVIAL(error) << e.what();
      report::Fail(e.what());
      bTerminate = true;
      stdservfunc::FatalError(NO_ERROR, 0);
    }
    catch (...) {
      BOOST_LOG_TRIVIAL(error) << "Unexpected error!";
      report::Fail("Unexpected error!");
      bTerminate = true;
      stdservfunc::FatalError(NO_ERROR, 0);
    }

    srv_list.clear();
    connection_list.clear();

    /// closing opened libraries
    for (auto module_it = module_list.begin(); module_it != module_list.end(); ++module_it)
    {
      tcpserver::Close close =
        (tcpserver::Close)GetProcAddress(module_it->second, "_close@0");
      if (close) {
        close();
      }
      else
      {
        BOOST_LOG_TRIVIAL(warning) << "Close procedure _close@0 not found in library: " << module_it->first;
      }
      FreeLibrary(module_it->second);
    }

    module_list.clear();

    SetEvent(hConnectionsClosedEvent);
  }

  SetEvent(hEndEvent);

  BOOST_LOG_TRIVIAL(warning) << "Closed";
} // void work::Run()

///
/// Stop service operation
///
void work::Stop()
{
  bTerminate = true;

	SetEvent(hStopEvent);

  if (WAIT_TIMEOUT == WaitForSingleObject(hEndEvent, 10000))
  {
    BOOST_LOG_TRIVIAL(error) << "Can't stop service thread, emergency stop.";
    report::Fail("Can't stop service thread, emergency stop.");
  }
  
  CloseHandle(hStopEvent);
  CloseHandle(hEndEvent);
} // void work::Stop()

///
/// SetWorkDirectory set working directory to directory  that content executable file
///
void work::SetWorkDirectory()
{
	TCHAR buff[_MAX_PATH];
	DWORD Size = _MAX_PATH;
  
    // default Service working directory C:\WinNT\system32\
	
	GetModuleFileName(NULL, buff, Size);

	Size = lstrlen(buff);
	while (Size > 0)
	{
		if (buff[Size]==TEXT('\\')) break;
		Size--;
	}

	buff[Size] = 0;

	SetCurrentDirectory(buff);
} // void work::SetWorkDirectory()

///
/// ParametersChanged function called from SrvCfg, after parameters changed
///
void work::ParametersChanged()
{
  SetEvent(hParametersChangedEvent);

  if (WAIT_TIMEOUT == WaitForSingleObject(hConnectionsClosedEvent, 10000))
  {
    BOOST_LOG_TRIVIAL(error) << "Can't stop service thread, emergency stop.";
    report::Fail("Can't stop service thread, emergency stop.");
    bTerminate = true;
    SetEvent(hStopEvent);
    stdservfunc::FatalError(NO_ERROR, 0);
  }

  report::Information(MSG_PARAMETRS_CHANGED);
} // void work::ParametersChanged()

