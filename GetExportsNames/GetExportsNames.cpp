// ֳכאגםûי DLL-פאיכ.

#include "GetExportsNames.h"

#include "stdafx.h"

#include <Windows.h>
#include <dbghelp.h>
#include <Winsvc.h>
//#include <advapi.h>

#define ServiceName         TEXT("TcpServer")
#define PARAMETERS_CHANGED  129

extern "C" __declspec(dllexport) int __stdcall GetDLLFileExports(LPCSTR szFileName, char **&pszFunctions)
{
  HANDLE hFile;
  HANDLE hFileMapping;
  LPVOID lpFileBase;
  PIMAGE_DOS_HEADER pImg_DOS_Header;
  PIMAGE_NT_HEADERS pImg_NT_Header;
  PIMAGE_EXPORT_DIRECTORY pImg_Export_Dir;

  int nNoOfExports;

  hFile = CreateFileA(szFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
  if (hFile == INVALID_HANDLE_VALUE)
    return false;

  hFileMapping = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
  if (hFileMapping == 0)
  {
    CloseHandle(hFile);
    return false;
  }

  lpFileBase = MapViewOfFile(hFileMapping, FILE_MAP_READ, 0, 0, 0);
  if (lpFileBase == 0)
  {
    CloseHandle(hFileMapping);
    CloseHandle(hFile);
    return false;
  }

  pImg_DOS_Header = (PIMAGE_DOS_HEADER)lpFileBase;

  pImg_NT_Header = (PIMAGE_NT_HEADERS)((LONG)pImg_DOS_Header + (LONG)pImg_DOS_Header->e_lfanew);


  if (IsBadReadPtr(pImg_NT_Header, sizeof(IMAGE_NT_HEADERS)) || pImg_NT_Header->Signature != IMAGE_NT_SIGNATURE)
  {
    UnmapViewOfFile(lpFileBase);
    CloseHandle(hFileMapping);
    CloseHandle(hFile);
    return false;
  }

  pImg_Export_Dir = (PIMAGE_EXPORT_DIRECTORY)pImg_NT_Header->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
  if (!pImg_Export_Dir)
  {
    UnmapViewOfFile(lpFileBase);
    CloseHandle(hFileMapping);
    CloseHandle(hFile);
    return false;
  }

  pImg_Export_Dir = (PIMAGE_EXPORT_DIRECTORY)ImageRvaToVa(pImg_NT_Header, pImg_DOS_Header, (DWORD)pImg_Export_Dir, 0);

  DWORD **ppdwNames = (DWORD **)pImg_Export_Dir->AddressOfNames;

  ppdwNames = (PDWORD*)ImageRvaToVa(pImg_NT_Header, pImg_DOS_Header, (DWORD)ppdwNames, 0);
  if (!ppdwNames)
  {
    UnmapViewOfFile(lpFileBase);
    CloseHandle(hFileMapping);
    CloseHandle(hFile);
    return false;
  }

  nNoOfExports = pImg_Export_Dir->NumberOfNames;

  pszFunctions = new char*[nNoOfExports];

  for (int i = 0; i < nNoOfExports; i++)
  {
    char *szFunc = (PSTR)ImageRvaToVa(pImg_NT_Header, pImg_DOS_Header, (DWORD)*ppdwNames, 0);

    pszFunctions[i] = new char[strlen(szFunc) + 1];
    strcpy(pszFunctions[i], szFunc);

    ppdwNames++;
  }
  UnmapViewOfFile(lpFileBase);
  CloseHandle(hFileMapping);
  CloseHandle(hFile);
  return nNoOfExports;
};

// send message to restart to service
extern "C" __declspec(dllexport) void __stdcall Signal()
{
  SC_HANDLE hSCM;
  SC_HANDLE hService;

  hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

  if (!hSCM)
  {
    return;
  }

  hService = OpenService(hSCM, ServiceName, SERVICE_USER_DEFINED_CONTROL);

  if (!hService)
  {
    CloseServiceHandle(hSCM);

    return;
  }

  SERVICE_STATUS sStatus;

  ControlService(hService, PARAMETERS_CHANGED, &sStatus);

  CloseServiceHandle(hService);
  CloseServiceHandle(hSCM);
}
