/***************************************************************************
*                                                                          *
*  stdfunc.h represent service standart functions                          *
*                                                                          *
*  Copyright (C) 2013, Alexey Malishev (Moscow, Russia)                    *
*   All rights reserved.                                                   *
*                                                                          *
****************************************************************************/


#pragma once

#ifndef __STDFUNC_H__
#define __STDFUNC_H__

namespace stdservfunc
{
	void WINAPI ServiceMain(DWORD dwArgc, LPTSTR *psArgv);
	void FatalError(DWORD Err, DWORD SpecErr);
}

#endif // __STDFUNC_H__
