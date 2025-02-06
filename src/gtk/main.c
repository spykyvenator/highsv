#include <gtk/gtk.h>
#include "highsv.h"

gboolean mip = 0, pos = 1;

int
main (int argc, char *argv[])
{
  int res;
  if (argc == 1) {
    char *argv_new[2];
    argv_new[0] = argv[0];
    GError *error = NULL;
    GFileIOStream* stream = NULL;
    GFile *new = g_file_new_tmp("new-XXXXXX.txt", &stream, &error);
    if (!new) {
        g_printerr("Error creating temp file: %s\n", error->message);
        g_clear_error(&error);
        return 1;
    }
    argv_new[1] = g_file_get_path(new);
    if (!g_io_stream_close((GIOStream*)stream, NULL, &error)) {
        g_printerr("Error closing temp file stream: %s\n", error->message);
        g_clear_error(&error);
        return 1;
    }

    res = g_application_run(G_APPLICATION(highsv_app_new ()), 2, argv_new);
    g_free(argv_new[1]);
  } else {
    res = g_application_run(G_APPLICATION(highsv_app_new ()), argc, argv);
  }
  return res;
}
