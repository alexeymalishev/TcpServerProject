/***************************************************************************
*                                                                          *
*  resource.h                                                              *
*                                                                          *
*  Copyright (C) 2013, Alexey Malishev (Moscow, Russia)                    *
*   All rights reserved.                                                   *
*                                                                          *
****************************************************************************/


#pragma once

#ifndef __STDDEF_H__
#define __STDDEF_H__

#include <windows.h>
#include <tchar.h>

// internal service name
#define ServiceName TEXT("TcpServer")

// mnemonic service name
#define DisplayName TEXT("TcpServer Service")

// Event source name in Event Log
#define EventSource TEXT("TcpServerEventSource")

// Path to message file (look cmdline.cpp InstallEventLog)
#define MsgFileName TEXT("TcpServerEventMsg.dll")

#endif
