#!/usr/bin/env bash

SCRIPT_ROOT=$(dirname -- $(realpath ${BASH_SOURCE[0]}))
. config/parser.sh || exit $?

# setup
mkdir -p build

if [ $# -ne 1 ]; then
	echo "Only input 1 arg: the file to run"
	exit 1
fi
if [ ! -f "$1" ]; then
	echo "File does not exist"
	exit 2
fi

time_limit=$(opt_or_default time_limit 3.0)

function test_program {
	echo -n "" > build/empty.txt
	dialog --keep-tite --aspect 2 --title 'Input the test case' \
		--editbox build/empty.txt 0 0 2> build/input.txt
	
  cat build/input.txt
  echo "====================================="
	timeout -k "$time_limit" "$time_limit" $@ < build/input.txt
	status=$?
	echo
	echo "====================================="
	if [ $status -eq 124 ]; then
		echo 'TLE (Time Limit Exceeded)'
	elif [ $status -ne 0 ]; then
		echo 'RTE (Runtime Exception), process returned '"$status"
	else
		echo 'Process returned normally'
	fi
}

ext=${1##*.}
case "$ext" in
	cpp|cxx|cc)
		if ! clang++ -O3 -std=$(opt_or_default cpp_standard c++11) "$1" -o build/run; then
			echo "Compiler error, exiting..."
			exit 3
		fi
		test_program build/run
		;;
	c)
		if ! clang -O3 -std=$(opt_or_default c_standard c11) "$1" -o build/run; then
			echo "Compiler error, exiting..."
			exit 3
		fi
		test_program build/run
		;;
	java)
		test_program java --source=$(opt_or_default java_version 11) "$1"
		;;
	py)
		test_program python3 "$1"
		;;
	# can add more languages if necessary
esac