#include <gtk/gtk.h>
#include "../sol.h"

int
main(int argc, char *argv[])
{
    char Flags;
    int c;
    if (argc == 1) pHelp();
    while ((c = getopt(argc, argv, "pi")) != -1) {
        switch (c) {
            case 'p':// only allow positive results
                break;
            case 'i':// only allow integer results
                break;
        }
    }
}
