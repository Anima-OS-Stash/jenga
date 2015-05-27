#!/bin/bash

build_desktop=0
build_terminal=0

for x in $@; do
	echo $x
	if (test "$x" = "desktop"); then build_desktop=1; fi
	if (test "$x" = "terminal"); then build_terminal=1; fi
done

if (test "$build_desktop" = "1"); then
	echo "[] Build: Desktop"
	gcc -O3 -std=gnu99 $(pkg-config --cflags --libs vte) my-desktop.c -o /bin/my-desktop
fi
if (test "$build_terminal" = "1"); then
	echo "[] Build: Terminal emulator"
	gcc -O3 -std=gnu99 $(pkg-config --cflags --libs vte) my-terminal-emulator.c -o /bin/my-terminal-emulator
fi
