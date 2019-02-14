#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hash.h"
#include "hashmap.h"

int main()
{
    printf("%u\n", hash_sdbm("oWo", 4));
    printf("%u\n", hash_sdbm("wubba lubba dub dub", 20));
    printf("%u\n", hash_djb2("oWo", 4));
    printf("%u\n", hash_djb2("wubba lubba dub dub", 20));
    printf("%u\n", hash_sse42_crc32("oWo", 4));
    printf("%u\n", hash_sse42_crc32("wubba lubba dub dub", 20));
    printf("%u\n", hash_default_crc32("oWo", 4));
    printf("%u\n", hash_default_crc32("wubba lubba dub dub", 20));

    hashmap_t* map = NULL;
    map = hashmap_init(hash_sdbm, hash_crc32);

    char* key1 = "koira";
    hashmap_key_t hmk = {strlen(key1) + 1, key1};
    int value = 666;

    printf("added index=%ld\n", hashmap_add(map, &hmk, &value));
    printf("index of \"koira\"=%ld\n", hashmap_find(map, &hmk));

    void* v = map->buckets[hashmap_find(map, &hmk)]->value;
    printf("%p\n", v);
    printf("%d\n", *(int*)v);

    hashmap_free(map);

    return EXIT_SUCCESS;
}
