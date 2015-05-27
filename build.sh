#!/bin/bash

build_terminal=0

for x in $@; do
	echo $x
	if (test "$x" = "terminal"); then build_terminal=1; fi
done

if (test "$build_terminal" = "1"); then
	echo "[] Build: Terminal emulator"
	gcc -O3 -std=gnu99 $(pkg-config --cflags --libs vte) my-terminal-emulator.c -o /bin/my-terminal-emulator
fi
