#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include "hash.h"
#include "hashmap.h"

int
main(int argc, char** argv)
{
    char* fname1;
    char* fname2;
    int opt;
    const char* short_opt = "f:s:";
    struct option long_opt[] =
    {
        {"first file",  required_argument, NULL, 'f'},
        {"second file", required_argument, NULL, 's'},
        {NULL,          0,                 NULL, 0  }
    };

    while((opt = getopt_long(argc, argv, short_opt, long_opt, NULL)) != -1)
    {
        switch(opt)
        {
            case -1:
            case 0:
                break;
            case 'f':
                fname1 = optarg;
                break;
            case 's':
                fname2 = optarg;
                break;
            case ':':
            case '?':
                return -2;
            default:
                return -2;
        }
    }

    printf("f1=%s f2=%s\n", fname1, fname2);

    return EXIT_SUCCESS;
}
