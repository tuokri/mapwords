#ifndef MAPWORDS_HASH_H
#define MAPWORDS_HASH_H

#ifdef __x86_64

#include <x86intrin.h>

#endif

#include <stdint.h>
#include <stddef.h>

typedef uint64_t hash_t;

// All hash functions accept a char buffer
// and return the calculated hash value of type hash_t.

hash_t
hash_djb2(const char* buffer);

hash_t
hash_sdbm(const char* buffer);

// https://docs.oracle.com/javase/6/docs/api/java/lang/String.html#hashCode()
hash_t
hash_java(const char* buffer);

hash_t
hash_default_crc32(const char* buffer);

hash_t
hash_sse42_crc32(const char* buffer);

hash_t
hash_crc32(const char* buffer);

#endif //MAPWORDS_HASH_H
