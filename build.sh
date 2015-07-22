#!/bin/bash

build_desktop=0
build_terminal=0
build_filemanager=0

for x in $@; do
	echo $x
	if (test "$x" = "desktop"); then build_desktop=1; fi
	if (test "$x" = "terminal"); then build_terminal=1; fi
	if (test "$x" = "filemanager"); then build_filemanager=1; fi
done

if (test "$build_desktop" = "1"); then
	echo "[] Build: Desktop"
	gcc -O3 -std=gnu99 $(pkg-config --cflags --libs vte) my-desktop.c -o /bin/my-desktop
fi
if (test "$build_terminal" = "1"); then
	echo "[] Build: Terminal emulator"
	gcc -O3 -std=gnu99 $(pkg-config --cflags --libs vte) my-terminal-emulator.c -o /bin/my-terminal-emulator
fi
if (test "$build_filemanager" = "1"); then
	echo "[] Build: File manager"
	gcc -O3 -std=gnu99 $(pkg-config --cflags --libs gtkwidget2 libfile) my-file-manager.c -o /bin/my-file-manager
fi
