/***************************************************************************
*                                                                          *
*  parameters.cpp  registry interface                                      *
*                                                                          *
*  Copyright (C) 2013, Alexey Malishev (Moscow, Russia)                    *
*   All rights reserved.                                                   *
*                                                                          *
****************************************************************************/

#include <sstream>
#include "parameters.h"

namespace Parameters
{

HKEY OpenKey(const std::string& lpKey)
{

	HKEY  hKey;
	DWORD res;

	res = RegOpenKeyExA(
            //HKEY_CURRENT_USER,
            HKEY_LOCAL_MACHINE,
            lpKey.c_str(),
			      0,
			      KEY_QUERY_VALUE,
			      &hKey);

	if (res != ERROR_SUCCESS)
	{	
 		std::stringstream sErrMessage;

    char buf[256];
    int sz = 256;

    FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, 0, res, 0, buf, sz, 0);

    sErrMessage << "Registry key " << lpKey << " Errorcode " << buf;
		throw std::exception(sErrMessage.str().c_str());
	}

	return hKey;
}

//--------------------------------------------------------------

HKEY CreateKey(const std::string& lpKey)
{
    HKEY  hKey;
    DWORD res;

    res = RegCreateKeyExA(
            //HKEY_CURRENT_USER,
            HKEY_LOCAL_MACHINE,
            lpKey.c_str(),
            0,
            NULL,
            0,
            KEY_SET_VALUE,
            NULL,
            &hKey,
            NULL);


    if (res != ERROR_SUCCESS)
    {
        std::stringstream sErrMessage;

        char buf[256];
        int sz = 256;

        FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, 0, res, 0, buf, sz, 0);
        sErrMessage << "Registry key " << lpKey << "\\Parameters Error " << buf;

        throw std::exception(sErrMessage.str().c_str());
    }

    return hKey;
}

//--------------------------------------------------------------


void GetParam(const std::string& lpKey, const std::string& lpVal, std::string* sData)
{
	DWORD res;
	DWORD type;
	HKEY  hKey = OpenKey(lpKey);

	const size_t bufferSize = 512;

	char tmpStr[bufferSize];
	DWORD size = bufferSize;

	res = RegQueryValueExA(
			hKey,
      lpVal.c_str(),
			NULL,
			&type,
			(BYTE*) tmpStr,
            &size);

	RegCloseKey(hKey);

	if ((res != ERROR_SUCCESS) || (type != REG_SZ))
	{
		std::stringstream sErrMessage;

    char buf[256];
    int sz = 256;

    FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, 0, res, 0, buf, sz, 0);

    sErrMessage << "Registry key " << lpKey << "\\" << lpVal << " Errorcode " << buf;
		throw std::exception(sErrMessage.str().c_str());
	}

    if(0 != size) {
        *sData = tmpStr;
    } else {
        *sData = "";
    }
}

void GetParam(const std::string& lpKey, const std::string& lpVal, DWORD* pParam)
{
	DWORD res;
	DWORD type;
	HKEY  hKey = OpenKey(lpKey);
    DWORD size = sizeof(*pParam);

	res = RegQueryValueExA(
			hKey,
            lpVal.c_str(),
			NULL,
			&type,
            (BYTE*) pParam,
			&size);

	RegCloseKey(hKey);

	if ((res != ERROR_SUCCESS) || (type != REG_DWORD))
	{
		std::stringstream sErrMessage;

        char buf[256];
        int sz = 256;

        FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, 0, res, 0, buf, sz, 0);

        sErrMessage << "Registry key " << lpKey << "\\" << lpVal << " Errorcode " << buf;
		throw std::exception(sErrMessage.str().c_str());
	}
}

void SetParam(const std::string& lpKey, const std::string& lpVal, const std::string& sData)
{
    DWORD res;
    HKEY  hKey = CreateKey(lpKey);

    size_t size = sData.length();

    res = RegSetValueExA(
                hKey,
                lpVal.c_str(),
                0,
                REG_SZ,
                (CONST BYTE*) sData.c_str(),
                size);


    RegCloseKey(hKey);

    if (res != ERROR_SUCCESS)
    {
        std::stringstream sErrMessage;

        char buf[256];
        int sz = 256;

        FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, 0, res, 0, buf, sz, 0);

        sErrMessage << "Registry key " << lpKey << "\\" << lpVal << " Errorcode " << buf;

        throw std::exception(sErrMessage.str().c_str());
    }
}

void SetParam(const std::string& lpKey, const std::string& lpVal, const DWORD &nParam)
{
    DWORD res;
    HKEY  hKey = CreateKey(lpKey);

    res = RegSetValueExA(
                hKey,
                lpVal.c_str(),
                0,
                REG_DWORD,
                (CONST BYTE*) &nParam,
                sizeof(nParam));



    RegCloseKey(hKey);

    if (res != ERROR_SUCCESS)
    {
        std::stringstream sErrMessage;

        char buf[256];
        int sz = 256;

        FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, 0, res, 0, buf, sz, 0);

        sErrMessage << "Registry key " << lpKey << "\\" << lpVal << " Errorcode " << buf;
        throw std::exception(sErrMessage.str().c_str());
    }
}

} //namespace Parameters
///////////////////////////////////////////////////////////////////////////////////////////////
