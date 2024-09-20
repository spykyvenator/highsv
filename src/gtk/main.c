#include <gtk/gtk.h>

#include "highsv.h"

int
main (int argc, char *argv[])
{
  int res;
  if (argc == 1) {
    char *argv_new[2];
    argv_new[0] = argv[0];
    argv_new[1] = "/tmp/test.ltx";
    res = g_application_run(G_APPLICATION(highsv_app_new ()), 2, argv_new);
  } else {
    res = g_application_run(G_APPLICATION(highsv_app_new ()), argc, argv);
  }
  return res;
}
