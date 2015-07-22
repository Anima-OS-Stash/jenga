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

// gcc -O3 -std=gnu99 $(pkg-config --cflags --libs gtkwidget2 libfile) my-file-manager.c

#include <gtkwidget2.h>
#include <libfile.h>
#include <gdk/gdkkeysyms.h>
#include <malloc.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>

typedef struct tag1 {char *address, *name, *type; int selected, marked;} tag1;
typedef struct tag2 {GtkWidget *window; tag1 *tag1;} tag2;

static GtkWidget *overall, *overall1, *pages, *pages_list, *pages_label, *pages_new, *pages_remove, **page;

static unsigned long id_gtkwidget2; static int screen_width, screen_height, screen_span; static char **command; static char **place; static int n_pages, this_page, wanted_page; static GdkGeometry sizing; static char *to; static char stop;

static GtkWidget *chase (GtkWidget *_from, int _spots, ...);
static GtkWidget *session (int _nth);

static GtkWidget *button (int _type, const char *_id, void *_when_clicked, void *_data);
static GtkWidget *popup (int _width, int _height, int _x, int _y, GtkWidget *_content);

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

static void *popup_pages (GtkWidget *widget, void *data);
static void *popup_select (GtkWidget *widget, void *data);
static void *popup_selected (GtkWidget *widget, void *data);
static void *popup_ask_copy_selected (GtkWidget *widget, void *data);
static void *popup_ask_move_selected (GtkWidget *widget, void *data);
static void *popup_ask_erase_selected (GtkWidget *widget, void *data);
static void *popup_ask_copy_context (GtkWidget *widget, void *data);
static void *popup_ask_move_context (GtkWidget *widget, void *data);
static void *popup_ask_erase_context (GtkWidget *widget, void *data);
static void *popup_copy_selected (GtkWidget *widget, void *data);
static void *popup_move_selected (GtkWidget *widget, void *data);
static void *popup_erase_selected (GtkWidget *widget, void *data);
static void *popup_copy_context (GtkWidget *widget, void *data);
static void *popup_move_context (GtkWidget *widget, void *data);
static void *popup_erase_context (GtkWidget *widget, void *data);
static void *popup_quit (GtkWidget *widget, void *data);
static void *popup_what (int _type, GtkWidget *_from, void *_parcel);

static void *do_up (GtkWidget *widget, void *data);
static void *do_reload (GtkWidget *widget, void *data);
static void *do_select_all (GtkWidget *widget, void *data);
static void *do_deselect_all (GtkWidget *widget, void *data);
static void *do_invert_selection (GtkWidget *widget, void *data);
static void *do_copy_selected (void *data);
static void *do_move_selected (void *data);
static void *do_erase_selected (void *data);
static void *do_copy_context (void *data);
static void *do_move_context (void *data);
static void *do_erase_context (void *data);
static void *do_withdraw (GtkWidget *widget, void *data);
static void *do_stop (GtkWidget *widget, void *data);
static void *do_terminal (GtkWidget *widget, void *data);
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
	return chase (page[_nth], 4, 0, 1, 0, 0);
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

int main (int argc, char *argv[]) {
	// Properties.
	printf ("[my-file-manager] Hi.\n"); gtk_init (&argc, &argv);
	page = NULL;
	id_gtkwidget2 = gtk_widget_2_get_id ();
	GdkScreen *screen = gdk_screen_get_default (); screen_width = gdk_screen_get_width (screen); screen_height = gdk_screen_get_height (screen);
	screen_span = screen_width < screen_height ? screen_width / 20 : screen_height / 20;
	command = (char **) malloc (sizeof (char *) * 2); if (command == NULL) {printf ("[my-file-manager] More memory, please! T_T\n"); exit (1);}
	command[0] = "/bin/bash"; command[1] = NULL;
	place = NULL;
	n_pages = 0; this_page = -1; wanted_page = -1;
	to = NULL; stop = 0;
	
	// Main casts.
	overall = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_resize ((GtkWindow *) overall, screen_width / 2, screen_height / 2);
	gtk_window_set_title ((GtkWindow *) overall, "my-file-manager");
	g_signal_connect (overall, "delete_event", (GCallback) popup_quit, NULL);
	overall1 = gtk_vbox_new (FALSE, 0); gtk_container_add ((GtkContainer *) overall, overall1);
	pages = gtk_hbox_new (FALSE, 0); gtk_box_pack_start ((GtkBox *) overall1, pages, FALSE, FALSE, 0);
	pages_list = button (1, "view-list-symbolic", popup_pages, NULL); gtk_box_pack_start ((GtkBox *) pages, pages_list, FALSE, FALSE, 0);
	pages_label = gtk_label_new ("(Unlabeled)"); gtk_box_pack_start ((GtkBox *) pages, pages_label, TRUE, FALSE, 0);
	pages_new = button (1, "gtk-add", new_page, NULL); gtk_box_pack_start ((GtkBox *) pages, pages_new, FALSE, FALSE, 0);
	pages_remove = button (1, "gtk-remove", remove_this_page, NULL); gtk_box_pack_start ((GtkBox *) pages, pages_remove, FALSE, FALSE, 0);
	new_page (NULL, NULL);
	
	GtkAccelGroup *keybindings = gtk_accel_group_new ();
	gtk_accel_group_connect (keybindings, GDK_KEY_N, GDK_CONTROL_MASK | GDK_SHIFT_MASK, GTK_ACCEL_VISIBLE, g_cclosure_new ((GCallback) new_page_keybinding, NULL, NULL));
	gtk_accel_group_connect (keybindings, GDK_KEY_Left, GDK_CONTROL_MASK | GDK_SHIFT_MASK, GTK_ACCEL_VISIBLE, g_cclosure_new ((GCallback) page_minus1, NULL, NULL));
	gtk_accel_group_connect (keybindings, GDK_KEY_Right, GDK_CONTROL_MASK | GDK_SHIFT_MASK, GTK_ACCEL_VISIBLE, g_cclosure_new ((GCallback) page_plus1, NULL, NULL));
	gtk_accel_group_connect (keybindings, GDK_KEY_W, GDK_CONTROL_MASK | GDK_SHIFT_MASK, GTK_ACCEL_VISIBLE, g_cclosure_new ((GCallback) remove_page_keybinding, NULL, NULL));
	gtk_accel_group_connect (keybindings, GDK_KEY_Q, GDK_CONTROL_MASK | GDK_SHIFT_MASK, GTK_ACCEL_VISIBLE, g_cclosure_new ((GCallback) ask_quit_keybinding, NULL, NULL));
	gtk_window_add_accel_group ((GtkWindow *) overall, keybindings);
	
	// Action!
	gtk_widget_show_all (overall);
	gtk_main ();
	
	// Dispose.
	printf ("[my-file-manager] Bye.\n"); return 0;
}

static void show_nth_page (int _nth, int _from) {
	if ((_nth < 0) || (_nth >= n_pages)) return;
	
	printf ("[my-file-manager] %d out, %d in\n", this_page >= 0 ? page[this_page] : 0, page[_nth]);
	if ((this_page >= 0) && (_from != 1)) {g_object_ref (page[this_page]); gtk_container_remove ((GtkContainer *) overall1, page[this_page]);}
	gtk_box_pack_start ((GtkBox *) overall1, page[_nth], TRUE, TRUE, 0); gtk_widget_show_all (page[_nth]); //if (this_page >= 0) g_object_unref (page[_nth]);
	
	GList *_list = gtk_container_get_children ((GtkContainer *) page[_nth]); if (_list == NULL) {printf ("[my-file-manager] More memory, please! T_T\n"); quit_program (NULL, NULL);}
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
	GtkWidget **_page = (GtkWidget **) malloc (sizeof (GtkWidget *) * (n_pages + 1)); if (_page == NULL) {printf ("[my-file-manager] More memory, please! T_T\n"); quit_program (NULL, NULL);}
	int nth = 0; while (nth < n_pages) {_page[nth] = page[nth]; nth++;}
	_page[nth] = gtk_vbox_new (FALSE, 0); printf ("[my-file-manager] New page: %d\n", _page[nth]);
	page = _page;
	
	char **_place = (char **) malloc (sizeof (char *) * (n_pages + 1));  if (_place == NULL) {printf ("[my-file-manager] More memory, please! T_T\n"); free (_page); quit_program (NULL, NULL);}
	nth = 0; while (nth < n_pages) {_place[nth] = place[nth]; nth++;}
	_place[nth] = n_pages >= 1 ? strdup (place[this_page]) : strdup ("/");
	place = _place;
	
	char *_where = n_pages >= 1 ? strdup (place[this_page]) : strdup ("/");
	GtkWidget *_page0 = gtk_vbox_new (FALSE, 0); gtk_box_pack_start ((GtkBox *) _page[nth], _page0, TRUE, TRUE, 0);
	GtkWidget *_toolbar = gtk_hbox_new (FALSE, 0); gtk_box_pack_start ((GtkBox *) _page0, _toolbar, FALSE, FALSE, 0);
	GtkWidget *_button_up = button (1, "up", do_up, NULL); gtk_box_pack_start ((GtkBox *) _toolbar, _button_up, FALSE, FALSE, 0);
	GtkWidget *_button_reload = button (1, "stock-refresh", do_reload, NULL); gtk_box_pack_start ((GtkBox *) _toolbar, _button_reload, FALSE, FALSE, 0);
	GtkWidget *_button_select = button (1, "edit-select-all", popup_select, NULL); gtk_box_pack_start ((GtkBox *) _toolbar, _button_select, FALSE, FALSE, 0);
	GtkWidget *_button_process = button (1, "system-run", popup_selected, NULL); gtk_box_pack_start ((GtkBox *) _toolbar, _button_process, FALSE, FALSE, 0);
	GtkWidget *_button_terminal = button (1, "terminal", do_terminal, NULL); gtk_box_pack_start ((GtkBox *) _toolbar, _button_terminal, FALSE, FALSE, 0);
	GtkWidget *_page1 = gtk_scrolled_window_new (NULL, NULL); gtk_box_pack_start ((GtkBox *) _page0, _page1, TRUE, TRUE, 0);
	GtkWidget *_page2 = gtk_vbox_new (FALSE, 0); gtk_scrolled_window_add_with_viewport ((GtkScrolledWindow *) _page1, _page2);
	
	char *_type = libfile_status (_where, 't'); if (_type == NULL) {printf ("[my-file-manager] More memory, please! T_T\n"); quit_program (NULL, NULL);}
	char is_directory = strcmp (_type, "Directory") == 0 ? 'y' : 'n'; free (_type);
	n_pages++; show_nth_page (n_pages - 1, 0);
	if (is_directory == 'y') do_to_session (widget, (void *) _where, 2);
	free (_where);
}
static void *new_page_keybinding (void *data) {new_page (NULL, NULL);}

static void *remove_page (GtkWidget *widget, void *data) {
	if (n_pages <= 1) {free (place[this_page]); quit_program (NULL, NULL); return NULL;}
	
	GtkWidget **_page = (GtkWidget **) malloc (sizeof (GtkWidget *) * (n_pages - 1)); if (_page == NULL) {printf ("[my-file-manager] More memory, please! T_T\n"); quit_program (NULL, NULL);}
	int nth = 0, nth1 = 0; while (nth < n_pages) {
		if (nth == this_page) gtk_widget_destroy (page[nth]);
		else {_page[nth1] = page[nth]; nth1++;}
		nth++;
	}
	free (page); page = _page;
	
	char **_place = (char **) malloc (sizeof (char *) * (n_pages - 1)); if (_place == NULL) {printf ("[my-file-manager] More memory, please! T_T\n"); quit_program (NULL, NULL);}
	nth = 0, nth1 = 0; while (nth < n_pages) {
		if (nth == this_page) free (place[nth]);
		else {_place[nth1] = place[nth]; nth1++;}
		nth++;
	}
	free (place); place = _place;
	
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
	
	GList *_list = gtk_container_get_children ((GtkContainer *) page[nth]); if (_list == NULL) {printf ("[my-file-manager] More memory, please! T_T\n"); quit_program (NULL, NULL);}
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

static void *popup_pages (GtkWidget *widget, void *data) {
	GdkWindow *_soul = gtk_button_get_event_window ((GtkButton *) pages_list);
	int _x, _y; gdk_window_get_deskrelative_origin (_soul, &_x, &_y);
	int _width = gdk_window_get_width (_soul), _height = gdk_window_get_height (_soul);
	GtkWidget *_w = popup (screen_width / 2, screen_height / 2, _x, _y + _height, NULL);
	GtkWidget *_overall1 = gtk_scrolled_window_new (NULL, NULL); gtk_container_add ((GtkContainer *) _w, _overall1);
	GtkWidget *_overall2 = gtk_vbox_new (FALSE, 0); gtk_scrolled_window_add_with_viewport ((GtkScrolledWindow *) _overall1, _overall2);
	int nth; for (nth = 0; nth < n_pages; nth++) {
		GList *_list = gtk_container_get_children ((GtkContainer *) page[nth]); if (_list == NULL) {printf ("[my-file-manager] More memory, please! T_T\n"); quit_program (NULL, NULL);}
		GtkWidget *_b = button (0, place[nth], show_page, (void *) page[nth]); gtk_box_pack_start ((GtkBox *) _overall2, _b, TRUE, FALSE, 0);
		g_list_free (_list);
	}
	gtk_widget_show_all (_w);
}
static void *popup_select (GtkWidget *widget, void *data) {popup_what (1, widget, widget);}
static void *popup_selected (GtkWidget *widget, void *data) {popup_what (2, widget, widget);}
static void *popup_ask_copy_selected (GtkWidget *widget, void *data) {popup_what (4, widget, data);}
static void *popup_ask_move_selected (GtkWidget *widget, void *data) {popup_what (5, widget, data);}
static void *popup_ask_erase_selected (GtkWidget *widget, void *data) {popup_what (6, widget, data);}
static void *popup_ask_copy_context (GtkWidget *widget, void *data) {popup_what (7, widget, data);}
static void *popup_ask_move_context (GtkWidget *widget, void *data) {popup_what (8, widget, data);}
static void *popup_ask_erase_context (GtkWidget *widget, void *data) {popup_what (9, widget, data);}
static void *popup_copy_selected (GtkWidget *widget, void *data) {popup_what (10, widget, data);}
static void *popup_move_selected (GtkWidget *widget, void *data) {popup_what (11, widget, data);}
static void *popup_erase_selected (GtkWidget *widget, void *data) {popup_what (12, widget, data);}
static void *popup_copy_context (GtkWidget *widget, void *data) {popup_what (13, widget, data);}
static void *popup_move_context (GtkWidget *widget, void *data) {popup_what (14, widget, data);}
static void *popup_erase_context (GtkWidget *widget, void *data) {popup_what (15, widget, data);}
static void *popup_quit (GtkWidget *widget, void *data) {
	int _x = screen_width / 2; int _y = screen_height / 2;
	GtkWidget *_w = popup (_x, _y, _x / 2, _y / 2, NULL);
	GtkWidget *_box1 = gtk_vbox_new (FALSE, 0); gtk_container_add ((GtkContainer *) _w, _box1);
	GtkWidget *_question = gtk_label_new ("Quit?"); gtk_box_pack_start ((GtkBox *) _box1, _question, FALSE, FALSE, 0);
	GtkWidget *_answer_y = button (1, "emblem-ok-symbolic", quit_program, NULL); gtk_box_pack_start ((GtkBox *) _box1, _answer_y, FALSE, FALSE, 0);
	GtkWidget *_answer_n = button (1, "gtk-cancel", do_withdraw, (void *) _w); gtk_box_pack_start ((GtkBox *) _box1, _answer_n, FALSE, FALSE, 0);
	gtk_widget_show_all (_w);
}
static void *popup_what (int _type, GtkWidget *_from, void *_parcel) {
	if (_parcel == NULL) return NULL;
	
	char *_file1 = NULL;
	if (((_type >= 7) && (_type <= 9)) || ((_type >= 13) && (_type <= 15))) {_file1 = strdup (gtk_window_get_title ((GtkWindow *) _parcel)); if (_file1 == NULL) {gtk_widget_destroy ((GtkWidget *) _parcel); return NULL;}}
	
	if ((_type >= 10) && (_type <= 15)) {
		to = strdup (gtk_entry_get_text ((GtkEntry *) chase ((GtkWidget *) _parcel, 2, 0, 1)));
		if (to == NULL) {gtk_widget_destroy ((GtkWidget *) _parcel); return NULL;}
	}
	if ((_type >= 4) && (_type <= 15)) gtk_widget_destroy ((GtkWidget *) _parcel);
	
	int _x = screen_width / 2; int _y = screen_height / 2;
	GtkWidget *_w = popup (_x, _y, _x / 2, _y / 2, NULL);
	GtkWidget *_b = gtk_vbox_new (FALSE, 0); gtk_container_add ((GtkContainer *) _w, _b);
	
	/*
		1 = Selection of item(s)
		2 = Process of selected item(s)
		3 = Context menu
		4 = Ask whether to copy selected file(s)
		5 = Ask whether to move selected file(s)
		6 = Ask whether to erase selected file(s)
		7 = Context menu: Copy
		8 = Context menu: Move
		9 = Context menu: Erase
		10 = Copying
		11 = Moving
		12 = Erasing
		13 = From context: Copying
		14 = From context: Moving
		15 = From context: Erasing
	*/
	if (_type == 1) {
		gtk_box_pack_start ((GtkBox *) _b, button (0, "Select all", do_select_all, _w), TRUE, FALSE, 0);
		gtk_box_pack_start ((GtkBox *) _b, button (0, "Deselect all", do_deselect_all, _w), TRUE, FALSE, 0);
		gtk_box_pack_start ((GtkBox *) _b, button (0, "Invert selection", do_invert_selection, _w), TRUE, FALSE, 0);
	}
	else if (_type == 2) {
		gtk_box_pack_start ((GtkBox *) _b, button (0, "Copy to", popup_ask_copy_selected, _w), TRUE, FALSE, 0);
		gtk_box_pack_start ((GtkBox *) _b, button (0, "Move to", popup_ask_move_selected, _w), TRUE, FALSE, 0);
		gtk_box_pack_start ((GtkBox *) _b, button (0, "Erase", popup_ask_erase_selected, _w), TRUE, FALSE, 0);
	}
	else if (_type == 3) {
		gtk_window_set_title ((GtkWindow *) _w, ((tag1 *) _parcel)->address);
		gtk_box_pack_start ((GtkBox *) _b, button (0, "Copy to", popup_ask_copy_context, _w), TRUE, FALSE, 0);
		gtk_box_pack_start ((GtkBox *) _b, button (0, "Move to", popup_ask_move_context, _w), TRUE, FALSE, 0);
		gtk_box_pack_start ((GtkBox *) _b, button (0, "Erase", popup_ask_erase_context, _w), TRUE, FALSE, 0);
	}
	else if ((_type >= 4) && (_type <= 6)) {
		gtk_box_pack_start ((GtkBox *) _b, gtk_label_new (_type == 6 ? "Erase" : (_type == 5 ? "Move to" : "Copy to")), TRUE, FALSE, 0);
		gtk_box_pack_start ((GtkBox *) _b, _type == 6 ? gtk_label_new ("Are you sure?") : gtk_entry_new (), TRUE, FALSE, 0);
		gtk_box_pack_start ((GtkBox *) _b, button (0, _type == 6 ? "Erase" : (_type == 5 ? "Move" : "Copy"), _type == 6 ? popup_erase_selected : (_type == 5 ? popup_move_selected : popup_copy_selected), _w), TRUE, FALSE, 0);
	}
	else if ((_type >= 7) && (_type <= 9)) {
		gtk_window_set_title ((GtkWindow *) _w, _file1);
		gtk_box_pack_start ((GtkBox *) _b, gtk_label_new (_type == 9 ? "Erase" : (_type == 8 ? "Move to" : "Copy to")), TRUE, FALSE, 0);
		gtk_box_pack_start ((GtkBox *) _b, _type == 9 ? gtk_label_new ("Are you sure?") : gtk_entry_new (), TRUE, FALSE, 0);
		if (_type != 9) gtk_entry_set_text ((GtkEntry *) chase (_b, 1, 1), _file1);
		gtk_box_pack_start ((GtkBox *) _b, button (0, _type == 9 ? "Erase" : (_type == 8 ? "Move" : "Copy"), _type == 9 ? popup_erase_context : (_type == 8 ? popup_move_context : popup_copy_context), _w), TRUE, FALSE, 0);
	}
	else if ((_type >= 10) && (_type <= 15)) {
		if ((_type >= 13) && (_type <= 15)) gtk_window_set_title ((GtkWindow *) _w, _file1);
		gtk_box_pack_start ((GtkBox *) _b, gtk_label_new (_type == 12 ? "Erasing" : (_type == 11 ? "Moving" : "Copying")), TRUE, FALSE, 0);
		gtk_box_pack_start ((GtkBox *) _b, gtk_label_new (_type >= 13 ? gtk_window_get_title ((GtkWindow *) _w) : "(File)"), TRUE, FALSE, 0);
		if ((_type != 12) && (_type != 15)) {
			gtk_box_pack_start ((GtkBox *) _b, gtk_label_new ("to"), TRUE, FALSE, 0);
			gtk_box_pack_start ((GtkBox *) _b, gtk_label_new (_type >= 13 ? gtk_window_get_title ((GtkWindow *) _w) : "(File)"), TRUE, FALSE, 0);
		}
		gtk_box_pack_start ((GtkBox *) _b, button (0, "Cancel", do_stop, _w), TRUE, FALSE, 0);
	}
	gtk_box_pack_start ((GtkBox *) _b, button (0, "No, nothing", do_withdraw, _w), TRUE, FALSE, 0);
	
	gtk_widget_show_all (_w);
	
	if (_type == 3) {gtk_box_pack_start ((GtkBox *) _b, gtk_label_new (((tag1 *) _parcel)->address), TRUE, FALSE, 0);}
	else if ((_type >= 7) && (_type <= 9)) free (_file1);
	else if ((_type >= 10) && (_type <= 12)) {
		stop = 0; g_thread_new ("worker1", _type == 12 ? do_erase_selected : (_type == 11 ? do_move_selected : do_copy_selected), _w);
	}
	else if ((_type >= 13) && (_type <= 15)) {
		free (_file1); stop = 0; g_thread_new ("worker1", _type == 15 ? do_erase_context : (_type == 14 ? do_move_context : do_copy_context), _w);
	}
}

static void *do_up (GtkWidget *widget, void *data) {
	char *_address = libtext_connect (2, gtk_label_get_text ((GtkLabel *) pages_label), "/..");
	do_to_session (widget, widget, 1); do_to_session (widget, (void *) _address, 2);
	free (_address);
}
static void *do_reload (GtkWidget *widget, void *data) {do_to_session (widget, widget, 1); do_to_session (widget, (void *) gtk_label_get_text ((GtkLabel *) pages_label), 2);}
static void *do_select_all (GtkWidget *widget, void *data) {do_to_session (widget, data, 3);}
static void *do_deselect_all (GtkWidget *widget, void *data) {do_to_session (widget, data, 4);}
static void *do_invert_selection (GtkWidget *widget, void *data) {do_to_session (widget, data, 5);}
static void *do_copy_selected (void *data) {do_to_session ((GtkWidget *) data, (void *) data, 7);}
static void *do_move_selected (void *data) {do_to_session ((GtkWidget *) data, (void *) data, 8);}
static void *do_erase_selected (void *data) {do_to_session ((GtkWidget *) data, (void *) data, 9);}
static void *do_copy_context (void *data) {do_to_session ((GtkWidget *) data, (void *) data, 10);}
static void *do_move_context (void *data) {do_to_session ((GtkWidget *) data, (void *) data, 11);}
static void *do_erase_context (void *data) {do_to_session ((GtkWidget *) data, (void *) data, 12);}
static void *do_withdraw (GtkWidget *widget, void *data) {do_to_session (widget, data, 0);}
static void *do_stop (GtkWidget *widget, void *data) {do_to_session (widget, data, 6);}
static void *do_terminal (GtkWidget *widget, void *data) {do_to_session (widget, widget, 13);}
static void *do_to_session (GtkWidget *_from, void *_parcel, int _order) {
	if (_parcel == NULL) {
		if ((_order >= 7) && (_order <= 9)) {gdk_threads_enter (); gtk_widget_destroy ((GtkWidget *) _parcel); free (to); to = NULL; gdk_threads_leave (); g_thread_exit (NULL);}
		return NULL;
	}
	
	/*
		1 = Remove all item(s) from current session
		2 = Add item(s) in current session
		3 = Select all item(s)
		4 = Deselect all item(s)
		5 = Invert selection
		6 = Stop processing file(s)
		7 = Copy selected file(s) :: Must run with g_thread_create()
		8 = Move selected file(s) :: Must run with g_thread_create()
		9 = Erase selected file(s) :: Must run with g_thread_create()
		10 = From context: Copy :: Must run with g_thread_create()
		11 = From context: Move :: Must run with g_thread_create()
		12 = From context: Erase :: Must run with g_thread_create()
		13 = Run terminal
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
	else if ((_order >= 3) && (_order <= 5)) {
		GList *_list = gtk_container_get_children ((GtkContainer *) session (this_page)); if (_list == NULL) return NULL;
		int _nth; for (_nth = 0; _nth < g_list_length (_list); _nth++) {
			GtkWidget2 *_item = (GtkWidget2 *) g_list_nth_data (_list, _nth);
			if (_order == 3) {if (((tag1 *) _item->data)->selected != 1) {((tag1 *) _item->data)->selected = 1; content (_item, NULL);}}
			else if (_order == 4) {if (((tag1 *) _item->data)->selected != 0) {((tag1 *) _item->data)->selected = 0; content (_item, NULL);}}
			else if (_order == 5) {if (((tag1 *) _item->data)->selected != 1) ((tag1 *) _item->data)->selected = 1; else ((tag1 *) _item->data)->selected = 0; content (_item, NULL);}
		}
		g_list_free (_list); gtk_widget_destroy ((GtkWidget *) _parcel);
	}
	else if (_order == 6) {stop = 1; gtk_widget_destroy ((GtkWidget *) _parcel); do_reload (NULL, NULL);}
	else if ((_order >= 7) && (_order <= 9)) {
		GList *_list = gtk_container_get_children ((GtkContainer *) session (this_page)); if (_list == NULL) g_thread_exit (NULL);
		int _nth; for (_nth = 0; _nth < g_list_length (_list); _nth++) {
			if (stop == 1) break;
			GtkWidget2 *_item = (GtkWidget2 *) g_list_nth_data (_list, _nth);
			if (((tag1 *) _item->data)->selected == 1) {
				char *_name = libfile_name (((tag1 *) _item->data)->address); char *_to1 = libtext_connect (3, to, "/", _name);
				gdk_threads_enter ();
				gtk_label_set_text ((GtkLabel *) chase ((GtkWidget *) _parcel, 2, 0, 1), ((tag1 *) _item->data)->address);
				if (_order != 9) gtk_label_set_text ((GtkLabel *) chase ((GtkWidget *) _parcel, 2, 0, 3), _to1);
				gdk_threads_leave ();
				
				char _result;
				if (_order == 9) libfile_erase (((tag1 *) _item->data)->address, &stop);
				else if (_order == 8) libfile_move (((tag1 *) _item->data)->address, _to1, &stop);
				else libfile_copy (((tag1 *) _item->data)->address, _to1, &stop);
				if (_result != 's') {gdk_threads_enter (); stop = 1; gdk_threads_leave (); break;}
				free (_to1); free (_name);
			}
		}
		g_list_free (_list);
		gdk_threads_enter (); gtk_widget_destroy ((GtkWidget *) _parcel); free (to); to = NULL; gdk_threads_leave ();
		g_thread_exit (NULL);
	}
	else if ((_order >= 10) && (_order <= 12)) {
		char *_from = (char *) gtk_window_get_title ((GtkWindow *) _parcel); if (_from == NULL) g_thread_exit (NULL);
		
		gdk_threads_enter ();
		gtk_label_set_text ((GtkLabel *) chase ((GtkWidget *) _parcel, 2, 0, 1), _from);
		if (_order != 12) gtk_label_set_text ((GtkLabel *) chase ((GtkWidget *) _parcel, 2, 0, 3), to);
		gdk_threads_leave ();
		
		char _result;
		if (_order == 12) libfile_erase (_from, &stop);
		else if (_order == 11) libfile_move (_from, to, &stop);
		else libfile_copy (_from, to, &stop);
		//if (_result != 's') {gdk_threads_enter (); stop = 1; gdk_threads_leave ();}
		
		gdk_threads_enter (); gtk_widget_destroy ((GtkWidget *) _parcel); free (to); to = NULL; gdk_threads_leave ();
		g_thread_exit (NULL);
	}
	else if (_order == 13) {
		char *_here = (char *) gtk_label_get_text ((GtkLabel *) pages_label);
		
		pid_t _worker = fork ();
		if (_worker == 0) {
			execl ("/bin/env", "env", "/bin/terminal-emulator", NULL);
		}
	}
	else gtk_widget_destroy ((GtkWidget *) _parcel);
}
