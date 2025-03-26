#include <gtk/gtk.h>
#include "../sol.h"
#include "../parse/parse.h"

int
main(int argc, char *argv[])
{
    char Flags;
    int c;
    if (argc == 1) pHelp();
    while ((c = getopt(argc, argv, "pi")) != -1) {
        switch (c) {
            case 'p':// only allow positive results
                Flags + 0b1;
                break;
            case 'i':// only allow integer results
                Flags + b10;
                break;
        }
    }

    initModel();

    for (int index = optind; index < argc; index++)
    {
        FILE *fd;
        if (!(fd = fopen(argv[index], "r")))
        {
            perror("Error: could not open file\n");
            perror(argv[index]);
            return (-1);
        }
        yyset_in(fd);
    }
}
