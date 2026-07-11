#ifndef INAZUMAQUERY_UTILS_HASH_MAP_H_
#define INAZUMAQUERY_UTILS_HASH_MAP_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "InazumaQuery/core/core.h"

// TODO make hash map work with ina_list_t abstraction as well.
// already did this with sorting and filtering algorithms, should do it
// here for consistency and it may be useful in the future

// TODO make hash map dynamic and not of a fixed size
// just like with ina_list_t. Avoid allocating too much memory

typedef struct ina_hash_map_t ina_hash_map_t;

INA_BEGIN_EXTERN_C

INA_API ina_hash_map_t *ina_hash_map_create(size_t count);

INA_API int ina_hash_map_add(ina_hash_map_t *hmap, char const *key,
                             uint16_t value);

INA_API uint16_t ina_hash_map_get(ina_hash_map_t *hmap, char const *key,
                                  bool *found);

INA_API void ina_hash_map_destroy(ina_hash_map_t **hmap);

INA_END_EXTERN_C

#endif // INAZUMAQUERY_UTILS_HASH_MAP_H_
