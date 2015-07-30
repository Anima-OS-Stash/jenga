#!/bin/bash

build_desktop=0
build_terminal=0
build_filemanager=0
build_texteditor=0
build_imageviewer=0
build_mediaplayer=0
build_webbrowser=0

for x in $@; do
	echo $x
	if (test "$x" = "desktop"); then build_desktop=1; fi
	if (test "$x" = "terminal"); then build_terminal=1; fi
	if (test "$x" = "filemanager"); then build_filemanager=1; fi
	if (test "$x" = "texteditor"); then build_texteditor=1; fi
	if (test "$x" = "imageviewer"); then build_imageviewer=1; fi
	if (test "$x" = "mediaplayer"); then build_mediaplayer=1; fi
	if (test "$x" = "webbrowser"); then build_webbrowser=1; fi
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
if (test "$build_texteditor" = "1"); then
	echo "[] Build: Text editor"
	gcc -O3 -std=gnu99 $(pkg-config --cflags --libs gtkwidget2 libfile gtksourceview-2.0) my-text-editor.c -o /bin/my-text-editor
fi
if (test "$build_imageviewer" = "1"); then
	echo "[] Build: Image viewer"
	gcc -O3 -std=gnu99 $(pkg-config --cflags --libs gtkwidget2 libfile) my-image-viewer.c -o /bin/my-image-viewer
fi
if (test "$build_mediaplayer" = "1"); then
	echo "[] Build: Media player"
	gcc -O3 -std=gnu99 $(pkg-config --cflags --libs gtkwidget2 libfile gstreamer-1.0 gstreamer-video-1.0) my-media-player.c -o /bin/my-media-player
fi
if (test "$build_webbrowser" = "1"); then
	echo "[] Build: Web browser"
	gcc -O3 -std=gnu99 $(pkg-config --cflags --libs gtkwidget2 libfile webkit-1.0) my-web-browser.c -o /bin/my-web-browser
fi
