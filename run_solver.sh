#!/bin/sh

set -e

echo "== running tests==";
for testfile in `ls -1 tests`; do
	cat tests/$testfile | ./bin/calculate
	echo "== press any key to continue to next test ==";
	read 
done


