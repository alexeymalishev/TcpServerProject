// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
  case DLL_PROCESS_ATTACH:
  {
    boost::log::add_common_attributes();
    boost::log::add_file_log
      (
      boost::log::keywords::file_name = "BO_Ex_%N.log",
      boost::log::keywords::rotation_size = 10 * 1024 * 1024,
      boost::log::keywords::time_based_rotation = boost::log::sinks::file::rotation_at_time_point(0, 0, 0),
      boost::log::keywords::auto_flush = true,
      // This makes the sink to write log records that look like this:
      // YYYY-MM-DD HH:MI:SS: <normal> A normal severity message
      // YYYY-MM-DD HH:MI:SS: <error> An error severity message
      boost::log::keywords::format =
      (
      boost::log::expressions::stream
      << boost::log::expressions::format_date_time< boost::posix_time::ptime >("TimeStamp", "%Y-%m-%d %H:%M:%S")
      << ": <" << boost::log::trivial::severity
      << "> " << boost::log::expressions::smessage
      )
      );

    BOOST_LOG_TRIVIAL (info) << "Start\n";

    break;
  }
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

