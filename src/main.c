#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <time.h>
#include <assert.h>

#include "hash.h"
#include "hashmap.h"
#include "util.h"

// Assume generous 511 (+ '\0') maximum word length.
#define WORD_SIZE 512

int
main(int argc, char** argv)
{
    struct timespec ts_start;
    timespec_get(&ts_start, TIME_UTC);

    hash_t (* hashf)(const char*) = NULL;
    char hashf_name[HASHF_NAME_MAX_LENGTH] = {'\0'};

    char* fname1 = NULL;
    int opt;
    const char* short_opt = "f:h:";
    struct option long_opt[] =
        {
            {"file",  required_argument, NULL, 'f'},
            {"hashf", required_argument, NULL, 'h'},
            {NULL, 0,                    NULL, 0}
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
            case 'h':
                strcpy(hashf_name, optarg);
                break;
            case ':':
            case '?':
                return -2;
            default:
                return -2;
        }
    }

    printf("main(): file to be read: %s\n", fname1);
    FILE* f1 = fopen(fname1, "r");
    if (!f1)
    {
        printf("main(): error opening file\n");
        return EXIT_FAILURE;
    }

    char word_buffer[WORD_SIZE] = {'\0'};
    uint64_t wordcount = 0;
    uint64_t charcount = 0;

    hashf = get_hashf(hashf_name);
    if (hashf == NULL)
    {
        strcpy(hashf_name, "hash_crc32");
        hashf = hash_djb2;
    }

    hashmap_map_t* map = hashmap_init(hashf);
    if (!map)
    {
        printf("main(): error initializing map in\n");
        return EXIT_FAILURE;
    }

    int64_t* value = calloc(1, sizeof(int64_t));
    if (!value)
    {
        printf("main(): error allocating memory for 'value'\n");
        goto err;
    }

    int64_t status;
    while (read_next_word(f1, word_buffer, WORD_SIZE) == 1)
    {
        hashmap_print(map);
        puts("************************");

        str_tolower(word_buffer);
        // printf("%s\n", word_buffer);
        wordcount++;
        charcount += strlen(word_buffer);

        status = hashmap_get(map, word_buffer, value);
        if (status == HASHMAP_KEY_FOUND)
        {
            (*value)++;
            status = hashmap_update(map, word_buffer, *value);
            if (status != HASHMAP_OK)
            {
                printf("main(): hashmap_update(): error: %ld, word: %s\n",
                       status, word_buffer);
                goto err;
            }
            continue;
        }
        else if (status == HASHMAP_ERROR)
        {
            printf("main(): hashmap_get(): error: %ld, word: %s\n",
                   status, word_buffer);
            goto err;
        }

        status = hashmap_add(map, word_buffer, 1);
        if (status != HASHMAP_OK)
        {
            printf("main(): hashmap_add(): error: %ld, word: %s\n",
                   status, word_buffer);
            goto err;
        }
    }

    struct timespec ts_stop;
    timespec_get(&ts_stop, TIME_UTC);

    char buffer[100];
    printf("stats: hashf=%s\n", hashf_name);
    strftime(buffer, sizeof buffer, "%T", gmtime(&ts_start.tv_sec));
    printf("stats: start_time=%s.%.9ld\n", buffer, ts_start.tv_nsec);
    strftime(buffer, sizeof buffer, "%T", gmtime(&ts_stop.tv_sec));
    printf("stats: stop_time=%s.%.9ld\n", buffer, ts_stop.tv_nsec);

    printf("stats: map_size=%lu\n", map->size);
    printf("stats: collisions=%lu\n", map->collisions);
    printf("stats: wordcount=%lu\n", wordcount);
    printf("stats: charcount=%lu\n", charcount);

    // hashmap_print(map);

    hashmap_free(map);
    fclose(f1);
    free(value);
    return EXIT_SUCCESS;

    err:
    hashmap_print(map);
    free(value);
    hashmap_free(map);
    fclose(f1);
    return EXIT_FAILURE;
}
