#include "../sol.h"
#include "../parse/parse.h"
#include <gio/gunixoutputstream.h>

static void
pHelp()
{
    puts("run the program better and git gud");
}

int
main(int argc, char *argv[])
{
    char Flags = 0;
    int c;
    FILE *out = NULL;
    GOutputStream *ostream;

    if (argc == 1) pHelp();
    while ((c = getopt(argc, argv, "nio:h")) != -1) {
        switch (c) {
            case 'n':// also allow negative results
                Flags += 0b1;
                break;
            case 'i':// only allow integer results
                Flags += 0b10;
                break;
            case 'o':
                out = fopen(optarg, "w+");
                break;
            case 'h':
                pHelp();
                return 0;
        }
    }

    initModel();

    ostream = g_unix_output_stream_new(fileno(out ? out : stdout), FALSE);

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
    if (out) fclose(out);
}
