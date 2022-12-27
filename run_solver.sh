#!/bin/sh

set -e

echo "== compiling ==";
g++ -std=c++17 -o bin/calculate lib/boggle.cpp lib/test.cpp -O3

echo "== running tests==";
for testfile in `ls -1 tests`; do
	cat tests/$testfile | ./bin/calculate
	echo "== press any key to continue to next test ==";
	read 
done


