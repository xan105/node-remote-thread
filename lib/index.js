/*
Copyright (c) Anthony Beaumont
This source code is licensed under the MIT License
found in the LICENSE file in the root directory of this source tree.
*/

import { join, dirname, resolve } from "node:path";
import { accessSync, constants } from "node:fs";
import { dlopen } from "node-gyp-load";
import { 
  shouldIntegerPositive,
  shouldStringNotEmpty,
  shouldWindows
} from "@xan105/is/assert";
import { Failure, errorLookup } from "@xan105/error";

//Load bindings
const NAPI = await dlopen({
  name: "thread", 
  cwd: join(import.meta.dirname, "../")
});

export function createRemoteThread(pid, filePath){
  
  shouldIntegerPositive(pid);
  shouldStringNotEmpty(filePath);
  shouldWindows();

  accessSync(filePath, constants.F_OK);

  const code = NAPI.createRemoteThread(pid, resolve(filePath));
  if(code > 0) throw new Failure(...errorLookup(code));
}