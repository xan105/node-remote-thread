import { spawn } from "node:child_process";
import { createRemoteThread } from "../lib/index.js";

const EXECUTABLE = "G:\\Standalone\\Red Alert 3\\Data\\ra3_1.12.game";
const ADDON = "G:\\Standalone\\Red Alert 3\\Launcher\\opencnconline.dll";
const ARGS = ["-config", '"G:\\Standalone\\Red Alert 3\\RA3_english_1.12.SkuDef"'];

let binary = spawn(EXECUTABLE, ARGS, {
  cwd: "G:\\Standalone\\Red Alert 3",
  stdio:[ "ignore", "ignore", "ignore" ], 
  detached: true,
  windowsVerbatimArguments: true
});

binary.once("error", (err) => {
  binary = null;
  console.error(err);
});

binary.once("spawn", () => {
  binary.unref();
  try{
    console.log(binary.pid);
    createRemoteThread(binary.pid, ADDON);
  }catch(err){
    console.error(err);
  }
  binary = null;
});

binary.once("close", ()=>{
  process.exit(0);
});

setTimeout(() => {}, 60 * 60 * 1000); //Keep alive