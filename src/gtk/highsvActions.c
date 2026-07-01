#include <gtksourceview/gtksource.h>

#include "highsvWin.h"
#include "highsvSol.h"
#include "../util.h"
#include "highsvTab.h"
#include "highsvActions.h"

extern gboolean mip, pos;


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

void
quit_activated(GSimpleAction *action, GVariant *parameter, gpointer app)
{
  g_application_quit(G_APPLICATION(app));
}

void
open_tab(GSimpleAction *action, GVariant *parameter, gpointer app)
{
  HighsvAppWindow *win = HIGHSV_APP_WINDOW(gtk_application_get_active_window(GTK_APPLICATION(app)));
  openNew(NULL, win);
}

void
open_new_tab(GSimpleAction *action, GVariant *parameter, gpointer app)
{
  HighsvAppWindow *win = HIGHSV_APP_WINDOW(gtk_application_get_active_window(GTK_APPLICATION(app)));
  openNew(NULL, win);
}

void
close_tab(GSimpleAction *action, GVariant *parameter, gpointer app)
{
  HighsvAppWindow *win = HIGHSV_APP_WINDOW(gtk_application_get_active_window(GTK_APPLICATION(app)));
  closeActive(NULL, win);
}

void
close_tab_by_pointer(GtkButton *button, gpointer data)
{
  GtkWidget *t = (GtkWidget*) data;
  GtkNotebook *n = g_object_get_data(G_OBJECT(t), "parent_notebook");

  gtk_notebook_remove_page(n, gtk_notebook_page_num(n, t));
  g_object_unref(t);
}

void
close_errormsg(GtkButton *button, gpointer data)
{
    GtkWidget *revealer, *overlay;

    revealer = (GtkWidget*) data;
    overlay = gtk_widget_get_parent(GTK_WIDGET(revealer));

    gtk_revealer_set_reveal_child(GTK_REVEALER(revealer), FALSE);
    gtk_overlay_remove_overlay(GTK_OVERLAY(overlay), revealer);

    if (freeEM(revealer))
        die("failed to free errormsg");
}

void
save_tab_by_pointer(GtkButton *button, gpointer data)
{
  GtkWidget *temp = (GtkWidget*) data;
  gchar *path = (gchar*) g_object_get_data(G_OBJECT(temp), "path");
  GFile *f = g_file_new_for_path(path);

  gtk_widget_set_visible(GTK_WIDGET(button), false);
  if (!f) return;// silently ignore if no filename is present
  saveFile(f, getContentFromTab(temp));
  g_object_unref(f);
}

void
showSaveBtn(GtkTextBuffer* buffer, GtkTextBufferNotifyFlags flags, guint position, guint length, gpointer data) 
{
    GtkWidget *t = data;
    gtk_widget_set_visible(t, true);
}


void
save_tab(GSimpleAction *action, GVariant *parameter, gpointer app)
{
  HighsvAppWindow *win = HIGHSV_APP_WINDOW(gtk_application_get_active_window(GTK_APPLICATION(app)));
  saveActive(NULL, win);
}

/* In case We'd like a keybind for saving as
void
save_as_tab(GSimpleAction *action, GVariant *parameter, gpointer app)
{
  HighsvAppWindow *win = HIGHSV_APP_WINDOW(gtk_application_get_active_window(GTK_APPLICATION(app)));
  saveAsActive(NULL, win);
}
*/

void
solve_tab(GSimpleAction *action, GVariant *parameter, gpointer app)
{
  HighsvAppWindow *win = HIGHSV_APP_WINDOW(gtk_application_get_active_window(GTK_APPLICATION(app)));
  solveEntry(NULL, win);
}

/*
 * could I set the sol button action here instead?
 */
void
toggleMip(GtkCheckButton *self, gpointer app)
{
	mip = gtk_check_button_get_active(self);
}

void
togglePos(GtkCheckButton *self, gpointer app)
{
	pos = gtk_check_button_get_active(self);
}

void
search(GSimpleAction *a, GVariant *parameter, gpointer app)
{
    HighsvAppWindow *win = HIGHSV_APP_WINDOW(gtk_application_get_active_window(GTK_APPLICATION(app)));
    GtkWidget *overlay = getNotebookActive(GTK_NOTEBOOK(win->notebook));
    openTabSearchDialog(overlay);
}

void
search_entry_up(GtkButton *BtnUp, GObject *entry)
{
    int sel = (int) g_object_get_data(G_OBJECT(entry), "selection");// should return 0 if not found
    g_object_set_data(G_OBJECT(entry), "selection", (void*) ++sel);// should return 0 if not found

    g_signal_emit_by_name(entry, "search-changed");
}

void
search_entry_dwn(GtkButton *BtnUp, GObject *entry)
{
    int sel = (int) g_object_get_data(G_OBJECT(entry), "selection");// should return 0 if not found
    sel = MAX(sel-1, 0);
    g_object_set_data(G_OBJECT(entry), "selection", (void*) sel);// should return 0 if not found

    g_signal_emit_by_name(entry, "search-changed");
}

void
search_disabled(GtkWidget *sb, GtkTextBuffer *buffer)
{
    GtkTextIter start_s, end_s;
    gtk_text_buffer_get_start_iter(buffer, &start_s);
    gtk_text_buffer_get_end_iter(buffer, &end_s);
    gtk_text_buffer_remove_tag_by_name(buffer, "search_res", &start_s, &end_s);
    gtk_text_buffer_remove_tag_by_name(buffer, "search_sel", &start_s, &end_s);
}

void
search_changed_cb(GtkSearchEntry *entry, GtkTextBuffer *buffer)
{
    const char *text;
    GtkTextIter start_s, end_s, start_match, end_match;
    GtkTextTagTable *table;
    GtkTextTag* tag;
    int nbResults = 0, sel;

    sel = (int) g_object_get_data(G_OBJECT(entry), "selection");// should return 0 if not found

    text = gtk_editable_get_text(GTK_EDITABLE(entry));
    gtk_text_buffer_get_start_iter(buffer, &start_s);
    gtk_text_buffer_get_end_iter(buffer, &end_s);

    if (!strcmp(text, "")) {// early stop if search removed => hide searchbar
        GtkWidget *searchBar = getEntrySearchBar(GTK_WIDGET(entry));

        gtk_text_buffer_remove_tag_by_name(buffer, "search_res", &start_s, &end_s);
        gtk_text_buffer_remove_tag_by_name(buffer, "search_sel", &start_s, &end_s);
        gtk_search_bar_set_search_mode(GTK_SEARCH_BAR(searchBar), 
                FALSE);

        return;
    }

    table = gtk_text_buffer_get_tag_table(buffer);
    tag = gtk_text_tag_table_lookup(table, "search_res");

    
    if (!tag)// only add tag once
    {
        gtk_text_buffer_create_tag(buffer, "search_res", 
          "background", "#38383838", NULL);
        gtk_text_buffer_create_tag(buffer, "search_sel", 
          "background", "#38383880", NULL);
    } else {
        gtk_text_buffer_remove_tag_by_name(buffer, "search_sel", &start_s, &end_s);
        gtk_text_buffer_remove_tag_by_name(buffer, "search_res", &start_s, &end_s);
    }

    while (gtk_text_iter_forward_search(&start_s, text, 
        GTK_TEXT_SEARCH_TEXT_ONLY | 
        GTK_TEXT_SEARCH_VISIBLE_ONLY, 
        &start_match, &end_match, NULL)) {
        if (nbResults == sel) 
        {
            gtk_text_buffer_apply_tag_by_name(buffer, 
                "search_sel", &start_match, &end_match);
            GtkWidget *view = GTK_WIDGET(g_object_get_data(
                        G_OBJECT(entry), "view"));
            gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW(view),
              &start_match,
              0.4,
              TRUE,
              0.0,
              0.0
            );
        }
        else
        {
            gtk_text_buffer_apply_tag_by_name(buffer, 
                "search_res", &start_match, &end_match);
        }
        gint offset = gtk_text_iter_get_offset(&end_match);
        gtk_text_buffer_get_iter_at_offset(buffer, 
            &start_s, offset);
        nbResults++;
    }
    GtkWidget *label = GTK_WIDGET(g_object_get_data(G_OBJECT(entry),
                            "label"));
    if (sel == nbResults)// TODO: clean this up
    {
        sel = nbResults-1;
        nbResults = 0;
        g_object_set_data(G_OBJECT(entry), "selection", (void*) sel);// should return 0 if not found
        gtk_text_buffer_get_start_iter(buffer, &start_s);
        while (gtk_text_iter_forward_search(&start_s, text, 
            GTK_TEXT_SEARCH_TEXT_ONLY | 
            GTK_TEXT_SEARCH_VISIBLE_ONLY, 
            &start_match, &end_match, NULL)) {
            gtk_text_buffer_apply_tag_by_name(buffer, 
                nbResults == sel ? "search_sel" : "search_res", 
                &start_match, &end_match);
            gint offset = gtk_text_iter_get_offset(&end_match);
            gtk_text_buffer_get_iter_at_offset(buffer, 
                &start_s, offset);
            nbResults++;
        }
    }
    char lbls[512];
    snprintf(lbls, 512, "%d/%d", sel+1, nbResults);
    gtk_label_set_label(GTK_LABEL(label), lbls);
}

void
zoomIn(GSimpleAction *a, GVariant *parameter, gpointer app)
{
    HighsvAppWindow *win = HIGHSV_APP_WINDOW(gtk_application_get_active_window(GTK_APPLICATION(app)));
    GtkWidget *tab = getNotebookActive(GTK_NOTEBOOK(win->notebook));
    GtkWidget *view = gtk_scrolled_window_get_child(GTK_SCROLLED_WINDOW(tab));// owned by instance
    puts("ok");
    puts(gtk_widget_get_css_name(GTK_WIDGET(view)));
}

void
complete(GSimpleAction *action, GVariant *parameter, gpointer app)
{
    HighsvAppWindow *win = HIGHSV_APP_WINDOW(gtk_application_get_active_window(GTK_APPLICATION(app)));
    GtkWidget *tab = getNotebookActive(GTK_NOTEBOOK(win->notebook));
    GtkWidget *view = gtk_scrolled_window_get_child(GTK_SCROLLED_WINDOW(tab));// owned by instance
    GtkSourceCompletion *c = gtk_source_view_get_completion(GTK_SOURCE_VIEW(view));
    gtk_source_completion_show(c);
}

gboolean
on_key_pressed(GtkEventControllerKey *controller,
               guint                  keyval,
               guint                  keycode,
               GdkModifierType        state,
               gpointer               user_data)
{
    HighsvAppWindow *win = user_data;
    if (state & GDK_ALT_MASK) {
        gunichar c = gdk_keyval_to_unicode(keyval);
        if (c >= '1' && c <= '9')
            gtk_notebook_set_current_page(win->notebook, c - '1');
    }

    return FALSE;
}
#pragma GCC diagnostic pop
