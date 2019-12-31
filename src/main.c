#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <inttypes.h>

#include "hash.h"
#include "hashmap.h"
#include "util.h"

#ifdef _WIN32

#include <windows.h>

static LARGE_INTEGER frequency;
static LARGE_INTEGER start;
static LARGE_INTEGER end;
static double interval;

#define TIMER_BEGIN() { \
    QueryPerformanceFrequency(&frequency); \
    QueryPerformanceCounter(&start); \
}

#define TIMER_END() { \
    QueryPerformanceCounter(&end); \
    interval = (double) (end.QuadPart - start.QuadPart) / frequency.QuadPart; \
    printf("stats: duration=%f\n", interval); \
}

#elif defined(__linux__)

#include <time.h>

static struct timespec ts_start;
static struct timespec ts_stop;

#define TIMER_BEGIN() { \
    timespec_get(&ts_start, TIME_UTC); \
}

#define TIMER_END() { \
    timespec_get(&ts_stop, TIME_UTC); \
    double duration = (double) (ts_stop.tv_sec - ts_start.tv_sec) \
        + ((double) (ts_stop.tv_nsec - ts_start.tv_nsec) / 1000000000L); \
    printf("stats: duration=%f\n", duration); \
}

#endif

// Assume generous 511 (+ '\0') maximum word length.
#define WORD_SIZE 512

int
main(int argc, char** argv)
{
    TIMER_BEGIN();

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
        strcpy(hashf_name, "hash_djb2");
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
                printf("main(): hashmap_update(): error: %"PRId64", word: %s\n",
                       status, word_buffer);
                goto err;
            }
            continue;
        }
        else if (status == HASHMAP_ERROR)
        {
            printf("main(): hashmap_get(): error: %"PRId64", word: %s\n",
                   status, word_buffer);
            goto err;
        }

        status = hashmap_add(map, word_buffer, 1);
        if (status != HASHMAP_OK)
        {
            printf("main(): hashmap_add(): error: %"PRId64", word: %s\n",
                   status, word_buffer);
            goto err;
        }
    }

    hashmap_bucket_t* results = NULL;
    status = hashmap_sort_by_value(map, 0, map->capacity - 1, &results);
    if (status != HASHMAP_OK)
    {
        printf("main(): hashmap_sort_by_value(): error: %"PRId64"\n",
               status);
    }
    else
    {
        uint64_t j = 1;
        puts("100 most common words:");
        uint64_t limit = (map->capacity >= 100) ? (map->capacity - 100) : (map->capacity - map->size);
        for (uint64_t i = map->capacity - 1; i >= limit; --i)
        {
            printf("%-3lu: %-16s %16lu\n", j++, results[i].key, results[i].value);
        }
    }

    TIMER_END();

    printf("stats: hashf=%s\n", hashf_name);
    printf("stats: map_size=%"PRIu64"\n", map->size);
    printf("stats: collisions=%"PRIu64"\n", map->collisions);
    printf("stats: word_count=%"PRIu64"\n", wordcount);
    printf("stats: char_count=%"PRIu64"\n", charcount);
    printf("stats: rehash_count=%"PRIu64"\n", map->rehashes);
    printf("stats: capacity=%"PRIu64"\n", map->capacity);

    // hashmap_print(map);

    if (results)
    {
        hashmap_free_buckets(results, map->capacity);
    }
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
