#!/usr/bin/env bash

cd build
if make; then
	mkdir -p ../run
	cd ../run
	../build/Crayfish
else
	exit 1
fi

exit 0
