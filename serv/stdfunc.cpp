/***************************************************************************
*                                                                          *
*  stdfunc.cpp represent service standart functions                        *
*                                                                          *
*  Copyright (C) 2013, Alexey Malishev (Moscow, Russia)                    *
*   All rights reserved.                                                   *
*                                                                          *
****************************************************************************/

#include "stddef.h"
#include "stdafx.h"

#include "work.h"
#include "stdfunc.h"

#include <thread>

// Message sended from SrvCfg after parameters changed
#define PARAMETERS_CHANGED		129

namespace stdservfunc
{
	HANDLE eSendPending;
  std::thread hSendThread;

	SERVICE_STATUS_HANDLE ssHandle; 
	SERVICE_STATUS sStatus;
	
	void WINAPI ServiceHandler(DWORD dwCode);
	
	void BeginSendPending(DWORD dwPendingType);
	void EndSendPending();

	DWORD WINAPI SendPending(DWORD dwState);
};

///
/// ServiceMain - service entry point
///
void WINAPI stdservfunc::ServiceMain(DWORD dwArgc, LPTSTR *psArgv)
{
	ssHandle = RegisterServiceCtrlHandler(ServiceName, ServiceHandler);

	sStatus.dwCheckPoint = 0;
	sStatus.dwWaitHint = 0;
	
	sStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP 
							| SERVICE_ACCEPT_SHUTDOWN
              /*| SERVICE_ACCEPT_PAUSE_CONTINUE*/;	
	sStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;	
	sStatus.dwWin32ExitCode = NOERROR;
	sStatus.dwServiceSpecificExitCode = 0;

	DWORD Err, SpecErr;

	BeginSendPending(SERVICE_START_PENDING);

	if (!work::Init(&Err, &SpecErr))
	{
		EndSendPending();

		sStatus.dwCurrentState = SERVICE_STOPPED;
		sStatus.dwWin32ExitCode = Err;
		sStatus.dwServiceSpecificExitCode = SpecErr;

		SetServiceStatus(ssHandle, &sStatus);

		return;
	}

	EndSendPending();

	sStatus.dwCurrentState = SERVICE_RUNNING;

	SetServiceStatus(ssHandle, &sStatus);

	work::Run();
}  // void WINAPI stdservfunc::ServiceMain(DWORD dwArgc, LPTSTR *psArgv)

///
/// FatalError called by working process when error occured
///
void stdservfunc::FatalError(DWORD Err, DWORD SpecErr)
{
	sStatus.dwWin32ExitCode = Err;
	sStatus.dwServiceSpecificExitCode = SpecErr;
	sStatus.dwCurrentState = SERVICE_STOPPED;

	SetServiceStatus(ssHandle, &sStatus);
} // void StdServFunc::FatalError(DWORD Err, DWORD SpecErr)

///
/// ServiceHandler process commands Stop() and ParametersChanged()
///
void WINAPI stdservfunc::ServiceHandler(DWORD dwCode)
{
    switch (dwCode)
	{
	case SERVICE_CONTROL_STOP:
	case SERVICE_CONTROL_SHUTDOWN:		
		work::Stop();
		sStatus.dwCurrentState = SERVICE_STOPPED;		
		break;	
	case PARAMETERS_CHANGED:
		// Это сообщение посылается конфигуратором
		work::ParametersChanged();		
		break;
	default:		
		break;
	}
	SetServiceStatus(ssHandle, &sStatus);
} // void WINAPI stdservfunc::ServiceHandler(DWORD dwCode)

///
/// BeginSendPending create thread for sending messages 
///
void stdservfunc::BeginSendPending(DWORD dwPendingType)
{
	eSendPending = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (!eSendPending) return;
	
  hSendThread = std::thread(SendPending, dwPendingType);
} // void stdservfunc::BeginSendPending(DWORD dwPendingType)

///
/// EndSendPending clse thread sending messages
///
void stdservfunc::EndSendPending()
{
	// Если поток и не запускался
	if (!hSendThread.joinable()) return;

	SetEvent(eSendPending);

  hSendThread.join();

	CloseHandle(eSendPending);

	eSendPending = NULL;
} // void stdservfunc::EndSendPending()

///
/// SendPending thread function send every second keepalive message 
///
DWORD WINAPI stdservfunc::SendPending(DWORD dwState)
{
	sStatus.dwCheckPoint = 0;	
	sStatus.dwWaitHint = 2000;
	sStatus.dwCurrentState = dwState;
	
	for (;;)
	{				
		if (WaitForSingleObject(eSendPending, 1000) != WAIT_TIMEOUT) break;
        sStatus.dwCheckPoint++;
        SetServiceStatus(ssHandle, &sStatus);
	}

	sStatus.dwCheckPoint = 0;
	sStatus.dwWaitHint = 0;
	return 0;
} // DWORD WINAPI stdservfunc::SendPending(LPVOID dwState)
