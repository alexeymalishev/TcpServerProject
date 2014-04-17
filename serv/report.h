/***************************************************************************
*                                                                          *
*  report.h provide interface with event log                               *
*                                                                          *
*  Copyright (C) 2013, Alexey Malishev (Moscow, Russia)                    *
*   All rights reserved.                                                   *
*                                                                          *
****************************************************************************/

#pragma once

#ifndef __REPORT_H__
#define __REPORT_H__

#include "mymsg.h"

namespace report
{
	bool RegisterAsSource();
	void Deregister();

	void Information(DWORD code);
	void Fail(const char* text);
}

#endif