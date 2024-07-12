#pragma once

#include <gtk/gtk.h>


#define HIGHSV_APP_TYPE (highsv_app_get_type ())
G_DECLARE_FINAL_TYPE (HighsvApp, highsv_app, HIGHSV, APP, GtkApplication)


HighsvApp *highsv_app_new(void);
