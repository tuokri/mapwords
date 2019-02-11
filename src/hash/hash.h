#ifndef MAPWORDS_HASH_H
#define MAPWORDS_HASH_H

#include <stdint.h>

typedef uint64_t hash_t;

hash_t
hash_djb2(char* buffer, size_t size);

hash_t
hash_sdbm(char* buffer, size_t size);

hash_t
hash_sse42_crc32(char* buffer, size_t size);

#endif //MAPWORDS_HASH_H
