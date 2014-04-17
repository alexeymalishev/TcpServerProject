/***************************************************************************
*                                                                          *
*  cmdline.h functions for command line processing                         *
*  install, uninstall, help                                                *
*                                                                          *
*  Copyright (C) 2013, Alexey Malishev (Moscow, Russia)                    *
*   All rights reserved.                                                   *
*                                                                          *
****************************************************************************/

#pragma once

#ifndef __CMDLINE_H__
#define __CMDLINE_H__

namespace cmdline
{
	bool Install();
	bool Uninstall();
	void DisplayHelp();
}

#endif