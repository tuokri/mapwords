#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <time.h>

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

    hash_t (*hashf)(const char*) = NULL;
    char hashf_name[HASHF_NAME_MAX_LENGTH] = {'\0'};

    char* fname1 = NULL;
    int opt;
    const char* short_opt = "f:h:";
    struct option long_opt[] =
        {
            {"file", required_argument, NULL, 'f'},
            {"hashf", required_argument, NULL, 'h'},
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

    printf("file to be read: %s\n", fname1);
    FILE* f1 = fopen(fname1, "r");
    if (!f1)
    {
        printf("error opening file\n");
        return EXIT_FAILURE;
    }

    char word_buffer[WORD_SIZE] = {'\0'};
    uint64_t wordcount = 0;
    uint64_t charcount = 0;

    hashf = get_hashf(hashf_name);
    if (hashf == NULL)
    {
        hashf = hash_crc32;
    }

    hashmap_map_t* map = hashmap_init(hashf);
    if (!map)
    {
        printf("error initializing map in main()\n");
        return EXIT_FAILURE;
    }

    int64_t* value = NULL;
    int64_t status;
    while (read_next_word(f1, word_buffer, WORD_SIZE) == 1)
    {
        str_tolower(word_buffer);
        wordcount++;
        charcount += strlen(word_buffer);

        value = calloc(1, sizeof(int64_t));
        if (!value)
        {
            printf("error allocating memory in main()\n");
            return EXIT_FAILURE;
        }

        status = hashmap_get(map, word_buffer, value);
        if (status == HASHMAP_OK)
        {
            // printf("%s->%ld\n", word_buffer, *value);
        }
        free(value);

        status = hashmap_add(map, word_buffer, 0);
        if (status != HASHMAP_OK)
        {
            switch (status)
            {
                case HASHMAP_KEY_ALREADY_IN_MAP:
//                    printf("hashmap_add(): key=%s already in map\n", word_buffer);
//                    for (uint64_t i = 0; i < map->capacity; ++i)
//                    {
//                        hashmap_bucket_t bucket = map->buckets[i];
//                        if (bucket.in_use)
//                        {
//                            printf("(%lu): [%s->%lu]\n", i, bucket.key, bucket.value);
//                        }
//                    }
                    break;
                default:
                    printf("hashmap_add(): error: %ld\n", status);
                    return EXIT_FAILURE;
            }
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

    hashmap_free(map);
    fclose(f1);
    return EXIT_SUCCESS;
}
