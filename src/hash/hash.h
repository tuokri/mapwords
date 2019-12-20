#ifndef MAPWORDS_HASH_H
#define MAPWORDS_HASH_H

#include <inttypes.h>

#include "common.h"

// Maximum length for hash function name string.
#define HASHF_NAME_MAX_LENGTH 128

typedef uint64_t hash_t;

// All hash functions accept a *null-terminated* char buffer
// and return the calculated hash value of type hash_t.

// Bernstein DJB2 hash.
hash_t
hash_djb2(const char* buffer);

// SDBM general purpose hash.
hash_t
hash_sdbm(const char* buffer);

// Java style String hashCode.
hash_t
hash_java(const char* buffer);

// Get hash function pointer from string.
hash_t (* get_hashf(const char*))(const char*);

#endif //MAPWORDS_HASH_H
