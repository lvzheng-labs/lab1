#!/bin/bash

DIR=$(dirname ${BASH_SOURCE[0]})

if [ $# -ne 1 ]; then
	echo "usage: $0 <dict-file>"
	exit 1
fi

echo "patience..."
time $DIR/preprocess < "$1" | $DIR/distribute.py
echo "OK."
