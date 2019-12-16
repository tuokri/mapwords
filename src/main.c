#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include "hash.h"
#include "hashmap.h"
#include "util.h"

// Assume generous 511 (+'\0') maximum word length.
#define WORD_SIZE 511

int
main(int argc, char** argv)
{
    char* fname1 = NULL;
    int opt;
    const char* short_opt = "f:";
    struct option long_opt[] =
        {
            {"file", required_argument, NULL, 'f'},
            {NULL, 0,                   NULL, 0}
        };

    while ((opt = getopt_long(argc, argv, short_opt, long_opt, NULL)) != -1)
    {
        switch (opt)
        {
            case -1:
            case 0:
                break;
            case 'f':
                fname1 = optarg;
                break;
            case ':':
            case '?':
                return -2;
            default:
                return -2;
        }
    }

    printf("file to be read: %s\n", fname1);
    FILE* f1 = fopen(fname1, "r");
    if (!f1)
    {
        printf("error opening file\n");
        return EXIT_FAILURE;
    }

    char word[WORD_SIZE + 1];
    hashmap_map_t* map = hashmap_init(hash_crc32, hash_djb2);
    if (!map)
    {
        printf("error initializing map\n");
        return EXIT_FAILURE;
    }

    uint64_t wordcount = 0;
    while (read_next_word(f1, word, WORD_SIZE) == 1)
    {
        str_tolower(word);
        wordcount++;

        hashmap_key_t key =
            {
                strlen(word) + 1,
                word
            };

        int64_t status = hashmap_add(map, key, 5);
        if (status != HASHMAP_OK)
        {
            switch (status)
            {
                case HASHMAP_KEY_ALREADY_IN_MAP:
                    printf("hashmap_add(): key=%s already in map\n", key.key);
                    break;
                default:
                    printf("hashmap_add(): error: %ld\n", status);
                    return EXIT_FAILURE;
            }
        }

        printf("map->size=%lu\n", map->size);
    }

    fclose(f1);
    // fclose(f2);
    return EXIT_SUCCESS;
}
