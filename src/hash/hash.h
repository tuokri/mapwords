#ifndef MAPWORDS_HASH_H
#define MAPWORDS_HASH_H

#ifdef __x86_64
#include <x86intrin.h>
#endif

#include <stdint.h>
#include <stddef.h>

typedef uint32_t hash_t;

/**
 *
 * @param buffer
 * @param size
 * @return
 */
hash_t
hash_djb2(char* buffer, size_t size);

/**
 *
 * @param buffer
 * @param size
 * @return
 */
hash_t
hash_sdbm(char* buffer, size_t size);

/**
 *
 * @param buffer
 * @param size
 * @return
 */
hash_t
hash_default_crc32(char* buffer, size_t size);

/**
 *
 * @param buffer
 * @param size
 * @return
 */
hash_t
hash_sse42_crc32(char* buffer, size_t size);

/**
 *
 * @param buffer
 * @param size
 * @return
 */
hash_t
hash_crc32(char* buffer, size_t size);

#endif //MAPWORDS_HASH_H
