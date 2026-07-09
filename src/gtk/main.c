#include <gtk/gtk.h>
#include "highsv.h"
#include "highsvWin.h"

int
main (int argc, char *argv[])
{
  int res;

  HighsvApp *app = highsv_app_new();
  res = g_application_run(G_APPLICATION(app), argc, argv);
  return res;
}
