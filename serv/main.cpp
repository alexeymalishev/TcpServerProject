
#include "stddef.h"
#include "stdafx.h"

#include "stdfunc.h"
#include "cmdline.h"
#include "work.h" 
#include "report.h"

int _tmain(int argc, TCHAR* argv[])
{
  work::SetWorkDirectory();

  boost::log::add_common_attributes();
  boost::log::add_file_log
    (
    boost::log::keywords::file_name = "serv_%N.log",
    boost::log::keywords::rotation_size = 10 * 1024 * 1024,
    boost::log::keywords::time_based_rotation = boost::log::sinks::file::rotation_at_time_point(0, 0, 0),
    boost::log::keywords::auto_flush = true,
    boost::log::keywords::format =
    (
      boost::log::expressions::stream
        << boost::log::expressions::format_date_time< boost::posix_time::ptime >("TimeStamp", "%Y-%m-%d %H:%M:%S")
        << ": <" << boost::log::trivial::severity
        << "> " << boost::log::expressions::smessage
    )
    );

  BOOST_LOG_TRIVIAL(info) << TEXT("Start process");

  work::SetWorkDirectory();

	if (argc > 1)
	{
		// lstricmp - сравнение без учёта регистра. 
		if (lstrcmpi(argv[1], TEXT("/install"))==0)
		{
			if (cmdline::Install())
			{
				report::RegisterAsSource();
        report::Information(MSG_INSTALL);
				report::Deregister();
			}
		}
		else if (lstrcmpi(argv[1], TEXT("/uninstall"))==0)
		{
			cmdline::Uninstall();
		}
		else
		{
			cmdline::DisplayHelp();
		}
		return 0;
	}

    work::GlobalInit();
	
	SERVICE_TABLE_ENTRY steTable[] = 
	{
		{ServiceName, stdservfunc::ServiceMain},
		{NULL, NULL}
	};

	StartServiceCtrlDispatcher(steTable);

    work::GlobalEnd();
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////