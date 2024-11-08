About
=====

NAPI Native addon for Windows DLL injection with support for Wow64 and Unicode path.

This is a simple injector using the classic `createRemoteThread()` + `LoadLibrary()` from `Kernel32` combo.

- x64 injector
  + can inject x64 DLL into x64 processes.
  + can inject x86 DLL into x86 processes (Wow64).
- x86 injector 
  + can inject x86 DLL into x86 processes.

📦 Scoped `@xan105` packages are for my own personal use but feel free to use them.

Example
=======

```js
import { spawn } from "node:child_process";
import { dirname } from "node:path";
import { createRemoteThread } from "@xan105/remote-thread";

const EXECUTABLE = "G:\\foo\\bar.exe";
const ADDON = "G:\\foo\\人名.dll";

const binary = spawn(EXECUTABLE, [], {
  cwd: dirname(EXECUTABLE),
  stdio:[ "ignore", "ignore", "ignore" ], 
  detached: true
});

binary.once("error", (err) => {
  console.error(err);
});

binary.once("spawn", () => {
  binary.unref();
  //DLL Injection
  createRemoteThread(binary.pid, ADDON);
});
```

Installation
============

```
npm install @xan105/remote-thread
```

🚀 x86 and x64 prebuilt binary provided.

Force compiling:
```
npm install @xan105/remote-thread --build-from-source
```

You will need C/C++ build tools and Python 3.x (node-gyp) to build this module.<br />

> [!IMPORTANT]  
> This package doesn't have any installation restrictions in its package.json file to facilitate multi-platform development; however, it is designed to work only on Windows with x86/x64 architecture.

API
===

⚠️ This module is only available as an ECMAScript module (ESM).

## Named export

#### `createRemoteThread(pid: string, filePath: string): void`

Injects the specified DLL located at _filePath_ into the process identified by the given _pid_.

❌ This function will throw if you are not running on Windows x86/x64.<br />
❌ This function will throw on unexpected error.