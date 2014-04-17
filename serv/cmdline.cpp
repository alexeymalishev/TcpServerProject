/***************************************************************************
*                                                                          *
*  cmdline.cpp functions for command line processing                       *
*  install, uninstall, help                                                *
*                                                                          *
*  Copyright (C) 2013, Alexey Malishev (Moscow, Russia)                    *
*   All rights reserved.                                                   *
*                                                                          *
****************************************************************************/

#include "stddef.h"
#include "stdafx.h"
#include "resource.h"
#include "cmdline.h"

// Keys in registry
#define SERVICE_PARENT_KEY TEXT("System\\CurrentControlSet\\Services")
#define LOG_PARENT_KEY     TEXT("System\\CurrentControlSet\\Services\\Eventlog\\Application")

#define KEY_LEN 256
#define MSG_LEN 256


namespace cmdline
{
	bool InstallService(DWORD* pErr);
	bool WriteDescription(HINSTANCE hInst, DWORD* pErr);
	bool InstallEventLog(DWORD* pErr);

	bool UninstallService(DWORD* pErr);
	bool UninstallEventLog(DWORD* pErr);
}

///
///  install service and registry eventlog.
///
bool cmdline::Install()
{
	DWORD		Err;
	TCHAR		msgString[MSG_LEN];
	HINSTANCE   hInst;
	
	hInst = GetModuleHandle(NULL);

  BOOST_LOG_TRIVIAL(info) << "Installing!";

	if (  !InstallService(&Err)
		||!WriteDescription(hInst, &Err)
		||!InstallEventLog(&Err)
		)
	{
		Uninstall();
		LoadString(hInst, Err, msgString, MSG_LEN);
		MessageBox(NULL, msgString, DisplayName, MB_OK);
		return false;
	}

	LoadString(hInst, IDS_CMDLINE_MCREATE, msgString, MSG_LEN);
	MessageBox(NULL, msgString, DisplayName, MB_OK);

	return true;
} // bool cmdline::Install()

///
///   uninstal service and unregistry eventlog
///
bool cmdline::Uninstall()
{
	DWORD		Err;
	TCHAR		msgString[MSG_LEN];
	HINSTANCE   hInst;
	
	hInst = GetModuleHandle(NULL);

	if (  !UninstallService(&Err)		
		||!UninstallEventLog(&Err)
		)
	{
		LoadString(hInst, Err, msgString, MSG_LEN);
		MessageBox(NULL, msgString, DisplayName, MB_OK);
		return false;
	}

	LoadString(hInst, IDS_CMDLINE_MDELETE, msgString, MSG_LEN);
	MessageBox(NULL, msgString, DisplayName, MB_OK);

	return true;
} // bool cmdline::Uninstall()

///
/// DisplayHelp
///
void cmdline::DisplayHelp()
{
	TCHAR		msgString[MSG_LEN];
	HINSTANCE   hInst;
	
	hInst = GetModuleHandle(NULL);

	LoadString(hInst, IDS_CMDLINE_MHELP, msgString, MSG_LEN);
	MessageBox(NULL, msgString, ServiceName, MB_OK);
} // void cmdline::DisplayHelp()

///
/// InstallService
///
bool cmdline::InstallService(DWORD* pErr)
{
	*pErr = 0;

	SC_HANDLE hSCM;
	SC_HANDLE hService;	

	// open SCM
	hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);

	if (!hSCM)
	{
		if (GetLastError()==ERROR_ACCESS_DENIED)
		{
			*pErr = IDS_CMDLINE_EOPENSCM_AD;
		}
		else
		{
			*pErr = IDS_CMDLINE_EOPENSCM;
		}

		return false;
	}
		
	TCHAR ServicePath[_MAX_PATH + 3];

	/// Get path to executable file
	GetModuleFileName(NULL, ServicePath + 1, _MAX_PATH);

	ServicePath[0] = TEXT('\"');
	ServicePath[lstrlen(ServicePath) + 1] = 0;
	ServicePath[lstrlen(ServicePath)] = TEXT('\"');

	hService = CreateService(
		hSCM,
		ServiceName,
		DisplayName,
		0,
		SERVICE_WIN32_OWN_PROCESS,
		SERVICE_DEMAND_START,
		SERVICE_ERROR_NORMAL,
		ServicePath,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL);

	CloseServiceHandle(hSCM);

    if (!hService)
	{
		switch (GetLastError())
		{
		case ERROR_DUP_NAME:
			*pErr = IDS_CMDLINE_ECREATE_DUPNAME;
			break;
		case ERROR_SERVICE_EXISTS:
			*pErr = IDS_CMDLINE_ECREATE_EXISTS;
			break;
		default:
			*pErr = IDS_CMDLINE_ECREATE;
		}		

		return false;
	}

	CloseServiceHandle(hService);

	return true;
} // bool cmdline::InstallService(DWORD* pErr)

///
/// WriteDescription to registry
///
bool cmdline::WriteDescription(HINSTANCE hInst, DWORD* pErr)
{
	*pErr = 0;

	TCHAR Descr[MSG_LEN];
	DWORD res;	

	// ѕолучает описание
	res = LoadString(hInst, IDS_DESCRIPTION, Descr, MSG_LEN);
	
	if (res==0)
	{
		*pErr = IDS_CMDLINE_ELOAD_DESC;
		return false;
	}

	TCHAR KeyName[KEY_LEN];

	wsprintf(KeyName, TEXT("%s\\%s"), SERVICE_PARENT_KEY, ServiceName);

	HKEY hKey;

	res = RegOpenKeyEx(
			HKEY_LOCAL_MACHINE,
			KeyName,
			0,
			KEY_SET_VALUE,
			&hKey);

	if (res!=ERROR_SUCCESS)
	{
		*pErr = IDS_CMDLINE_EOPEN_SERVKEY;
		return false;
	}

	res = RegSetValueEx(
			hKey,
			TEXT("Description"),
			0,
			REG_SZ,
			(CONST BYTE*) Descr,
			(lstrlen(Descr) + 1)*sizeof(TCHAR));

	if (res!=ERROR_SUCCESS)
	{
		*pErr = IDS_CMDLINE_EWRITE_DESC;
		RegCloseKey(hKey);
		return false;
	}

	RegCloseKey(hKey);
	return true;
} // bool cmdline::WriteDescription(HINSTANCE hInst, DWORD* pErr)

///
/// InstallEventLog create new event source for ”мутеƒщп
///
bool cmdline::InstallEventLog(DWORD* pErr)
{
	*pErr = 0;

	HKEY  hKey;
	DWORD res;	

	TCHAR KeyName[KEY_LEN];

	wsprintf(KeyName, TEXT("%s\\%s"),	LOG_PARENT_KEY, EventSource);

	res = RegCreateKeyEx(
			HKEY_LOCAL_MACHINE, 
			KeyName,
			0,
			NULL,
			0,
			KEY_SET_VALUE,
			NULL,
			&hKey,
			NULL);

	if (res!=ERROR_SUCCESS)
	{
		*pErr = IDS_CMDLINE_ECREATE_LOGKEY;
		return false;
	}

	TCHAR MsgFilePath[_MAX_PATH];

	DWORD Size = _MAX_PATH;

  GetModuleFileName(NULL, MsgFilePath, Size);

	Size = lstrlen(MsgFilePath);
	while (Size > 0)
	{
		if (MsgFilePath[Size]==TEXT('\\')) break;
		Size--;
	}

	MsgFilePath[Size + 1] = 0;

	lstrcat(MsgFilePath, MsgFileName);

    res = RegSetValueEx(
		    hKey,					  
            TEXT("EventMessageFile"),       
            0,                        
            REG_SZ,                   
            (LPBYTE) MsgFilePath,            
            (lstrlen(MsgFilePath) + 1)*sizeof(TCHAR));

	if (res!=ERROR_SUCCESS)
	{
		*pErr = IDS_CMDLINE_EWRITE_LOG;
		RegCloseKey(hKey);
		return false;
	}

	RegCloseKey(hKey);
	return true;
} // bool cmdline::InstallEventLog(DWORD* pErr)

///
/// UninstallService
///
bool cmdline::UninstallService(DWORD* pErr)
{
	*pErr = 0;

	SC_HANDLE hSCM;
	SC_HANDLE hService;

	hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

	if (!hSCM)
	{
		if (GetLastError()==ERROR_ACCESS_DENIED)
		{
			*pErr = IDS_CMDLINE_EOPENSCM_AD;
		}
		else
		{
			*pErr = IDS_CMDLINE_EOPENSCM;
		}		
		return false;
	}

	hService = OpenService(hSCM, ServiceName,  DELETE | SERVICE_STOP);
	
	if (!hService)
	{
		CloseServiceHandle(hSCM);

		if (GetLastError()==ERROR_SERVICE_DOES_NOT_EXIST)
		{
			// service not exist
			return true;
		}
					
		*pErr = IDS_CMDLINE_EOPENSERV;
		
		return false;
	}

    SERVICE_STATUS ss;

	ControlService(hService, SERVICE_CONTROL_STOP, &ss);

	DeleteService(hService);

	CloseServiceHandle(hService);
	CloseServiceHandle(hSCM);

	return true;
} // bool cmdline::UninstallService(DWORD* pErr)

///
/// remove service from Event Log
///
bool cmdline::UninstallEventLog(DWORD* pErr)
{
	*pErr = 0;

	HKEY  hKey;
	DWORD res;

	res = RegOpenKeyEx(
			HKEY_LOCAL_MACHINE, 
			LOG_PARENT_KEY,
			0,
			DELETE,
			&hKey);

	if (res!=ERROR_SUCCESS)
	{
		*pErr = IDS_CMDLINE_EOPEN_LOGKEY;
		return false;
	}

	res = RegDeleteKey(hKey, EventSource);

	if (res!=ERROR_SUCCESS)
	{
		*pErr = IDS_CMDLINE_EDELETE_LOG;
		RegCloseKey(hKey);
		return false;
	}

	RegCloseKey(hKey);
	return true;
} // bool cmdline::UninstallEventLog(DWORD* pErr)
