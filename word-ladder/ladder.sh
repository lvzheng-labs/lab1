#!/bin/bash

DIR=$(dirname ${BASH_SOURCE[0]})

while true; do
	echo "Input string a:"
	read s1
	if [ -z "$s1" ]; then
		exit
	fi

	echo "Input string b:"
	read s2
	if [ -z "$s1" ]; then
		exit
	fi

	echo
	echo -e "$s1\n$s2" | $DIR/query | $DIR/display.py
	echo
done
