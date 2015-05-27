/*
	Copyright (C) 2015 Minho Jo <whitestone8214@openmailbox.org>
	
	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.
	
	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
	
	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// gcc -O3 -std=gnu99 $(pkg-config --cflags --libs vte) my-desktop.c

#include <vte/vte.h>
#include <gdk/gdkkeysyms.h>
#include <malloc.h>

static GtkWidget *overall, *overall1, *pages, *pages_list, *pages_label, *pages_new, *pages_remove, **page;

static int screen_width, screen_height; static char **command; static int n_pages, this_page, wanted_page; static GdkGeometry sizing; static GdkPixbuf *wallpaper; static GdkWindow *soul;

int main (int argc, char *argv[]);
static gboolean show_wallpaper (GtkWidget *widget, GdkEvent *event, void *data);

int main (int argc, char *argv[]) {
	// Properties.
	printf ("[my-desktop] Hi.\n"); gtk_init (&argc, &argv);
	page = NULL;
	GdkScreen *screen = gdk_screen_get_default (); screen_width = gdk_screen_get_width (screen); screen_height = gdk_screen_get_height (screen);
	wallpaper = gdk_pixbuf_new_from_file ("/etc/pictures/wallpaper.jpg", NULL);
	n_pages = 0; this_page = -1; wanted_page = -1;
	
	// Main casts.
	overall = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_resize ((GtkWindow *) overall, screen_width, screen_height);
	gtk_window_set_type_hint ((GtkWindow *) overall, GDK_WINDOW_TYPE_HINT_DESKTOP);
	
	// Action!
	gtk_widget_show_all (overall);
	soul = gtk_widget_get_window (overall);
	gdk_window_set_events (soul, gdk_window_get_events (soul) | GDK_EXPOSURE_MASK);
	g_signal_connect (overall, "expose_event", (GCallback) show_wallpaper, NULL);
	gtk_main ();
	
	// Dispose.
	printf ("[my-desktop] Bye.\n"); return 0;
}

static gboolean show_wallpaper (GtkWidget *widget, GdkEvent *event, void *data) {
	GdkColor color; color.red = 65535; color.green = 0; color.blue = 0;
	GdkGC *palette = gdk_gc_new ((GdkDrawable *) soul);
	
	float _width = (float) gdk_pixbuf_get_width (wallpaper); float _height = (float) gdk_pixbuf_get_height (wallpaper);
	float _width_scaled = _width / ((float) screen_width); float _height_scaled = _height / ((float) screen_height);
	float _width_new = _width / (_width_scaled < _height_scaled ? _width_scaled : _height_scaled);
	float _height_new = _height / (_width_scaled < _height_scaled ? _width_scaled : _height_scaled);
	GdkPixbuf *_wallpaper = gdk_pixbuf_scale_simple (wallpaper, (int) _width_new, (int) _height_new, GDK_INTERP_BILINEAR);
	printf ("[] %f x %f -> (%f x %f) -> %f x %f\n", _width, _height, _width_scaled, _height_scaled, _width_new, _height_new);
	gdk_draw_pixbuf ((GdkDrawable *) soul, palette, _wallpaper, 0, 0, 0, 0, -1, -1, GDK_RGB_DITHER_MAX, 0, 0);
	
	g_object_unref (palette); return TRUE;
}
