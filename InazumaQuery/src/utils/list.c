#include "InazumaQuery/utils/list.h"

#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "InazumaQuery/core/errno.h"

#define LIST_GROW_FACTOR 3.0f

int ensure_space(ina_list_t *ls, size_t count);


struct ina_list_t
{
    void *buf;
    size_t buf_size;
    size_t buf_used;

    size_t e_size;
    size_t e_count;
};

ina_list_t *ina_list_create(size_t e_size)
{
    ina_list_t *ls = malloc(sizeof(*ls));

    if (!ls)
    {
        ina_stderrno = errno;
        ina_errno = INA_ERRT_STD;
        return NULL;
    }

    ls->e_size = e_size;
    ls->e_count = 0;

    ls->buf_used = 0;
    ls->buf_size = 0;

    ls->buf = NULL;

    return ls;
}

int ensure_space(ina_list_t *ls, size_t count)
{
    if (!ls)
    {
        ina_errno = INA_ERRT_PARAM_NULL;
        return 0;
    }

    size_t required = ls->e_size * count;
    size_t new_size = ls->buf_size;

    while (new_size - ls->buf_used < required)
    {
        if (new_size == 0)
        {
            new_size += ls->e_size * LIST_GROW_FACTOR;
        }
        else
        {
            new_size += new_size * (1.0f / LIST_GROW_FACTOR);
        }
    }
    if (new_size == ls->buf_size) return 1;

    // realloc work just as malloc if ls->buf is NULL
    void *temp = realloc(ls->buf, new_size);

    if (!temp)
    {
        ina_stderrno = errno;
        ina_errno = INA_ERRT_STD;
        return 0;
    }

    ls->buf = temp;
    ls->buf_size = new_size;
    return 1;
}

int ina_list_add(ina_list_t *ls, void *e)
{
    if (!ls)
    {
        ina_errno = INA_ERRT_PARAM_NULL;
        return 0;
    }

    if (!ensure_space(ls, 1)) return 0;

    void *pos = ls->buf + (ls->e_count * ls->e_size);

    memcpy(pos, e, ls->e_size);

    ls->buf_used += ls->e_size;
    ls->e_count++;

    return 1;
}

void *ina_list_at(ina_list_t const *ls, size_t id)
{
    if (!ls)
    {
        ina_errno = INA_ERRT_PARAM_NULL;
        return NULL;
    }

    if (id >= ls->e_count)
    {
        ina_errno = INA_ERRT_LIST_OOB;
        return NULL;
    }

    void *e = ls->buf + ls->e_size * id;

    return e;
}

bool ina_list_copy(ina_list_t *dst, ina_list_t const *src, size_t offset,
                   size_t max_count)
{
    if (!dst || !src)
    {
        ina_errno = INA_ERRT_PARAM_NULL;
        return false;
    }

    if (offset >= ina_list_count(src) ||
        ina_list_sizeof_element(dst) != ina_list_sizeof_element(src))
    {
        ina_errno = INA_ERRT_PARAM_INVALID;
        return false;
    }

    size_t src_count = ina_list_count(src);
    size_t avail = src_count - offset;
    size_t count = avail < max_count ? avail : max_count;

    if (!ensure_space(dst, count))
    {
        return false;
    }

    memcpy(dst->buf + dst->buf_used,
           src->buf + (offset * ina_list_sizeof_element(src)),
           count * ina_list_sizeof_element(src));

    dst->e_count += count;
    dst->buf_used += src->e_size * count;

    return true;
}

void ina_list_destroy(ina_list_t **ls)
{
    if (!ls) return;
    if (!(*ls)) return;


    if ((*ls)->buf) free((*ls)->buf);

    free(*ls);

    return;
}

size_t ina_list_count(ina_list_t const *ls)
{
    if (!ls)
    {
        return 0;
    }

    return ls->e_count;
}

size_t ina_list_sizeof_element(ina_list_t const *ls)
{
    if (!ls) return 0;

    return ls->e_size;
}

void ina_list_foreach(ina_list_t const *ls, ina_list_iterator_fn_t iterator_fn)
{
    size_t count = ina_list_count(ls);
    for (size_t i = 0; i < count; ++i)
    {
        iterator_fn(ina_list_at(ls, i), i, count);
    }
}
