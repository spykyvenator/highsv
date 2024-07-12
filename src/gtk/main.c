#include <gtk/gtk.h>

#include "highsv.h"

int
main (int argc, char *argv[])
{
  return g_application_run (G_APPLICATION (highsv_app_new ()), argc, argv);
}
