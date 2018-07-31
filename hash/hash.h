#ifndef MAPWORDS_HASH_H
#define MAPWORDS_HASH_H

#include <stdint.h>

uint64_t
hash_djb2(char *str);

uint64_t
hash_sdbm(char *str);

#endif //MAPWORDS_HASH_H
