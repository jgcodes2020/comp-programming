#!/usr/bin/env python3
from io import SEEK_SET
import config.parser as cfg
from sys import exit
from pathlib import Path
import subprocess as subp
import os, sys
import time as tm
import argparse as ap

SCRIPT_ROOT = Path(__file__).parent
os.chdir(SCRIPT_ROOT)

def test_program_simple(cmd_argv: list[str]):
    TIME_LIMIT = float(cfg.get_cfg("time_limit", "3.0"))
    
    with open("build/input.txt", 'w+') as input_file:
        proc: subp.Popen = None
        
        if sys.stdin.isatty():
            dlgproc = subp.run([
                "dialog", "--keep-tite", "--aspect", "2", "--title", "Input test case",
                "--editbox", "build/input.txt", "0", "0"
            ], stderr=input_file)
            
            if dlgproc.returncode != 0:
                print("Input canceled")
                exit(0)
            
            input_file.seek(0, SEEK_SET)
            print(input_file.read())
            input_file.seek(0, SEEK_SET)
            
            proc = subp.Popen(cmd_argv, stdin=input_file, stdout=subp.PIPE, stderr=subp.PIPE)
        else:
            proc = subp.Popen(cmd_argv, stdin=sys.stdin, stdout=subp.PIPE, stderr=subp.PIPE)
        
        # Auto-controls TLE for us
        st = tm.time_ns()
        cout, cerr = None, None
        try:
            cout, cerr = proc.communicate(None, None)
            st = tm.time_ns() - st
            print("\n==============================")
            sys.stdout.buffer.write(cout)
            
        except subp.TimeoutExpired:
            print("\n================================")
            print("TLE (Time Limit Exceeded)")
            return
        
        if proc.returncode is not None:
            if proc.returncode == 0:
                print("\n==============================")
                print(f"Process returned normally ({st / 10**9:.3f} s)")
            else:
                print("\n==============================")
                print(f"RTE (Runtime Exception), process returned {proc.returncode}")
            return
        # process isn't dead
        proc.kill()
    
def test_program_interactive(cmd_argv: list[str]):
    subp.run(cmd_argv)
    
def test_program(cmd_argv: list[str], interactive: bool):
    if interactive:
        test_program_interactive(cmd_argv)
    else:
        test_program_simple(cmd_argv)
        
def check_valid_path(parser, arg):
    if not os.path.exists(arg):
        parser.error(f"{arg} does not exist!")
    else:
        return Path(arg)
    
        
def parse_args() -> ap.Namespace:
    parser = ap.ArgumentParser()
    parser.add_argument("source", type=lambda x: check_valid_path(parser, x))
    parser.add_argument("-i", "--interactive", action="store_true")
    
    return parser.parse_args()

def main():
    opts = parse_args()
    
    (SCRIPT_ROOT/"build").mkdir(exist_ok=True)
    os.chdir(SCRIPT_ROOT)

    src_path = Path(opts.source)

    match src_path.suffix:
        case ".cpp"|".cxx"|".cc":
            try:
                subp.run([
                    "clang++", "-O2", f"-std={cfg.get_cfg('cpp_standard', 'c++11')}", 
                    "-g", "-o", "build/run", src_path
                ]).check_returncode()
            except subp.CalledProcessError:
                print("Compiler error, exiting...")
                exit(3)
            print("==============================")
            test_program(["build/run"], interactive=opts.interactive)
        case ".c":
            try:
                subp.run([
                    "clang", f"-std={cfg.get_cfg('c_standard', 'c11')}", 
                    "-g", "-o", "build/run", src_path
                ]).check_returncode()
            except subp.CalledProcessError:
                print("Compiler error, exiting...")
                exit(3)
            test_program(["build/run"], interactive=opts.interactive)
        case ".java":
            test_program(["java", f"--source={cfg.get_cfg('java_version', '11')}", src_path], interactive=opts.interactive)
        case ".py":
            test_program(["python3", src_path], interactive=opts.interactive)
        case _:
            print("Unknown file extension.")
if __name__ == "__main__":
    main()