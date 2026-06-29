#include <gtk/gtk.h>
#include <gtksourceview/gtksource.h>

#include "highsv.h"
#include "highsvWin.h"
#include "highsvFile.h"
#include "highsvSol.h"
#include "highsvActions.h"
#include "../util.h"
//#include "../parse/parse.h"

G_DEFINE_TYPE(HighsvAppWindow, highsv_app_window, GTK_TYPE_APPLICATION_WINDOW)

static void
highsv_app_window_init (HighsvAppWindow *win)
{
  gtk_widget_init_template (GTK_WIDGET (win));
}

static void
highsv_app_window_class_init(HighsvAppWindowClass *class)
{
  gtk_widget_class_set_template_from_resource (GTK_WIDGET_CLASS (class), "/org/highsvapp/window.ui");

  gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), HighsvAppWindow, notebook);
  gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class), solveEntry);
  gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class), openNew);
  gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class), openNewEmpty);
  gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class), closeActive);
  gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class), saveActive);
  gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class), saveAsActive);
  gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class), toggleMip);
  gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class), togglePos);
}

HighsvAppWindow *
highsv_app_window_new(HighsvApp *app)
{
  HighsvAppWindow *win = g_object_new(HIGHSV_APP_WINDOW_TYPE, "application", app, NULL);
  return win;
}

static GtkWidget*
makeErrorMsg(const char *msg)
{
    GtkWidget *revealer, *box, *label, *button;

    revealer = gtk_revealer_new();
    box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    label = gtk_label_new(msg);
    button = gtk_button_new_with_label("✕");

    gtk_button_set_can_shrink(GTK_BUTTON(button), TRUE);
    gtk_button_set_has_frame(GTK_BUTTON(button), TRUE);
    g_signal_connect(G_OBJECT(button), "clicked", 
        G_CALLBACK(close_errormsg), revealer);
    gtk_widget_set_tooltip_text(button, "close tooltip");

    gtk_revealer_set_transition_type(GTK_REVEALER(revealer), GTK_REVEALER_TRANSITION_TYPE_SLIDE_DOWN);
    gtk_revealer_set_transition_duration(GTK_REVEALER(revealer), 500);

    gtk_box_append(GTK_BOX(box), label);
    gtk_box_append(GTK_BOX(box), button);
    gtk_box_set_spacing(GTK_BOX(box), 10);

    gtk_widget_add_css_class(box, "error-box");
    gtk_widget_add_css_class(button, "error-button");

    gtk_widget_set_halign(revealer, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(revealer, GTK_ALIGN_START);
    gtk_revealer_set_child(GTK_REVEALER(revealer), box);

    return revealer;
}

int
freeEM(GtkWidget *msg)
{
    g_object_unref(msg);
    return 0;
}

void
highsvShowError(const char *msg, GtkWidget *view, GtkTextBuffer *bfr, int x, int y, int x2, int y2)
{
    GtkWidget *msgb, *overlay;
    GtkTextIter start, end;
    GtkTextTag *t;

    msgb = makeErrorMsg(msg);

    overlay = gtk_widget_get_parent(gtk_widget_get_parent(view));
    t = gtk_text_buffer_create_tag(bfr, "error", 
            "background", "red",
            "editable", TRUE,
            NULL);


    gtk_text_buffer_get_iter_at_line_index(bfr, &start, x, 0);
    gtk_text_buffer_get_iter_at_line(bfr, &end, x);
    gtk_text_iter_backward_line(&end);
    gtk_text_buffer_apply_tag(bfr, t, &start, &end);

    gtk_overlay_add_overlay(GTK_OVERLAY(overlay), msgb);
    gtk_overlay_set_clip_overlay(GTK_OVERLAY(overlay), msgb, TRUE);
    gtk_revealer_set_reveal_child(GTK_REVEALER(msgb), TRUE);
}
