#!/usr/bin/env bash

cd build
if make
then
	cd ../run
	gdb ../build/Crayfish
else
	echo ERROR: Failed to build project
	exit 1
fi

exit 0
