#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "../src/sol.h"

int
main(int argc, char *argv[])
{
    if (argc != 2) return 1;
    FILE *f = fopen(argv[1], "r");
    uint32_t length = 0;
    char * buffer = NULL;
    if (f) {
        fseek (f, 0, SEEK_END);
        length = ftell (f);
        fseek (f, 0, SEEK_SET);
        buffer = malloc (length);
        if (!buffer || !fread (buffer, 1, length, f)) return 1;
        fclose (f);
        }
    parseString(buffer, NULL);
    return 0;
}
