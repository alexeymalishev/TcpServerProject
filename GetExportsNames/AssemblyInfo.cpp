#include "stdafx.h"

using namespace System;
using namespace System::Reflection;
using namespace System::Runtime::CompilerServices;
using namespace System::Runtime::InteropServices;
using namespace System::Security::Permissions;

//
// ����� �������� �� ���� ������ ��������������� ��������� �������
// ���������. �������������� �������� ���� ���������, ����� ��������
// ����� �������� �� ���� ������.
//
[assembly:AssemblyTitleAttribute(L"GetExportsNames")];
[assembly:AssemblyDescriptionAttribute(L"")];
[assembly:AssemblyConfigurationAttribute(L"")];
[assembly:AssemblyCompanyAttribute(L"newimpulse")];
[assembly:AssemblyProductAttribute(L"GetExportsNames")];
[assembly:AssemblyCopyrightAttribute(L"Copyright (c) newimpulse 2013")];
[assembly:AssemblyTrademarkAttribute(L"")];
[assembly:AssemblyCultureAttribute(L"")];

//
// �������� � ������ ������ ������� �� ��������� ������� ��������:
//
//      �������� ����� ������
//      �������������� ����� ������
//   ����� ������
//      ��������
//
// ����� ������ ��� �������� ��� ������� ������ ������ � �������� �� ���������
// ��������� "*", ��� �������� ����:

[assembly:AssemblyVersionAttribute("1.0.*")];

[assembly:ComVisible(false)];

[assembly:CLSCompliantAttribute(true)];