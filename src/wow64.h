/*
Copyright (c) Anthony Beaumont
This source code is licensed under the MIT License
found in the LICENSE file in the root directory of this source tree.
*/

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <psapi.h>

DWORD GetModuleBaseAddress(const HANDLE hProcess, LPCSTR moduleName);
DWORD GetProcAddressFromExportAddressTable(HANDLE hProcess, LPCSTR moduleName, LPCSTR procName);
