#include "../sol.h"
#include "../parse/parse.h"
#include <gio/gunixoutputstream.h>

static void
pHelp()
{
    puts("run the program better or git gud");
}

int
main(int argc, char *argv[])
{
    char Flags = 0;
    int c;
    if (argc == 1) pHelp();
    while ((c = getopt(argc, argv, "ni")) != -1) {
        switch (c) {
            case 'n':// also allow negative results
                Flags += 0b1;
                break;
            case 'i':// only allow integer results
                Flags += 0b10;
                break;
        }
    }

    initModel();

    GOutputStream *ostream = g_unix_output_stream_new(fileno(stdout), FALSE);

    for (int index = optind; index < argc; index++)
    {
        FILE *fd;
        if (!(fd = fopen(argv[index], "r")))
        {
            perror("Error: could not open file\n");
            perror(argv[index]);
            return (-1);
        }
        parseFile(fd, ostream, Flags && 0b10, !(Flags && 0b01));
    }
    g_output_stream_close(ostream, NULL, NULL);
}
