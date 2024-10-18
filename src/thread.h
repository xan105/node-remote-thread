/*
Copyright (c) Anthony Beaumont
This source code is licensed under the MIT License
found in the LICENSE file in the root directory of this source tree.
*/

#define WIN32_LEAN_AND_MEAN

#include <napi.h>
#include <windows.h>
#include "string.h"

#ifdef _WIN64
  #include "wow64.h"
#endif

LPTHREAD_START_ROUTINE GetProcAddressFrom(HANDLE hProcess, LPCSTR moduleName, LPCSTR procName);
Napi::Value createRemoteThread(const Napi::CallbackInfo& info);
Napi::Object Init(Napi::Env env, Napi::Object exports);
