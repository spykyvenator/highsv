#include "../sol.h"
#include <gio/gunixoutputstream.h>
#include "../parse/bison/errhandle.h"

static void
pHelp()
{
    puts("opts: h: help, o: outfile");
}

int
main(int argc, char *argv[])
{
    int c;
    FILE *out = NULL;
    GOutputStream *ostream;

    if (argc == 1) pHelp();
    while ((c = getopt(argc, argv, "nio:h")) != -1) {
        switch (c) {
            case 'o':
                out = fopen(optarg, "w+");
                break;
            case 'h':
                pHelp();
                return 0;
        }
    }

    //initModel();

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
        errHandle *eh = new_errHandle();
        parseFile(fd, ostream, eh);
        eh->free(eh);
    }
    g_output_stream_close(ostream, NULL, NULL);
    if (out) fclose(out);
}
