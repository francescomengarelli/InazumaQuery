#if defined(__linux__)
#  define _POSIX_C_SOURCE 200809L
#endif

#include "InazumaQuery/utils/hash_map.h"


#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "InazumaQuery/core/errno.h"

struct ina_hash_map_t
{
    char **keys;
    uint16_t *values;
    bool *used;
    size_t count;
};

unsigned int JSHash(const char *str, unsigned int length)
{
    unsigned int hash = 1315423911;
    unsigned int i = 0;

    for (i = 0; i < length; ++str, ++i)
    {
        hash ^= ((hash << 5) + (*str) + (hash >> 2));
    }

    return hash;
}

ina_hash_map_t *ina_hash_map_create(size_t count)
{
    ina_hash_map_t *hmap = malloc(sizeof(*hmap));
    if (!hmap)
    {
        ina_stderrno = errno;
        ina_errno = INA_ERRT_STD;
        return NULL;
    }

    hmap->values = malloc(count * sizeof(*hmap->values));
    hmap->used = malloc(count * sizeof(*hmap->used));
    hmap->keys = malloc(count * sizeof(*hmap->keys));
    hmap->count = count;

    if (!hmap->values || !hmap->used)
    {
        ina_hash_map_destroy(&hmap);
        ina_stderrno = errno;
        ina_errno = INA_ERRT_STD;
        return NULL;
    }

    for (size_t i = 0; i < count; ++i)
    {
        hmap->used[i] = false;
    }

    return hmap;
}

int ina_hash_map_add(ina_hash_map_t *hmap, char const *key, uint16_t value)
{
    unsigned int h = JSHash(key, strlen(key)) % hmap->count;

    unsigned int original_h = h;
    uint16_t i = 1;
    while (hmap->used[h])
    {
        if (strcmp(hmap->keys[h], key) == 0)
        {
            hmap->values[h] = value;
            return 1;
        }

        h = (original_h + i * i) % hmap->count;
        i++;

        if (h == original_h)
        {
            ina_errno = INA_ERRT_HASH_MAP_NOSPACE;
            return 0;
        }
    }

    hmap->keys[h] = strdup(key);
    hmap->used[h] = true;
    hmap->values[h] = value;

    return 1;
}

uint16_t ina_hash_map_get(ina_hash_map_t *hmap, char const *key, bool *found)
{
    unsigned int h = JSHash(key, strlen(key)) % hmap->count;

    unsigned int original_h = h;
    uint16_t i = 1;
    while (hmap->used[h])
    {
        if (strcmp(hmap->keys[h], key) == 0)
        {
            *found = true;
            return hmap->values[h];
        }

        h = (original_h + i * i) % hmap->count;
        i++;

        if (h == original_h)
        {
            break;
        }
    }

    *found = false;
    ina_errno = INA_ERRT_HASH_MAP_NOTFOUND;
    return 0;
}

void ina_hash_map_destroy(ina_hash_map_t **hmap)
{
    if (!(hmap) || !(*hmap)) return;

    if ((*hmap)->used) free((*hmap)->used);
    if ((*hmap)->values) free((*hmap)->values);

    for (size_t i = 0; i < (*hmap)->count; ++i)
    {
        if ((*hmap)->keys[i]) free((*hmap)->keys[i]);
    }

    *hmap = NULL;
    return;
}
