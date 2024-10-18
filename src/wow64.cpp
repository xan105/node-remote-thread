/*
Copyright (c) Anthony Beaumont
This source code is licensed under the MIT License
found in the LICENSE file in the root directory of this source tree.
*/

#include "wow64.h"

#ifdef _WIN64
  #pragma warning(disable : 4302)
  #pragma warning(disable : 4311)
  #pragma warning(disable : 4312)
#endif

DWORD GetModuleBaseAddress(const HANDLE hProcess, LPCSTR moduleName){

  DWORD moduleAddress = 0;
  HMODULE hMods[1024];
  DWORD cbNeeded;

  do 
  {
    if(EnumProcessModulesEx(hProcess, hMods, sizeof(hMods), &cbNeeded, LIST_MODULES_ALL)){
      for (int i = 0; i < (cbNeeded / sizeof(HMODULE)); i++)
      {
        char modName[MAX_PATH];
        if (GetModuleBaseNameA(hProcess, hMods[i], modName, sizeof(modName))){
          if (strcmp(modName, moduleName) == 0){
            MODULEINFO modinf;
            if(GetModuleInformation(hProcess, hMods[i], &modinf, cbNeeded)){ 
              moduleAddress = (DWORD) modinf.lpBaseOfDll;
            }
            break;
          }
        }
      }
    }
  } while(moduleAddress == 0);
  
  return moduleAddress;
}

DWORD GetProcAddressFromExportAddressTable(HANDLE hProcess, LPCSTR moduleName, LPCSTR procName){

  DWORD modBase = GetModuleBaseAddress(hProcess, moduleName);

  IMAGE_DOS_HEADER dosHeaders;
  IMAGE_NT_HEADERS32 ntHeaders;
  IMAGE_EXPORT_DIRECTORY exportDirectory;
  
  ReadProcessMemory(hProcess, reinterpret_cast<LPVOID>(modBase), static_cast<LPVOID>(&dosHeaders), sizeof(IMAGE_DOS_HEADER), 0);
    if (dosHeaders.e_magic != IMAGE_DOS_SIGNATURE) return 0;
  ReadProcessMemory(hProcess, reinterpret_cast<LPVOID>(modBase + static_cast<DWORD>(dosHeaders.e_lfanew)), static_cast<LPVOID>(&ntHeaders), sizeof(IMAGE_NT_HEADERS32), 0);
  ReadProcessMemory(hProcess, reinterpret_cast<LPVOID>(ntHeaders.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress + modBase), static_cast<LPVOID>(&exportDirectory), sizeof(IMAGE_EXPORT_DIRECTORY), 0);
  
  for (DWORD i = 0; i < exportDirectory.NumberOfFunctions; i++)
  {
    DWORD currentFunctionNamePtr;
    WORD currentFunctionNameOrdinalsPtr;
    DWORD currentFunctionVirtualAddress;

    char currentFunctionName[4096];

    ReadProcessMemory(hProcess, reinterpret_cast<LPVOID>(modBase + exportDirectory.AddressOfNames + (i * sizeof(DWORD))), &currentFunctionNamePtr, sizeof(DWORD), nullptr);
    ReadProcessMemory(hProcess, reinterpret_cast<LPVOID>(modBase + currentFunctionNamePtr), &currentFunctionName, sizeof(currentFunctionName), nullptr);
    ReadProcessMemory(hProcess, reinterpret_cast<LPVOID>(modBase + exportDirectory.AddressOfNameOrdinals + (i * sizeof(WORD))), &currentFunctionNameOrdinalsPtr, sizeof(WORD), 0);
    ReadProcessMemory(hProcess, reinterpret_cast<LPVOID>(modBase + exportDirectory.AddressOfFunctions + (currentFunctionNameOrdinalsPtr * sizeof(DWORD))), &currentFunctionVirtualAddress, sizeof(DWORD), nullptr);

    if (strcmp(procName, currentFunctionName) == 0){
      return (modBase + currentFunctionVirtualAddress);
    }
  }
    
  return 0;
}