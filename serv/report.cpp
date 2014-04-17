/***************************************************************************
*                                                                          *
*  report.cpp provide interface with event log                             *
*                                                                          *
*  Copyright (C) 2013, Alexey Malishev (Moscow, Russia)                    *
*   All rights reserved.                                                   *
*                                                                          *
****************************************************************************/

#include "stddef.h"
#include "stdafx.h"

#include "report.h"
#include "mymsg.h"

namespace report
{
	HANDLE hSource = NULL;
}

///
/// RegisterAsSource get event log descriptor
///
bool report::RegisterAsSource()
{	
	if (hSource!=NULL) return true;

	hSource = RegisterEventSource(NULL, EventSource);

	if (hSource==NULL) return false;
	return true;
} // bool report::RegisterAsSource()

///
/// Deregister close event log descriptor
///
void report::Deregister()
{
	DeregisterEventSource(hSource);
	hSource = NULL;
} // void report::Deregister()


///
/// Information add predefined message to event log
///
void report::Information(DWORD code)
{
  if (hSource == NULL)
  {
    return;
  }

  ReportEvent(
    hSource,
    EVENTLOG_SUCCESS,
    0,
    code,
    NULL,
    0,
    0,
    NULL,
    NULL);
} // void report::Information(DWORD code)

///
///  Fail add error message to event log
///
void report::Fail(const char* text)
{
	if (hSource==NULL)
	{
		return;
	}

  const size_t buffer_size = 1024;

  TCHAR buff[buffer_size];

  TCHAR* buffptr[] = { buff };

  size_t convertedChars = 0;
  mbstowcs_s(&convertedChars, buff, buffer_size, text, _TRUNCATE);

	ReportEvent(
		hSource, 
		EVENTLOG_ERROR_TYPE,
		0,
		MSG_FAIL,
		NULL,
		1,
		0,
		(LPCTSTR*) buffptr,
		NULL);
} // void report::Fail(const char* text)
