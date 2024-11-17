/*
Copyright (c) Anthony Beaumont
This source code is licensed under the MIT License
found in the LICENSE file in the root directory of this source tree.
*/

import { platform, arch } from "node:process";
import { join, dirname, resolve } from "node:path";
import { accessSync, constants } from "node:fs";
import { dlopen } from "@xan105/addons";
import { 
  shouldIntegerPositive,
  shouldStringNotEmpty,
  shouldWindows
} from "@xan105/is/assert";
import { Failure, errorLookup } from "@xan105/error";

const INTEGRITY = {
  win32: { 
    ia32: "sha384-qPlN82iKTaqK4VyvY7ur+Io23Hr4SGVF8qLPMovPhyqDXTASLJRt2DfM7fvnCmwe",
    x64: "sha384-rWHB9j/lDrphTS8fZrIqnvd6kEiG5OVbmeAiXDSR8i9DvnIzqq22yHUPKbWMt4+3",
    arm64: "sha384-zoT2xyCJr/1vRYPF6qTO+5sDc6EQY2/k8I3vsiToaWQIB2DeZal5EYLojZdwLzVi"
  }
};

//Load bindings
const NAPI = await dlopen("thread.node",{ 
  cwd: join(import.meta.dirname, "../"),
  integrity: INTEGRITY[platform]?.[arch] 
});

export function createRemoteThread(pid, filePath){
  
  shouldIntegerPositive(pid);
  shouldStringNotEmpty(filePath);
  shouldWindows();

  accessSync(filePath, constants.F_OK);

  const code = NAPI.createRemoteThread(pid, resolve(filePath));
  if(code > 0) throw new Failure(...errorLookup(code));
}