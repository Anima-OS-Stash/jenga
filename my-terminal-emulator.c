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

// gcc -O3 -std=gnu99 $(pkg-config --cflags --libs vte) my-terminal-emulator.c

#include <vte/vte.h>
#include <gdk/gdkkeysyms.h>
#include <malloc.h>

static GtkWidget *overall, *overall1, *pages, *pages_list, *pages_label, *pages_new, *pages_remove, **page;

static int screen_width, screen_height; static char **command; static int n_pages, this_page, wanted_page; static GdkGeometry sizing;

int main (int argc, char *argv[]);
static void *ask_quit (GtkWidget *widget, void *data);
static void *ask_quit_keybinding (void *data);
static void *withdraw_widget (GtkWidget *widget, void *data);
static void *quit_program (GtkWidget *widget, void *data);
static void *new_page (GtkWidget *widget, void *data);
static void *new_page_keybinding (void *data);
static void *remove_page (GtkWidget *widget, void *data);
static void *remove_page_keybinding (void *data);
static void *remove_this_page (GtkWidget *widget, void *data);
static void *show_pages (GtkWidget *widget, void *data);
static void *show_page (GtkWidget *widget, void *data);
static void *show_label (GtkWidget *widget, void *data);
static void *clear_scrollback (void *data);
static void *page_minus1 (void *data);
static void *page_plus1 (void *data);
static void *copy_text (void *data);
static void *paste_text (void *data);

int main (int argc, char *argv[]) {
	// Properties.
	printf ("[my-terminal-emulator] Hi.\n"); gtk_init (&argc, &argv);
	page = NULL;
	GdkScreen *screen = gdk_screen_get_default (); screen_width = gdk_screen_get_width (screen); screen_height = gdk_screen_get_height (screen);
	command = (char **) malloc (sizeof (char *) * 2); if (command == NULL) {printf ("[my-terminal-emulator] More memory, please! T_T\n"); return 1;}
	command[0] = "/bin/bash"; command[1] = NULL;
	n_pages = 0; this_page = -1; wanted_page = -1;
	
	// Main casts.
	overall = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title ((GtkWindow *) overall, "my-terminal-emulator");
	g_signal_connect (overall, "delete_event", (GCallback) ask_quit, NULL);
	overall1 = gtk_vbox_new (FALSE, 0); gtk_container_add ((GtkContainer *) overall, overall1);
	pages = gtk_hbox_new (FALSE, 0); gtk_box_pack_start ((GtkBox *) overall1, pages, FALSE, FALSE, 0);
	pages_list = gtk_button_new (); gtk_button_set_image ((GtkButton *) pages_list, gtk_image_new_from_icon_name ("view-list-symbolic", GTK_ICON_SIZE_MENU));
	g_signal_connect (pages_list, "clicked", (GCallback) show_pages, NULL);
	gtk_box_pack_start ((GtkBox *) pages, pages_list, FALSE, FALSE, 0);
	pages_label = gtk_label_new ("(Unlabeled)"); gtk_box_pack_start ((GtkBox *) pages, pages_label, TRUE, FALSE, 0);
	pages_new = gtk_button_new (); gtk_button_set_image ((GtkButton *) pages_new, gtk_image_new_from_icon_name ("gtk-add", GTK_ICON_SIZE_MENU));
	g_signal_connect (pages_new, "clicked", (GCallback) new_page, NULL);
	gtk_box_pack_start ((GtkBox *) pages, pages_new, FALSE, FALSE, 0);
	pages_remove = gtk_button_new (); gtk_button_set_image ((GtkButton *) pages_remove, gtk_image_new_from_icon_name ("gtk-remove", GTK_ICON_SIZE_MENU));
	g_signal_connect (pages_remove, "clicked", (GCallback) remove_this_page, NULL);
	gtk_box_pack_start ((GtkBox *) pages, pages_remove, FALSE, FALSE, 0);
	new_page (NULL, NULL);
	
	GtkAccelGroup *keybindings = gtk_accel_group_new ();
	gtk_accel_group_connect (keybindings, GDK_KEY_N, GDK_CONTROL_MASK | GDK_SHIFT_MASK, GTK_ACCEL_VISIBLE, g_cclosure_new ((GCallback) new_page_keybinding, NULL, NULL));
	gtk_accel_group_connect (keybindings, GDK_KEY_R, GDK_CONTROL_MASK | GDK_SHIFT_MASK, GTK_ACCEL_VISIBLE, g_cclosure_new ((GCallback) clear_scrollback, NULL, NULL));
	gtk_accel_group_connect (keybindings, GDK_KEY_Left, GDK_CONTROL_MASK | GDK_SHIFT_MASK, GTK_ACCEL_VISIBLE, g_cclosure_new ((GCallback) page_minus1, NULL, NULL));
	gtk_accel_group_connect (keybindings, GDK_KEY_Right, GDK_CONTROL_MASK | GDK_SHIFT_MASK, GTK_ACCEL_VISIBLE, g_cclosure_new ((GCallback) page_plus1, NULL, NULL));
	gtk_accel_group_connect (keybindings, GDK_KEY_W, GDK_CONTROL_MASK | GDK_SHIFT_MASK, GTK_ACCEL_VISIBLE, g_cclosure_new ((GCallback) remove_page_keybinding, NULL, NULL));
	gtk_accel_group_connect (keybindings, GDK_KEY_Q, GDK_CONTROL_MASK | GDK_SHIFT_MASK, GTK_ACCEL_VISIBLE, g_cclosure_new ((GCallback) ask_quit_keybinding, NULL, NULL));
	gtk_accel_group_connect (keybindings, GDK_KEY_C, GDK_CONTROL_MASK | GDK_SHIFT_MASK, GTK_ACCEL_VISIBLE, g_cclosure_new ((GCallback) copy_text, NULL, NULL));
	gtk_accel_group_connect (keybindings, GDK_KEY_V, GDK_CONTROL_MASK | GDK_SHIFT_MASK, GTK_ACCEL_VISIBLE, g_cclosure_new ((GCallback) paste_text, NULL, NULL));
	gtk_window_add_accel_group ((GtkWindow *) overall, keybindings);
	
	// Action!
	gtk_widget_show_all (overall);
	gtk_main ();
	
	// Dispose.
	printf ("[my-terminal-emulator] Bye.\n"); return 0;
}

static void *ask_quit (GtkWidget *widget, void *data) {
	GtkWidget *_overall = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	g_signal_connect (_overall, "delete_event", (GCallback) withdraw_widget, (void *) overall);
	gtk_window_set_modal ((GtkWindow *) _overall, TRUE);
	
	GtkWidget *_overall1 = gtk_hbox_new (FALSE, 0); gtk_container_add ((GtkContainer *) _overall, _overall1);
	
	GtkWidget *_question = gtk_label_new ("Quit?"); gtk_box_pack_start ((GtkBox *) _overall1, _question, FALSE, FALSE, 0);
	
	GtkWidget *_answer_y = gtk_button_new ();
	gtk_button_set_image ((GtkButton *) _answer_y, (GtkWidget *) gtk_image_new_from_icon_name ("emblem-ok-symbolic", GTK_ICON_SIZE_MENU));
	g_signal_connect (_answer_y, "clicked", (GCallback) quit_program, NULL);
	gtk_box_pack_start ((GtkBox *) _overall1, _answer_y, FALSE, FALSE, 0);
	
	GtkWidget *_answer_n = gtk_button_new ();
	gtk_button_set_image ((GtkButton *) _answer_n, (GtkWidget *) gtk_image_new_from_icon_name ("gtk-cancel", GTK_ICON_SIZE_MENU));
	g_signal_connect (_answer_n, "clicked", (GCallback) withdraw_widget, (void *) _overall);
	gtk_box_pack_start ((GtkBox *) _overall1, _answer_n, FALSE, FALSE, 0);
	
	gtk_widget_show_all (_overall);
}
static void *ask_quit_keybinding (void *data) {ask_quit (NULL, NULL);}

static void *withdraw_widget (GtkWidget *widget, void *data) {gtk_widget_destroy ((GtkWidget *) data);}

static void *quit_program (GtkWidget *widget, void *data) {
	free (page); free (command);
	gtk_main_quit ();
}

static void *new_page (GtkWidget *widget, void *data) {
	GtkWidget **_page = (GtkWidget **) malloc (sizeof (GtkWidget *) * (n_pages + 1)); if (_page == NULL) {printf ("[my-terminal-emulator] More memory, please! T_T\n"); quit_program (NULL, NULL);}
	int nth = 0; while (nth < n_pages) {_page[nth] = page[nth]; nth++;}
	_page[nth] = gtk_hbox_new (FALSE, 0); printf ("[my-terminal-emulator] New page: %d\n", _page[nth]);
	
	GtkWidget *_terminal = vte_terminal_new ();
	vte_terminal_set_scroll_on_keystroke ((VteTerminal *) _terminal, TRUE);
	vte_terminal_set_scrollback_lines ((VteTerminal *) _terminal, -1);
	g_signal_connect (_terminal, "child_exited", (GCallback) remove_page, (void *) _page[nth]);
	g_signal_connect (_terminal, "window_title_changed", (GCallback) show_label, (void *) _terminal);
	vte_terminal_fork_command_full ((VteTerminal *) _terminal, 0, "/", command, NULL, 0, NULL, NULL, NULL, NULL);
	if (n_pages <= 0) {
		sizing.min_width, sizing.base_width, sizing.width_inc = ((VteTerminal *) _terminal)->char_width; sizing.min_height, sizing.base_height, sizing.height_inc = ((VteTerminal *) _terminal)->char_height;
		gtk_window_set_geometry_hints ((GtkWindow *) overall, _terminal, &sizing, GDK_HINT_MIN_SIZE | GDK_HINT_BASE_SIZE | GDK_HINT_RESIZE_INC);
	}
	gtk_box_pack_start ((GtkBox *) _page[nth], _terminal, TRUE, TRUE, 0);
	
	GtkWidget *_scrollbar = gtk_vscrollbar_new (vte_terminal_get_adjustment ((VteTerminal *) _terminal)); gtk_box_pack_start ((GtkBox *) _page[nth], _scrollbar, FALSE, FALSE, 0);
	
	printf ("[my-terminal-emulator] %d out, %d in\n", this_page >= 0 ? page[this_page] : 0, _page[nth]);
	if (n_pages >= 1) {g_object_ref (page[this_page]); gtk_container_remove ((GtkContainer *) overall1, page[this_page]);}
	gtk_box_pack_start ((GtkBox *) overall1, _page[nth], TRUE, TRUE, 0); gtk_widget_show_all (_page[nth]);
	if (page != NULL) free (page); page = _page;
	n_pages++; this_page = n_pages - 1;
	gtk_widget_grab_focus (_terminal);
}
static void *new_page_keybinding (void *data) {new_page (NULL, NULL);}

static void *remove_page (GtkWidget *widget, void *data) {
	if (data == NULL) return NULL; printf ("[my-terminal-emulator] Remove page: %d\n", data);
	gtk_widget_destroy ((GtkWidget *) data); page[this_page] = NULL; if (n_pages <= 1) {quit_program (NULL, NULL); return NULL;}
	
	GtkWidget **_page = (GtkWidget **) malloc (sizeof (GtkWidget *) * (n_pages - 1)); if (_page == NULL) {printf ("[my-terminal-emulator] More memory, please! T_T\n"); quit_program (NULL, NULL);}
	int nth = 0, nth1 = 0; while (nth < n_pages) {
		if (page[nth] != NULL) {printf ("[my-terminal-emulator] page[%d] = %d\n", nth, page[nth]); _page[nth1] = page[nth]; nth1++;}
		nth++;
	}
	free (page); page = _page;
	n_pages--; this_page = this_page <= 0 ? 0 : this_page - 1;
	printf ("[my-terminal-emulator] %d out, %d in\n", data, page[this_page]);
	gtk_box_pack_start ((GtkBox *) overall1, page[this_page], TRUE, TRUE, 0); gtk_widget_show_all (page[this_page]); g_object_unref (page[this_page]);
}
static void *remove_page_keybinding (void *data) {remove_page (NULL, page[this_page]);}

static void *remove_this_page (GtkWidget *widget, void *data) {remove_page (NULL, page[this_page]);}

static void *show_pages (GtkWidget *widget, void *data) {
	// Main casts.
	GtkWidget *_overall = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_resize ((GtkWindow *) _overall, screen_width / 2, screen_height / 2);
	gtk_window_set_type_hint ((GtkWindow *) _overall, GDK_WINDOW_TYPE_HINT_DROPDOWN_MENU);
	gtk_window_set_decorated ((GtkWindow *) _overall, FALSE);
	GtkWidget *_overall1 = gtk_scrolled_window_new (NULL, NULL); gtk_container_add ((GtkContainer *) _overall, _overall1);
	GtkWidget *_overall2 = gtk_vbox_new (FALSE, 0); gtk_scrolled_window_add_with_viewport ((GtkScrolledWindow *) _overall1, _overall2);
	gtk_widget_show_all (_overall); gtk_widget_grab_focus (_overall);
	
	int nth; for (nth = 0; nth < n_pages; nth++) {
		GList *_list = gtk_container_get_children ((GtkContainer *) page[nth]); if (_list == NULL) {printf ("[my-terminal-emulator] More memory, please! T_T\n"); quit_program (NULL, NULL);}
		GtkWidget *_page = gtk_button_new (); gtk_button_set_label ((GtkButton *) _page, vte_terminal_get_window_title ((VteTerminal *) g_list_nth_data (_list, 0)));
		g_signal_connect (_page, "clicked", (GCallback) show_page, (void *) page[nth]);
		gtk_box_pack_start ((GtkBox *) _overall2, _page, TRUE, FALSE, 0);
		g_list_free (_list);
	}
	gtk_widget_show_all (_overall);
	
	// Resize and relocate.
	GdkWindow *_soul = gtk_button_get_event_window ((GtkButton *) pages_list);
	int _x, _y; gdk_window_get_deskrelative_origin (_soul, &_x, &_y);
	int _width = gdk_window_get_width (_soul), _height = gdk_window_get_height (_soul);
	gtk_window_move ((GtkWindow *) _overall, _x, _y + _height);
}

static void *show_page (GtkWidget *widget, void *data) {
	// Sanity check.
	if (data == NULL) return NULL;
	int nth; for (nth = 0; nth < n_pages; nth++) {if (data == (void *) page[nth]) break;} if (nth >= n_pages) return NULL;
	
	g_object_ref (page[this_page]); gtk_container_remove ((GtkContainer *) overall1, page[this_page]);
	gtk_box_pack_start ((GtkBox *) overall1, page[nth], TRUE, TRUE, 0); gtk_widget_show_all (page[nth]); g_object_unref (page[nth]);
	
	GList *_list = gtk_container_get_children ((GtkContainer *) page[nth]); if (_list == NULL) {printf ("[my-terminal-emulator] More memory, please! T_T\n"); quit_program (NULL, NULL);}
	gtk_widget_destroy (gtk_widget_get_toplevel (widget));
	gtk_widget_grab_focus ((GtkWidget *) g_list_nth_data (_list, 0));
	g_list_free (_list);
	this_page = nth;
}

static void *show_label (GtkWidget *widget, void *data) {
	if (data == NULL) return NULL;
	if (page[this_page] == gtk_widget_get_parent ((GtkWidget *) data)) gtk_label_set_text ((GtkLabel *) pages_label, vte_terminal_get_window_title ((VteTerminal *) data));
}

static void *clear_scrollback (void *data) {
	GList *_list = gtk_container_get_children ((GtkContainer *) page[this_page]); if (_list == NULL) {printf ("[my-terminal-emulator] More memory, please! T_T\n"); quit_program (NULL, NULL);}
	vte_terminal_reset ((VteTerminal *) g_list_nth_data (_list, 0), TRUE, TRUE);
	g_list_free (_list);
}

static void *page_minus1 (void *data) {
	g_object_ref (page[this_page]); gtk_container_remove ((GtkContainer *) overall1, page[this_page]);
	this_page = this_page <= 0 ? n_pages - 1 : this_page - 1;
	gtk_box_pack_start ((GtkBox *) overall1, page[this_page], TRUE, TRUE, 0); gtk_widget_show_all (page[this_page]); g_object_unref (page[this_page]);
}

static void *page_plus1 (void *data) {
	g_object_ref (page[this_page]); gtk_container_remove ((GtkContainer *) overall1, page[this_page]);
	this_page = this_page >= n_pages - 1 ? 0 : this_page + 1;
	gtk_box_pack_start ((GtkBox *) overall1, page[this_page], TRUE, TRUE, 0); gtk_widget_show_all (page[this_page]); g_object_unref (page[this_page]);
}

static void *copy_text (void *data) {
	GList *_list = gtk_container_get_children ((GtkContainer *) page[this_page]); if (_list == NULL) {printf ("[my-terminal-emulator] More memory, please! T_T\n"); quit_program (NULL, NULL);}
	vte_terminal_copy_clipboard ((VteTerminal *) g_list_nth_data (_list, 0));
	g_list_free (_list);
}

static void *paste_text (void *data) {
	GList *_list = gtk_container_get_children ((GtkContainer *) page[this_page]); if (_list == NULL) {printf ("[my-terminal-emulator] More memory, please! T_T\n"); quit_program (NULL, NULL);}
	vte_terminal_paste_clipboard ((VteTerminal *) g_list_nth_data (_list, 0));
	g_list_free (_list);
}
