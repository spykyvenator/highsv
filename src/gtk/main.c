#include <gtk/gtk.h>
#include "highsv.h"
#include "highsvWin.h"

gboolean mip = 0, pos = 1;

int
main (int argc, char *argv[])
{
  int res;

  HighsvApp *app = highsv_app_new();
  GList *windows;
  res = g_application_run(G_APPLICATION(app), argc, argv);
  return res;
}
