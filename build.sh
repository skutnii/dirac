#!/bin/bash

if [ "$1" = "clean" ]; then
	rm -rf build
	shift 1
fi	

if [ ! -d "build" ]; then
	mkdir build
fi

cd build
cmake .. $@
make