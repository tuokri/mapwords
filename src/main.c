#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

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
    hashmap_free(map);

    return EXIT_SUCCESS;
}
