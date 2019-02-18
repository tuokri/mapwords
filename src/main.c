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

    FILE* f1 = fopen(fname1, "r");
    // FILE* f2 = fopen(fname2, "r");

    char word[WORD_SIZE + 1];
    hashmap_t* map1 = hashmap_init(hash_sdbm, hash_crc32);
    // hashmap_t* map2 = hashmap_init(hash_sdbm, hash_crc32);

    uint32_t wordcount = 0;
    while(read_next_word(f1, word, WORD_SIZE) == 1)
    {
        str_tolower(word);
        // puts(word);
        wordcount++;
        
        char* str1 = malloc(strlen(word) + 1);
        strcpy(str1, word);
        hashmap_key_t key1 = {strlen(str1) + 1, str1};
        hashmap_key_t* key1p = malloc(sizeof(key1));
        memcpy(key1p, &key1, sizeof(key1));
        int count1 = 1;
        int* c1 = &count1;

        hashmap_add(map1, key1p, c1);
        // int64_t result = hashmap_add(map1, key1p, c1);
        // printf("%ld\n", result);
        // printf("---------\n");
    }

    // printf("map1->size=%u\n",map1->size);
    // printf("map1->capacity=%u\n", map1->capacity);
    // printf("wordcount=%u\n", wordcount);

    // while(read_next_word(f2, word, WORD_SIZE) == 1)
    // {
    //     str_tolower(word);
    //     puts(word);

    //     char* str2 = malloc(strlen(word) + 1);
    //     strcpy(str2, word);
    //     hashmap_key_t key2 = {strlen(str2) + 1, str2};
    //     int count2 = 1;
    //     int* c2 = &count2;

    //     hashmap_add(map2, &key2, c2);
    // }

    fclose(f1);
    // fclose(f2);
    return EXIT_SUCCESS;
}
