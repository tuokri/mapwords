#ifndef MAPWORDS_HASH_H
#define MAPWORDS_HASH_H

#include <stdint.h>

typedef uint64_t hash_t;

hash_t
hash_djb2(char *str);

hash_t
hash_sdbm(char *str);

#endif //MAPWORDS_HASH_H
