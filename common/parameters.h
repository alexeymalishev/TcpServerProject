/***************************************************************************
*                                                                          *
*  parameters.h    registry interface                                      *
*                                                                          *
*  Copyright (C) 2013, Alexey Malishev (Moscow, Russia)                    *
*   All rights reserved.                                                   *
*                                                                          *
****************************************************************************/

#pragma once

#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

#include "windows.h"

//
// Functions for get params from reg
//

#include <string>

namespace Parameters
{

// GetParam
//

  void GetParam(const std::string& lpKey, const std::string& lpVal, std::string *sData);
  void GetParam(const std::string& lpKey, const std::string& lpVal, DWORD *pParam);

  void SetParam(const std::string& lpKey, const std::string& lpVal, const std::string& sData);
  void SetParam(const std::string& lpKey, const std::string& lpVal, const DWORD& nParam);

} //namespace Parameters

#endif
