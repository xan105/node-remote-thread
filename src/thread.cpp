/*
Copyright (c) Anthony Beaumont
This source code is licensed under the MIT License
found in the LICENSE file in the root directory of this source tree.
*/

#include "thread.h"

LPTHREAD_START_ROUTINE GetProcAddressFrom(HANDLE hProcess, LPCSTR moduleName, LPCSTR procName){

  #ifdef _WIN64
  #pragma warning(disable : 4312)
    BOOL isWow64 = FALSE;
    if (IsWow64Process(hProcess, &isWow64)) {
      if(isWow64){
        LPTHREAD_START_ROUTINE procAddress = (LPTHREAD_START_ROUTINE) GetProcAddressFromExportAddressTable(hProcess, moduleName, procName);
        return procAddress; 
      }
    }
  #endif 

  HMODULE hModule = GetModuleHandleA(moduleName);
  if (hModule == NULL) return NULL;
  LPTHREAD_START_ROUTINE procAddress = (LPTHREAD_START_ROUTINE) GetProcAddress(hModule, procName);
  return procAddress; 
}

Napi::Value createRemoteThread(const Napi::CallbackInfo& info){
  Napi::Env env = info.Env();
  
  if (info.Length() != 2) {
    Napi::TypeError::New(env, "Expected 2 arguments: number PID, string FILEPATH").ThrowAsJavaScriptException();
    return env.Undefined();
  }
  
  if (!info[0].IsNumber()) {
    Napi::TypeError::New(env, "Expected number (PID)").ThrowAsJavaScriptException();
    return env.Undefined();
  }
  
  if (!info[1].IsString()) {
    Napi::TypeError::New(env, "Expected string (FILEPATH)").ThrowAsJavaScriptException();
    return env.Undefined();
  }
  
  DWORD pid = info[0].As<Napi::Number>().Uint32Value();
  std::wstring filePath = toWString(info[1].As<Napi::String>().Utf8Value());

  //Opens a handle to the target process with the needed permissions
  HANDLE hProcess = OpenProcess(
    PROCESS_CREATE_THREAD | 
    PROCESS_VM_OPERATION | 
    PROCESS_VM_WRITE | 
    PROCESS_VM_READ |
    PROCESS_QUERY_INFORMATION,
    false,
    pid
  );
  
  if (hProcess == NULL){
    return Napi::Number::New(env, GetLastError());
  }
  
  //Gets a pointer to the LoadLibrary function
  LPTHREAD_START_ROUTINE LoadLibAddress = GetProcAddressFrom(hProcess, "KERNEL32.DLL", "LoadLibraryW");
  if (LoadLibAddress == NULL){
    DWORD lastError = GetLastError();
    CloseHandle(hProcess);
    return Napi::Number::New(env, lastError);
  }
  
  //Allocates virtual memory for the file path
  LPVOID lpBaseAddress = VirtualAllocEx(
    hProcess, 
    NULL, 
    (filePath.length() + 1) * 2,
    MEM_RESERVE | MEM_COMMIT, 
    PAGE_EXECUTE_READWRITE
  );
  
  if (lpBaseAddress == NULL){
    DWORD lastError = GetLastError();
    CloseHandle(hProcess);
    return Napi::Number::New(env, lastError);
  }
  
  //Writes the filename to the previously allocated space
  if(!WriteProcessMemory(
    hProcess, 
    lpBaseAddress, 
    filePath.c_str(),
    (filePath.length() + 1) * 2,
    NULL
  )){
    DWORD lastError = GetLastError();
    VirtualFreeEx(hProcess, lpBaseAddress, 0, MEM_RELEASE);
    CloseHandle(hProcess);
    return Napi::Number::New(env, lastError);
  }

  //Creates a remote thread that loads the DLL triggering it
  HANDLE hThread = CreateRemoteThread(
    hProcess, 
    NULL, 
    NULL, 
    LoadLibAddress, 
    lpBaseAddress,
    NULL, 
    NULL
  );

  if(hThread == NULL) {
    DWORD lastError = GetLastError();
    VirtualFreeEx(hProcess, lpBaseAddress, 0, MEM_RELEASE);
    CloseHandle(hProcess);
    return Napi::Number::New(env, lastError);
  }
  
  WaitForSingleObject(hThread, INFINITE);
  
  CloseHandle(hThread);
  VirtualFreeEx(hProcess, lpBaseAddress, 0, MEM_RELEASE);
  CloseHandle(hProcess);
  
  return Napi::Number::New(env, 0);
}

Napi::Object Init(Napi::Env env, Napi::Object exports){

  exports.Set("createRemoteThread", Napi::Function::New(env, createRemoteThread));
  return exports;
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, Init);