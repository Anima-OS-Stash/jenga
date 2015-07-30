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

// gcc -O3 -std=gnu99 $(pkg-config --cflags --libs gtkwidget2 libfile webkit-1.0) my-web-browser.c

#include <gtkwidget2.h>
#include <libfile.h>
#include <gdk/gdkkeysyms.h>
#include <webkit/webkit.h>
#include <malloc.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>

typedef struct tag1 {char *address, *name, *type; int selected, marked;} tag1;
typedef struct tag2 {GtkWidget *window; tag1 *tag1;} tag2;
typedef struct tag3 {char type; char *text1; char *text2;} tag3;

static GtkWidget *overall, *overall1, *pages, *pages_list, *pages_label, *pages_new, *pages_remove, **page;

static unsigned long id_gtkwidget2; static int screen_width, screen_height, screen_span; static char **command; static char **place; static int n_pages, this_page, wanted_page; static GdkGeometry sizing; static char *to; static char stop;

static GtkWidget *chase (GtkWidget *_from, int _spots, ...);
static GtkWidget *session (int _nth);

static GtkWidget *button (int _type, const char *_id, void *_when_clicked, void *_data);
static GtkWidget *popup (int _width, int _height, int _x, int _y, GtkWidget *_content);

static GtkWidget *button2 (char _type, int _width, int _height, const char *_text1, const char *_text2, int _order);
static gboolean content_button (GtkWidget2 *_w, GdkEventExpose *_e);
static gboolean when_clicked_button (GtkWidget *_w, GdkEvent *_e, void *_x);
static void forget_button (GtkWidget2 *_w);

static GtkWidget *button3 (int _width, int _height, char _type, const char *_value1, const char *_value2);
static gboolean button3_draw (GtkWidget2 *_from, GdkEventExpose *_event);
static void button3_refresh (GtkWidget2 *_button);
static void button3_forget (GtkWidget2 *_from);

static GdkPixbuf *picture2 (const char *_address, int _width, int _height);

int main (int argc, char *argv[]);

static void show_nth_page (int _nth, int _from);

static void *ask_quit_keybinding (void *data);
static void *quit_program (GtkWidget *widget, void *data);

static void *new_page (GtkWidget *widget, void *data);
static void *new_page_keybinding (void *data);

static void *remove_page (GtkWidget *widget, void *data);
static void *remove_page_keybinding (void *data);
static void *remove_this_page (GtkWidget *widget, void *data);

static void *show_page (GtkWidget *widget, void *data);
static void *show_label (GtkWidget *widget, void *data);
static void *page_minus1 (void *data);
static void *page_plus1 (void *data);
static gboolean content (GtkWidget2 *_gw2, GdkEventExpose *_event);
static void rectangle (GdkWindow *_canvas, gboolean _fill, int _left, int _top, int _right, int _bottom, unsigned int _bg_red, unsigned int _bg_green, unsigned int _bg_blue, unsigned int _fg_red, unsigned int _fg_green, unsigned int _fg_blue);
static void picture (GdkWindow *_canvas, const char *_address, int _left, int _top, int _right, int _bottom);
static void caption (GdkWindow *_canvas, const char *_text, int _left, int _top, unsigned int _red, unsigned int _green, unsigned int _blue);
static gboolean when_clicked (GtkWidget *widget, GdkEvent *event, void *data);
static void forget (GtkWidget2 *_gw2);

static void *popup_page_list_from_keypress (void *_parcel);

static void *popup_pages (GtkWidget *widget, void *data);
static void *popup_select (GtkWidget *widget, void *data);
static void *popup_quit (GtkWidget *widget, void *data);
static void *popup_what (int _type, GtkWidget *_from, void *_parcel);

static void *do_to_session (GtkWidget *_from, void *_parcel, int _order);

static GtkWidget *chase (GtkWidget *_from, int _spots, ...) {
	va_list _list; va_start (_list, _spots); GtkWidget *_this = _from;
	int _nth = 0; for (_nth = 0; _nth < _spots; _nth++) {
		int _nth1 = va_arg (_list, int);
		if (_nth1 < 0) _this = gtk_widget_get_parent (_this);
		else {
			GList *_list1 = gtk_container_get_children ((GtkContainer *) _this); if (_list1 == NULL) return NULL;
			if (_nth1 >= g_list_length (_list1)) {g_list_free (_list1); return NULL;}
			_this = g_list_nth_data (_list1, _nth1); g_list_free (_list1);
		}
	}
	va_end (_list); return _this;
}
static GtkWidget *session (int _nth) {
	if ((_nth < 0) || (_nth >= n_pages)) return NULL;
	return chase (page[_nth], 3, 1, 0, 0);
}

static GtkWidget *button (int _type, const char *_id, void *_when_clicked, void *_data) {
	GtkWidget *_w = gtk_button_new ();
	if (_type == 1) gtk_button_set_image ((GtkButton *) _w, gtk_image_new_from_icon_name (_id, GTK_ICON_SIZE_MENU));
	else gtk_button_set_label ((GtkButton *) _w, _id);
	if (_when_clicked != NULL) g_signal_connect (_w, "clicked", (GCallback) _when_clicked, _data);
	return _w;
}
static GtkWidget *popup (int _width, int _height, int _x, int _y, GtkWidget *_content) {
	GtkWidget *_w = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_decorated ((GtkWindow *) _w, FALSE);
	gtk_window_set_modal ((GtkWindow *) _w, TRUE);
	gtk_window_resize ((GtkWindow *) _w, _width, _height);
	gtk_window_move ((GtkWindow *) _w, _x, _y);
	gtk_widget_set_events (_w, gtk_widget_get_events (_w) | GDK_FOCUS_CHANGE_MASK);
	g_signal_connect (_w, "focus_out_event", (GCallback) gtk_widget_destroy, NULL);
	if (_content != NULL) gtk_container_add ((GtkContainer *) _w, _content);
	return _w;
}

static GtkWidget *button2 (char _type, int _width, int _height, const char *_text1, const char *_text2, int _order) {
	GtkWidget2 *_button = (GtkWidget2 *) gtk_widget_new (id_gtkwidget2, NULL);
	_button->index = _order; _button->width = _width; _button->height = _height;
	_button->data = (void *) malloc (sizeof (tag3)); if (_button->data == NULL) {if (_button->label != NULL) free (_button->label); return NULL;} tag3 *_tag = (tag3 *) _button->data;
	_tag->type = _type; _tag->text1 = _text1 != NULL ? strdup (_text1) : NULL; _tag->text2 = _text2 != NULL ? strdup (_text2) : NULL;
	
	_button->draw = content_button; _button->forget = forget_button;
	g_signal_connect (_button, "button_release_event", (GCallback) when_clicked_button, NULL);
	return (GtkWidget *) _button;
}
static gboolean content_button (GtkWidget2 *_w, GdkEventExpose *_e) {
	tag3 *_tag = (tag3 *) _w->data;
	GdkWindow *_w1 = gtk_widget_get_window ((GtkWidget *) _w); GdkGC *_g = gdk_gc_new ((GdkDrawable *) _w1);
	if (_tag->text1 != NULL) {
		if (_tag->type == 'p') picture (_w1, _tag->text1, 0, 0, _w->width - 1, _w->height - 1);
		else caption (_w1, _tag->text1, 0, 0, 0, 0, 0);
	}
	return TRUE;
}
static gboolean when_clicked_button (GtkWidget *_w, GdkEvent *_e, void *_x) {
	GtkWidget2 *_w1 = (GtkWidget2 *) _w; GdkEventButton *_e1 = (GdkEventButton *) _e; GdkWindow *_c = _e1->window;
	if ((_e1->x < 0) || (_e1->x >= gdk_window_get_width (_c)) || (_e1->y < 0) || (_e1->y >= gdk_window_get_height (_c))) return FALSE;
	
	/*
		-1 = Close this window
		-2 = Quit this program
		1 = List page(s)
		2 = New page
		3 = Remove current page
		11 = Ask: New session?
		12 = New session
		13 = Ask: Load file?
		14 = Load file
		15 = Ask: Save as?
		16 = Save as
		17 = Play/Pause
	*/
	if (_w1->index == -1) gtk_widget_destroy (gtk_widget_get_toplevel (_w));
	else if (_w1->index == -2) {
		free (page);
		int _nth; for (_nth = 0; _nth < n_pages; _nth++) free (place[_nth]); free (place);
		free (command);
		gtk_main_quit ();
	}
	else if (_w1->index == 1) popup_what (1, overall, overall);
	else if (_w1->index == 2) new_page (overall, NULL);
	else if (_w1->index == 3) remove_page (overall, NULL);
	else if (_w1->index == 11) popup_what (11, overall, overall);
	else if (_w1->index == 12) {
		gtk_widget_destroy (gtk_widget_get_toplevel (_w));
		WebKitWebView *_session = (WebKitWebView *) session (this_page);
		webkit_web_back_forward_list_clear (webkit_web_view_get_back_forward_list (_session));
		webkit_web_view_load_uri (_session, "about:blank");
		free (place[this_page]); place[this_page] = strdup ("(New session)"); gtk_label_set_text ((GtkLabel *) pages_label, place[this_page]);
	}
	else if (_w1->index == 13) popup_what (12, overall, overall);
	else if (_w1->index == 14) {
		char *_file = strdup (gtk_entry_get_text ((GtkEntry *) chase (gtk_widget_get_toplevel (_w), 2, 0, 1)));
		gtk_widget_destroy (gtk_widget_get_toplevel (_w));
		if (_file == NULL) return TRUE;
		webkit_web_view_load_uri ((WebKitWebView *) session (this_page), _file);
		free (place[this_page]); place[this_page] = strdup (_file); gtk_label_set_text ((GtkLabel *) pages_label, place[this_page]);
		free (_file);
	}
	else if (_w1->index == 15) popup_what (13, overall, overall);
	else if (_w1->index == 16) {
		char *_file = strdup (gtk_entry_get_text ((GtkEntry *) chase (gtk_widget_get_toplevel (_w), 2, 0, 1)));
		gtk_widget_destroy (gtk_widget_get_toplevel (_w));
		if (_file == NULL) return TRUE;
		char *_file1 = libfile_refine_address (_file); free (_file); if (_file1 == NULL) return TRUE;
		GdkWindow *_content = gtk_widget_get_window (session (this_page)); int _width = ((GtkWidget2 *) session (this_page))->width; int _height = ((GtkWidget2 *) session (this_page))->height;
		GdkPixbuf *_picture; _picture = gdk_pixbuf_get_from_drawable (_picture, (GdkDrawable *) _content, gdk_colormap_get_system (), 0, 0, 0, 0, _width, _height);
		if (_picture == NULL) {free (_file1); return TRUE;}
		if (gdk_pixbuf_save (_picture, _file1, "png", NULL, NULL) != TRUE) {free (_file1); return TRUE;}
		free (place[this_page]); place[this_page] = strdup (_file1); gtk_label_set_text ((GtkLabel *) pages_label, place[this_page]);
		free (_file1);
	}
	
	return TRUE;
}
static void forget_button (GtkWidget2 *_w) {
	if (_w->index == -1) return;
	tag3 *_tag = (tag3 *) _w->data;
	if (_tag->text1 != NULL) free (_tag->text1);
	if (_tag->text2 != NULL) free (_tag->text2);
	free (_w->data);
	_w->index = -1;
}

static GtkWidget *button3 (int _width, int _height, char _type, const char *_value1, const char *_value2) {
	GtkWidget2 *_button = (GtkWidget2 *) gtk_widget_new (id_gtkwidget2, NULL);
	_button->width = _width; _button->height = _height; _button->fixed_size = 1;
	_button->data = (void *) malloc (sizeof (tag3)); if (_button->data == NULL) return NULL; tag3 *_tag = (tag3 *) _button->data;
	_tag->type = _type; _tag->text1 = _value1 != NULL ? strdup (_value1) : NULL; _tag->text2 = _value2 != NULL ? strdup (_value2) : NULL;
	_button->draw = button3_draw; _button->forget = button3_forget;
	return (GtkWidget *) _button;
}
static gboolean button3_draw (GtkWidget2 *_from, GdkEventExpose *_event) {
	GdkWindow *_canvas = gtk_widget_get_window ((GtkWidget *) _from); GdkGC *_paint = gdk_gc_new ((GdkDrawable *) _canvas); tag3 *_tag = (tag3 *) _from->data;
	if (_tag->type == 1) {
		if (_tag->text1 == NULL) {g_object_unref (_paint); return TRUE;}
		GdkPixbuf *_picture = picture2 (_tag->text1, -1, -1); if (_picture == NULL) {g_object_unref (_paint); return TRUE;}
		gdk_draw_pixbuf ((GdkDrawable *) _canvas, _paint, _picture, 0, 0, 0, 0, -1, -1, GDK_RGB_DITHER_MAX, 0, 0);
		g_object_unref (_picture);
	}
	g_object_unref (_paint); return TRUE;
}
static void button3_refresh (GtkWidget2 *_button) {
	GdkWindow *_canvas = gtk_widget_get_window ((GtkWidget *) _button);
	GdkRectangle _area; _area.x = 0; _area.y = 0; _area.width = _button->width; _area.height = _button->height;
	gdk_window_invalidate_rect (_canvas, &_area, TRUE); gdk_window_clear (_canvas); gtk_widget_show ((GtkWidget *) _button);
	gdk_window_clear (_canvas); button3_draw (_button, NULL);
	
}
static void button3_forget (GtkWidget2 *_from) {
	if (_from->label != NULL) {free (_from->label); _from->label = NULL;}
	if (_from->data == NULL) return; tag3 *_tag = (tag3 *) _from->data;
	if (_tag->text1 != NULL) free (_tag->text1); if (_tag->text2 != NULL) free (_tag->text2); free (_from->data); _from->data = NULL;
}

int main (int argc, char *argv[]) {
	// Properties.
	printf ("[my-web-browser] Hi.\n"); gtk_init (&argc, &argv);
	id_gtkwidget2 = gtk_widget_2_get_id ();
	page = NULL; place = NULL;
	GdkScreen *screen = gdk_screen_get_default (); screen_width = gdk_screen_get_width (screen); screen_height = gdk_screen_get_height (screen);
	screen_span = screen_width < screen_height ? screen_width / 20 : screen_height / 20;
	command = (char **) malloc (sizeof (char *) * 2); if (command == NULL) {printf ("[my-web-browser] More memory, please! T_T\n"); exit (1);}
	command[0] = "/bin/bash"; command[1] = NULL;
	n_pages = 0; this_page = -1; wanted_page = -1;
	to = NULL; stop = 0;
	
	// Main casts.
	overall = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_resize ((GtkWindow *) overall, screen_width / 2, screen_height / 2);
	gtk_window_set_title ((GtkWindow *) overall, "my-web-browser");
	g_signal_connect (overall, "delete_event", (GCallback) popup_quit, NULL);
	overall1 = gtk_vbox_new (FALSE, 0); gtk_container_add ((GtkContainer *) overall, overall1);
	pages = gtk_hbox_new (FALSE, 0); gtk_box_pack_start ((GtkBox *) overall1, pages, FALSE, FALSE, 0);
	gtk_box_pack_start ((GtkBox *) pages, button2 ('p', screen_span, screen_span, "/etc/pictures/list.svg", NULL, 1), FALSE, FALSE, 0);
	gtk_box_pack_start ((GtkBox *) pages, gtk_label_new ("(Unlabeled)"), TRUE, FALSE, 0); pages_label = chase (pages, 1, 1);
	gtk_box_pack_start ((GtkBox *) pages, button2 ('p', screen_span, screen_span, "/etc/pictures/plus.svg", NULL, 2), FALSE, FALSE, 0);
	gtk_box_pack_start ((GtkBox *) pages, button2 ('p', screen_span, screen_span, "/etc/pictures/minus.svg", NULL, 3), FALSE, FALSE, 0);
	new_page (NULL, NULL);
	
	GtkAccelGroup *keybindings = gtk_accel_group_new ();
	gtk_window_add_accel_group ((GtkWindow *) overall, keybindings);
	
	gtk_accel_group_connect (keybindings, GDK_KEY_Down, GDK_CONTROL_MASK | GDK_SHIFT_MASK, GTK_ACCEL_VISIBLE, g_cclosure_new ((GCallback) popup_page_list_from_keypress, NULL, NULL));
	gtk_accel_group_connect (keybindings, GDK_KEY_plus, GDK_CONTROL_MASK | GDK_SHIFT_MASK, GTK_ACCEL_VISIBLE, g_cclosure_new ((GCallback) new_page_keybinding, NULL, NULL));
	gtk_accel_group_connect (keybindings, GDK_KEY_underbar, GDK_CONTROL_MASK | GDK_SHIFT_MASK, GTK_ACCEL_VISIBLE, g_cclosure_new ((GCallback) remove_page_keybinding, NULL, NULL));
	gtk_accel_group_connect (keybindings, GDK_KEY_X, GDK_CONTROL_MASK | GDK_SHIFT_MASK, GTK_ACCEL_VISIBLE, g_cclosure_new ((GCallback) ask_quit_keybinding, NULL, NULL));
	
	gtk_accel_group_connect (keybindings, GDK_KEY_Left, GDK_CONTROL_MASK | GDK_SHIFT_MASK, GTK_ACCEL_VISIBLE, g_cclosure_new ((GCallback) page_minus1, NULL, NULL));
	gtk_accel_group_connect (keybindings, GDK_KEY_Right, GDK_CONTROL_MASK | GDK_SHIFT_MASK, GTK_ACCEL_VISIBLE, g_cclosure_new ((GCallback) page_plus1, NULL, NULL));
	
	// Action!
	gtk_widget_show_all (overall);
	gtk_main ();
	
	// Dispose.
	printf ("[my-web-browser] Bye.\n"); return 0;
}

static void show_nth_page (int _nth, int _from) {
	if ((_nth < 0) || (_nth >= n_pages)) return;
	
	printf ("[my-web-browser] %d out, %d in\n", this_page >= 0 ? page[this_page] : 0, page[_nth]);
	if ((this_page >= 0) && (_from != 1)) {g_object_ref (page[this_page]); gtk_container_remove ((GtkContainer *) overall1, page[this_page]);}
	gtk_box_pack_start ((GtkBox *) overall1, page[_nth], TRUE, TRUE, 0); gtk_widget_show_all (page[_nth]); //if (this_page >= 0) g_object_unref (page[_nth]);
	
	GList *_list = gtk_container_get_children ((GtkContainer *) page[_nth]); if (_list == NULL) {printf ("[my-web-browser] More memory, please! T_T\n"); quit_program (NULL, NULL);}
	gtk_widget_grab_focus ((GtkWidget *) g_list_nth_data (_list, 0));
	g_list_free (_list);
	this_page = _nth;
	gtk_label_set_text ((GtkLabel *) pages_label, place[this_page]);
}

static void *ask_quit_keybinding (void *data) {popup_quit (NULL, NULL);}
static void *quit_program (GtkWidget *widget, void *data) {
	free (page);
	int _nth; for (_nth = 0; _nth < n_pages; _nth++) free (place[_nth]);
	free (place);
	free (command);
	gtk_main_quit ();
}

static void *new_page (GtkWidget *widget, void *data) {
	// Expand page list
	GtkWidget **_page = (GtkWidget **) malloc (sizeof (GtkWidget *) * (n_pages + 1));
	if (_page == NULL) {printf ("[my-web-browser] More memory, please! T_T\n"); quit_program (NULL, NULL);}
	int _nth; for (_nth = 0; _nth < n_pages; _nth++) {
		_page[_nth] = page[_nth];
	}
	_page[_nth] = gtk_vbox_new (FALSE, 0); printf ("[my-web-browser] New page: %d\n", _page[_nth]);
	if (page != NULL) free (page);
	page = _page;
	
	// Expand place list
	char **_place = (char **) malloc (sizeof (char *) * (n_pages + 1));
	if (_place == NULL) {printf ("[my-web-browser] More memory, please! T_T\n"); free (_page); quit_program (NULL, NULL);}
	for (_nth = 0; _nth < n_pages; _nth++) {
		_place[_nth] = place[_nth];
	}
	_place[_nth] = strdup ("(New session)");
	if (place != NULL) free (place);
	place = _place;
	
	GtkWidget *_toolbar = gtk_hbox_new (FALSE, 0); gtk_box_pack_start ((GtkBox *) _page[_nth], _toolbar, FALSE, FALSE, 0);
	gtk_box_pack_start ((GtkBox *) _toolbar, button2 ('p', screen_span, screen_span, "/etc/pictures/paper.svg", NULL, 11), FALSE, FALSE, 0);
	gtk_box_pack_start ((GtkBox *) _toolbar, button2 ('p', screen_span, screen_span, "/etc/pictures/folder.svg", NULL, 13), FALSE, FALSE, 0);
	gtk_box_pack_start ((GtkBox *) _toolbar, button2 ('p', screen_span, screen_span, "/etc/pictures/floppy.svg", NULL, 0), FALSE, FALSE, 0);
	
	GtkWidget *_content = gtk_scrolled_window_new (NULL, NULL); gtk_box_pack_start ((GtkBox *) _page[_nth], _content, TRUE, TRUE, 0);
	gtk_scrolled_window_add_with_viewport ((GtkScrolledWindow *) _content, webkit_web_view_new ());
	
	n_pages++; show_nth_page (n_pages - 1, 0);
}
static void *new_page_keybinding (void *data) {new_page (NULL, NULL);}

static void *remove_page (GtkWidget *widget, void *data) {
	GtkWidget *_this_page = page[this_page]; char *_this_place = place[this_page];
	
	// Shrink page list
	GtkWidget **_page = (GtkWidget **) malloc (sizeof (GtkWidget *) * (n_pages - 1));
	if (_page == NULL) {printf ("[my-web-browser] More memory, please! T_T\n"); quit_program (NULL, NULL);}
	int _nth; int _nth1 = 0; for (_nth = 0; _nth < n_pages; _nth++) {
		if (_nth == this_page) continue;
		_page[_nth1] = page[_nth]; _nth1++;
	}
	webkit_web_back_forward_list_clear (webkit_web_view_get_back_forward_list ((WebKitWebView *) session (this_page)));
	gtk_widget_destroy (_this_page);
	free (page);
	page = _page;
	
	// Shrink place list
	char **_place = (char **) malloc (sizeof (char *) * (n_pages - 1));
	if (_place == NULL) {printf ("[my-web-browser] More memory, please! T_T\n"); free (_page); quit_program (NULL, NULL);}
	_nth1 = 0; for (_nth = 0; _nth < n_pages; _nth++) {
		if (_nth == this_page) continue;
		_place[_nth1] = place[_nth]; _nth1++;
	}
	free (_this_place);
	free (place);
	place = _place;
	
	n_pages--; show_nth_page (this_page <= 0 ? 0 : this_page - 1, 1);
}
static void *remove_page_keybinding (void *data) {remove_page (NULL, page[this_page]);}

static void *remove_this_page (GtkWidget *widget, void *data) {remove_page (NULL, page[this_page]);}

static void *show_page (GtkWidget *widget, void *data) {
	// Sanity check.
	if (data == NULL) return NULL;
	int nth; for (nth = 0; nth < n_pages; nth++) {if (data == (void *) page[nth]) break;} if (nth >= n_pages) return NULL;
	
	g_object_ref (page[this_page]); gtk_container_remove ((GtkContainer *) overall1, page[this_page]);
	gtk_box_pack_start ((GtkBox *) overall1, page[nth], TRUE, TRUE, 0); gtk_widget_show_all (page[nth]); g_object_unref (page[nth]);
	
	GList *_list = gtk_container_get_children ((GtkContainer *) page[nth]); if (_list == NULL) {printf ("[my-web-browser] More memory, please! T_T\n"); quit_program (NULL, NULL);}
	gtk_widget_destroy (gtk_widget_get_toplevel (widget));
	gtk_widget_grab_focus ((GtkWidget *) g_list_nth_data (_list, 0));
	g_list_free (_list);
	this_page = nth;
}

static void *show_label (GtkWidget *widget, void *data) {
	if (data == NULL) return NULL;
	if (page[this_page] == gtk_widget_get_parent ((GtkWidget *) data)) gtk_label_set_text ((GtkLabel *) pages_label, place[this_page]);
}

static void *page_minus1 (void *data) {show_nth_page (this_page <= 0 ? n_pages - 1 : this_page - 1, 0); g_object_unref (page[this_page]);}
static void *page_plus1 (void *data) {show_nth_page (this_page >= n_pages - 1 ? 0 : this_page + 1, 0); g_object_unref (page[this_page]);}

static gboolean content (GtkWidget2 *_gw2, GdkEventExpose *_event) {
	GdkWindow *_w = gtk_widget_get_window ((GtkWidget *) _gw2); tag1 *_t = (tag1 *) _gw2->data;
	GdkGC *_g = gdk_gc_new ((GdkDrawable *) _w);
	
	if (_t->selected == 1) rectangle (_w, TRUE, 0, 0, _gw2->width, _gw2->height, 65535, 32767, 32767, 65535, 32767, 32767);
	else rectangle (_w, TRUE, 0, 0, _gw2->width, _gw2->height, 65535, 65535, 65535, 65535, 65535, 65535);
	
	char *_icon = libtext_connect (3, "/etc/pictures/", strcmp (_t->type, "Directory") == 0 ? "folder" : "paper", ".svg");
	picture (_w, _icon, 0, 0, screen_span, screen_span);
	free (_icon);
	
	caption (_w, _t->name != NULL ? _t->name : "", screen_span, 0, 0, 0, 0);
	
	return TRUE;
}
static void rectangle (GdkWindow *_canvas, gboolean _fill, int _left, int _top, int _right, int _bottom, unsigned int _bg_red, unsigned int _bg_green, unsigned int _bg_blue, unsigned int _fg_red, unsigned int _fg_green, unsigned int _fg_blue) {
	GdkGC *_g = gdk_gc_new ((GdkDrawable *) _canvas);
	GdkColor _ca; _ca.red = _fg_red; _ca.green = _fg_green; _ca.blue = _fg_blue; gdk_gc_set_rgb_fg_color (_g, &_ca);
	GdkColor _cb; _cb.red = _bg_red; _cb.green = _bg_green; _cb.blue = _bg_blue; gdk_gc_set_rgb_bg_color (_g, &_cb);
	gdk_draw_rectangle ((GdkDrawable *) _canvas, _g, _fill, _left, _top, _right, _bottom);
	g_object_unref (_g);
}
static void picture (GdkWindow *_canvas, const char *_address, int _left, int _top, int _right, int _bottom) {
	if (_address == NULL) return;
	GdkGC *_g = gdk_gc_new ((GdkDrawable *) _canvas);
	gdk_draw_pixbuf ((GdkDrawable *) _canvas, _g, gdk_pixbuf_new_from_file_at_scale (_address, _right, _bottom, TRUE, NULL), _left, _top, _left, _top, -1, -1, GDK_RGB_DITHER_MAX, 0, 0);
	g_object_unref (_g);
}
static GdkPixbuf *picture2 (const char *_address, int _width, int _height) {
	if (_address == NULL) return NULL;
	GdkPixbuf *_picture = gdk_pixbuf_new_from_file (_address, NULL); if (_picture == NULL) return NULL;
	int _width1 = gdk_pixbuf_get_width (_picture); int _height1 = gdk_pixbuf_get_height (_picture);
	if (_width >= 0) _width1 = _width; if (_height >= 0) _height1 = _height;
	GdkPixbuf *_picture1 = gdk_pixbuf_new_from_file_at_scale (_address, _width1, _height1, TRUE, NULL);
	g_object_unref (_picture); return _picture1;
}
static void caption (GdkWindow *_canvas, const char *_text, int _left, int _top, unsigned int _red, unsigned int _green, unsigned int _blue) {
	GdkGC *_g = gdk_gc_new ((GdkDrawable *) _canvas);
	GdkColor _c; _c.red = _red; _c.green = _green; _c.blue = _blue; gdk_gc_set_rgb_fg_color (_g, &_c);
	PangoContext *_p = gdk_pango_context_get (); PangoLayout *_l = pango_layout_new (_p);
	pango_layout_set_markup (_l, _text, -1);
	gdk_draw_layout ((GdkDrawable *) _canvas, _g, _left, _top, _l);
	g_object_unref (_l); g_object_unref (_p); g_object_unref (_g);
}
static gboolean when_clicked (GtkWidget *widget, GdkEvent *event, void *data) {
	GtkWidget2 *_item = (GtkWidget2 *) widget; tag1 *_tag = (tag1 *) _item->data; GdkEventButton *_event = (GdkEventButton *) event; GdkWindow *_content = _event->window;
	
	if ((_event->x < 0) || (_event->x >= gdk_window_get_width (_content)) || (_event->y < 0) || (_event->y >= gdk_window_get_height (_content))) return FALSE;
	
	if (_event->x < screen_span) {_tag->selected = _tag->selected == 0 ? 1 : 0; content (_item, NULL);}
	else {
		if (_event->button == 1) {
			char *_here = (char *) gtk_label_get_text ((GtkLabel *) pages_label);
			char *_directory = libtext_connect (3, strcmp (_here, "/") == 0 ? "" : _here, "/", (char *) ((tag1 *) ((GtkWidget2 *) widget)->data)->name); if (_directory == NULL) return FALSE;
			do_to_session (widget, widget, 1); do_to_session (widget, (void *) _directory, 2);
			free (_directory);
		}
		else if (_event->button == 3) {popup_what (3, widget, _tag);}
	}
	
	return TRUE;
}
static void forget (GtkWidget2 *_gw2) {
	tag1 *_t = (tag1 *) _gw2->data; if (_t == NULL) return;
	if (_t->address != NULL) free (_t->address);
	if (_t->name != NULL) free (_t->name);
	if (_t->type != NULL) free (_t->type);
	free (_t); _gw2->data = NULL;
}

static void *popup_page_list_from_keypress (void *_parcel) {popup_what (1, overall, overall);}

static void *popup_pages (GtkWidget *widget, void *data) {popup_what (16, widget, widget);}
static void *popup_select (GtkWidget *widget, void *data) {popup_what (1, widget, widget);}
static void *popup_quit (GtkWidget *widget, void *data) {popup_what (17, widget, widget);}
static void *popup_what (int _type, GtkWidget *_from, void *_parcel) {
	if (_parcel == NULL) return NULL;
	
	/*
		1 = Page list
		11 = New session?
		12 = Open file?
		13 = Save as?
	*/
	
	int _x = screen_width / 2; int _y = screen_height / 2;
	GtkWidget *_w = popup (_x, _y, _x / 2, _y / 2, NULL);
	GtkWidget *_b = gtk_vbox_new (FALSE, 0); gtk_container_add ((GtkContainer *) _w, _b);
	
	if (_type == 1) {
		GtkWidget *_box1 = gtk_scrolled_window_new (NULL, NULL); gtk_box_pack_start ((GtkBox *) _b, _box1, TRUE, TRUE, 0);
		GtkWidget *_box2 = gtk_vbox_new (FALSE, 0); gtk_scrolled_window_add_with_viewport ((GtkScrolledWindow *) _box1, _box2);
		int nth; for (nth = 0; nth < n_pages; nth++) {
			gtk_box_pack_start ((GtkBox *) _box2, button (0, place[nth], show_page, (void *) page[nth]), TRUE, FALSE, 0);
		}
	}
	else if (_type == 11) {
		gtk_box_pack_start ((GtkBox *) _b, button2 ('t', screen_width / 2, screen_span, "Forget current session and start new one", NULL, 0), FALSE, FALSE, 0);
		gtk_box_pack_start ((GtkBox *) _b, button2 ('t', screen_width / 2, screen_span, "Are you sure?", NULL, 0), FALSE, FALSE, 0);
		gtk_box_pack_start ((GtkBox *) _b, button2 ('t', screen_width / 2, screen_span, "Yes.", NULL, 12), FALSE, FALSE, 0);
	}
	else if (_type == 12) {
		gtk_box_pack_start ((GtkBox *) _b, button2 ('t', screen_width / 2, screen_span, "Load file", NULL, 0), FALSE, FALSE, 0);
		gtk_box_pack_start ((GtkBox *) _b, gtk_entry_new (), FALSE, FALSE, 0);
		gtk_box_pack_start ((GtkBox *) _b, button2 ('t', screen_width / 2, screen_span, "Unsaved data will be lost.", NULL, 0), FALSE, FALSE, 0);
		gtk_box_pack_start ((GtkBox *) _b, button2 ('t', screen_width / 2, screen_span, "Load.", NULL, 14), FALSE, FALSE, 0);
		if (strcmp (place[this_page], "(New session)") != 0) gtk_entry_set_text ((GtkEntry *) chase (_b, 1, 1), place[this_page]);
	}
	else if (_type == 13) {
		gtk_box_pack_start ((GtkBox *) _b, button2 ('t', screen_width / 2, screen_span, "Save as", NULL, 0), FALSE, FALSE, 0);
		gtk_box_pack_start ((GtkBox *) _b, gtk_entry_new (), FALSE, FALSE, 0);
		gtk_box_pack_start ((GtkBox *) _b, button2 ('t', screen_width / 2, screen_span, "If the file already exists, content will be overwritten.", NULL, 0), FALSE, FALSE, 0);
		gtk_box_pack_start ((GtkBox *) _b, button2 ('t', screen_width / 2, screen_span, "Save.", NULL, 16), FALSE, FALSE, 0);
		if (strcmp (place[this_page], "(New session)") != 0) gtk_entry_set_text ((GtkEntry *) chase (_b, 1, 1), place[this_page]);
	}
	else if (_type == 17) {
		gtk_box_pack_start ((GtkBox *) _b, gtk_label_new ("Are you sure you want to quit?"), TRUE, FALSE, 0);
		gtk_box_pack_start ((GtkBox *) _b, button2 ('t', screen_width / 2, screen_span, "Yes. Quit.", NULL, -2), FALSE, FALSE, 0);
	}
	gtk_box_pack_start ((GtkBox *) _b, button2 ('t', screen_width / 2, screen_span, "No, nothing.", NULL, -1), FALSE, FALSE, 0);
	
	gtk_widget_show_all (_w);
}

static void *do_to_session (GtkWidget *_from, void *_parcel, int _order) {
	if (_parcel == NULL) {
		if ((_order >= 7) && (_order <= 9)) {gdk_threads_enter (); gtk_widget_destroy ((GtkWidget *) _parcel); free (to); to = NULL; gdk_threads_leave (); g_thread_exit (NULL);}
		return NULL;
	}
	
	/*
		1 = Remove all item(s) from current session
		2 = Add item(s) in current session
	*/
	if (_order == 1) {
		GList *_list = gtk_container_get_children ((GtkContainer *) session (this_page)); if (_list == NULL) return NULL;
		int _nth; for (_nth = 0; _nth < g_list_length (_list); _nth++) gtk_widget_destroy ((GtkWidget *) g_list_nth_data (_list, _nth));
		g_list_free (_list);
	}
	else if (_order == 2) {
		char *_place = libfile_refine_address ((char *) _parcel); if (_place == NULL) return NULL;
		DIR *_place1 = opendir (_place); if (_place1 == NULL) {free (_place); return NULL;} struct dirent *_file;
		while ((_file = readdir (_place1)) != NULL) {
			if ((strcmp (_file->d_name, ".") == 0) || (strcmp (_file->d_name, "..") == 0)) continue;
			GtkWidget *_file1 = gtk_widget_new (id_gtkwidget2, NULL); GtkWidget2 *_file2 = (GtkWidget2 *) _file1;
		
			_file2->width = screen_width / 2; _file2->height = screen_span; _file2->data = (void *) malloc (sizeof (tag1)); tag1 *_data = (tag1 *) _file2->data;
			char *_place2 = libtext_connect (3, strcmp (_place, "/") == 0 ? "" : _place, "/", _file->d_name);
			_data->address = _place2; _data->name = strdup (_file->d_name); _data->type = libfile_status (_place2, 't'); _data->selected = 0; _data->marked = 0;
			_file2->draw = content; _file2->forget = forget;
		
			g_signal_connect (_file1, "button_release_event", (GCallback) when_clicked, NULL);
			gtk_box_pack_start ((GtkBox *) session (this_page), _file1, TRUE, FALSE, 0);
		}
		gtk_widget_show_all (session (this_page));
		if (this_page >= 0) {if (place[this_page] != NULL) free (place[this_page]); place[this_page] = strdup (_place);}
		gtk_label_set_text ((GtkLabel *) pages_label, _place);
		closedir (_place1); free (_place);
	}
	else gtk_widget_destroy ((GtkWidget *) _parcel);
}
