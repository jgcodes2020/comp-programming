#!/usr/bin/env python3
from io import SEEK_SET
import shutil
import config.parser as cfg
from sys import argv, exit
from pathlib import Path
import subprocess as subp
import os
import psutil

SCRIPT_ROOT = Path(__file__).parent
os.chdir(SCRIPT_ROOT)

def test_program(cmd_argv: list[str]):
    TIME_LIMIT = float(cfg.get_cfg("time_limit", "3.0"))
    
    with open("build/input.txt", 'w+') as input_file:
        subp.run([
            "dialog", "--keep-tite", "--aspect", "2", "--title", "Input test case",
            "--editbox", "build/input.txt", "0", "0"
        ], stderr=input_file).check_returncode()
        
        input_file.seek(0, SEEK_SET)
        print(input_file.read())
        input_file.seek(0, SEEK_SET)
        print("==============================")
        
        proc = subp.Popen(cmd_argv, stdin=input_file)
        # Auto-controls TLE for us
        proc.wait(TIME_LIMIT)
        
        if proc.returncode is not None:
            if proc.returncode == 0:
                print("\n==============================")
                print("Process returned normally")
            else:
                print("\n==============================")
                print(f"RTE (Runtime Exception), process returned {proc.returncode}")
            return
        # process isn't dead
        proc.kill()
        print("\n==============================")
        print("TLE (Time Limit Exceeded)")

def main():
    (SCRIPT_ROOT/"build").mkdir(exist_ok=True)

    os.chdir(SCRIPT_ROOT)

    if len(argv) != 2:
        print("Only input 1 arg: the file to run")
        exit(1)
        
    src_path = Path(argv[1])
    if not src_path.is_file():
        print("Specified path is not a file")
        exit(2)

    match src_path.suffix:
        case ".cpp"|".cxx"|".cc":
            try:
                subp.run([
                    "clang++", "-O3", f"-std={cfg.get_cfg('cpp_standard', 'c++11')}", 
                    "-o", "build/run", argv[1]
                ]).check_returncode()
            except subp.CalledProcessError:
                print("Compiler error, exiting...")
                exit(3)
            test_program(["build/run"])
        case ".c":
            try:
                subp.run([
                    "clang++", "-O3", f"-std={cfg.get_cfg('c_standard', 'c11')}", 
                    "-o", "build/run", argv[1]
                ]).check_returncode()
            except subp.CalledProcessError:
                print("Compiler error, exiting...")
                exit(3)
            test_program(["build/run"])
        case ".java":
            test_program(["java", f"--source={cfg.get_cfg('java_version', '11')}", argv[1]])
        case ".python":
            test_program(["python3", argv[1]])
if __name__ == "__main__":
    main()