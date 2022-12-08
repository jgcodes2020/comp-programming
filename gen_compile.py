#!/usr/bin/env python3
from pathlib import Path
import os, sys, json
import config.parser as cfg

SCRIPT_ROOT = Path(__file__).parent
os.chdir(SCRIPT_ROOT)



def main():
    result = []
    for file in SCRIPT_ROOT.glob("**/*.c*"):
        absfile = file.resolve(True)
        match file.suffix:
            case ".cpp"|".cxx"|".cc":
                print(f"Found C++ file: {file}")
                result.append({
                    "directory": str(SCRIPT_ROOT),
                    "arguments": [
                        "clang++", "-O3", f"-std={cfg.get_cfg('cpp_standard', 'c++11')}", 
                        "-o", "build/run", str(absfile)
                    ],
                    "file": str(absfile)
                })
            case ".c":
                print(f"Found C file:   {file}")
                result.append({
                    "directory": str(SCRIPT_ROOT),
                    "arguments": [
                        "clang", "-O3", f"-std={cfg.get_cfg('c_standard', 'c11')}", 
                        "-o", "build/run", str(absfile)
                    ],
                    "file": str(absfile)
                })
    
    with open(SCRIPT_ROOT/"compile_commands.json", "w") as outfile:
        print("Serializing...")
        json.dump(result, outfile)
    
    pass
if __name__ == "__main__":
    main()