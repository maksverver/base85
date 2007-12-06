#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <libgen.h>
#include "base85.h"

static int opt_decode = 0, opt_raw = 0, opt_c = 0, opt_width = 75;

void parse_opts(int argc, char *argv[])
{
    for (;;)
    {
        switch (getopt(argc, argv, "drcw:"))
        {
        case 'h':
            goto usage;

        case 'd':
            opt_decode = 1;
            break;
        case 'r':
            opt_raw = 1;
            break;
        case 'c':
            opt_c = 1;
            break;
        case 'w':
            opt_width = atoi(optarg);
            if (opt_width <= 0)
            {
                fprintf(stderr, "Width must be greater than zero.\n\n");
                goto usage;
            }
        case '-':
        case -1:
            goto end_of_options;
        default:
            fprintf(stderr, "Unrecognized option: %c.\n\n", optopt);
            goto usage;
        }
    }

usage:
    fprintf(stderr, "Usage:\n"
    "  %1$s -h                Show this information\n"
    "  %1$s [-r] [-c] [-wX]   Encode base85 encoded data\n"
    "  %1$s -d                Decode base85 encoded data\n"
    "Options:\n"
    "  -r    Raw mode: output base85 character data only (no formatting)\n"
    "  -c    Output as a C-string (lines surrounded by double quotes)\n"
    "  -wX   Maximum number of base85 characters per line, excluding formatting\n"
    "        Ignored in raw mode. Default is 75.\n",
        argv[0]);
    exit(1);

end_of_options:

    if (opt_decode && (opt_c || opt_raw))
    {
        fprintf(stderr, "Option -d cannot be combined with -c or -r.\n");
        goto usage;
    }
    if (opt_raw && opt_c)
    {
        fprintf(stderr, "Option -r cannot be combined with -c.\n");
        goto usage;
    }
    if (optind < argc)
    {
        fprintf(stderr, "Unexpected extra argument: %s.\n\n", argv[optind]);
        goto usage;
    }
}

int main(int argc, char *argv[])
{
    char buffer_in[1000], buffer_out[1252];
    int len_in, len_out;

    parse_opts(argc, argv);

    if (opt_decode)
    {
        /* Decoding */
        int c = EOF;
        do {
            len_in = 0;
            while (len_in < sizeof(buffer_in))
            {
                if ((c = fgetc(stdin)) == EOF)
                    break;
                if (base85_char(c))
                    buffer_in[len_in++] = c;
            }
            len_out = base85_decode(buffer_in, len_in, buffer_out);
            fwrite(buffer_out, 1, len_out, stdout);
        } while (c != EOF);
    }
    else
    {
        /* Encoding */
        int n, col = 0;
        while ((len_in = fread(buffer_in, 1, sizeof(buffer_in), stdin)) > 0)
        {
            len_out = base85_encode(buffer_in, len_in, buffer_out);
            for (n = 0; n < len_out; ++n)
            {
                if (col == opt_width)
                {
                    if (opt_c)
                        fputc('"', stdout);
                    fputc('\n', stdout);
                    col = 0;
                }
                if (col == 0 && opt_c)
                    fputc('"', stdout);
                fputc(buffer_out[n], stdout);
                ++col;
            }
        }
        if (!opt_raw && col > 0)
        {
            if (opt_c)
                fputc('"', stdout);
            fputc('\n', stdout);
        }
    }
    return 0;
}
