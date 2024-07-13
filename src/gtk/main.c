#include <gtk/gtk.h>

#include "highsv.h"

int
main (int argc, char *argv[])
{
  /*
   * this is stupid
   * is there even a second place in this array?
   */
  if (argc == 1) {
    argc = 2;
    argv[1] = malloc(sizeof(char)*14);
    argv[1] = "/tmp/test.ltx";
  }
  return g_application_run(G_APPLICATION (highsv_app_new ()), argc, argv);
}
