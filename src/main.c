#include <gtk/gtk.h>

int
main(int argc, char **argv)
{
  g_setenv ("GSETTINGS_SCHEMA_DIR", ".", FALSE);// remove this?

  return g_application_run (G_APPLICATION (example_app_new ()), argc, argv);
}
