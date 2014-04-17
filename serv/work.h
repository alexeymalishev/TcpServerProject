/***************************************************************************
*                                                                          *
*  work.h main working functions for command line processing               *
*                                                                          *
*  Copyright (C) 2013, Alexey Malishev (Moscow, Russia)                    *
*   All rights reserved.                                                   *
*                                                                          *
****************************************************************************/

#pragma once

#ifndef __WORK_H__
#define __WORK_H__

namespace work
{
  bool GlobalInit();
	void GlobalEnd();

	bool Init(DWORD* pErr, DWORD* pSpecErr);

	void Run();
	void Stop();

  void SetWorkDirectory();
  void ParametersChanged();
};


#endif