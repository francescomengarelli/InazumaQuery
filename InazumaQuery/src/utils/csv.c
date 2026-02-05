#include "InazumaQuery/utils/csv.h"
#include "InazumaQuery/core/errno.h"
#include "InazumaQuery/utils/list.h"
#include "InazumaQuery/utils/utils.h"

#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

struct ina_csv_t
{
    char *raw_content;
    ina_list_t *cells;
    ina_list_t *row_start_index;
    uint32_t row_count;
};

bool parse_csv(ina_csv_t *csv);

ina_csv_t *ina_csv_create(char const *path)
{
    ina_csv_t *csv = malloc(sizeof(*csv));

    if (!csv)
    {
        ina_errno = INA_ERRT_STD;
        ina_stderrno = errno;
        return NULL;
    }

    csv->raw_content = ina_file_readall(path);
    if (!csv->raw_content)
    {
        free(csv);
        return NULL;
    }

    if (!parse_csv(csv))
    {
        ina_csv_destroy(&csv);
        return NULL;
    }

    return csv;
}

char const *ina_csv_get_cell(ina_csv_t *csv, uint32_t row, uint32_t col)
{
    if (!csv)
    {
        ina_errno = INA_ERRT_PARAM_NULL;
        return NULL;
    }

    uint32_t *row_start = ina_list_at(csv->row_start_index, row);
    if (!row_start) return "\0";

    char **cell_ptr = ina_list_at(csv->cells, (*row_start) + col);
    if (!cell_ptr) return "\0";

    return *cell_ptr;
}

void ina_csv_destroy(ina_csv_t **csv)
{
    if (!csv || !(*csv)) return;

    if ((*csv)->raw_content) free((*csv)->raw_content);

    ina_list_destroy(&(*csv)->cells);
    ina_list_destroy(&(*csv)->row_start_index);

    free(*csv);
    *csv = NULL;
}

bool is_separator(char c)
{
    if (c == ',' || c == '\n' || c == '\0') return true;

    return false;
}

bool parse_csv(ina_csv_t *csv)
{
    if (!csv || !csv->raw_content)
    {
        ina_errno = INA_ERRT_PARAM_NULL;
        return false;
    }

    csv->cells = ina_list_create(sizeof(char *));
    csv->row_start_index = ina_list_create(sizeof(uint32_t));

    int current_row = 0;
    uint32_t current_cell_idx = 0;

    ina_list_add(csv->row_start_index, &current_cell_idx);

    char *cursor = csv->raw_content;

    ina_list_add(csv->cells, &cursor);
    current_cell_idx++;

    while (*cursor != '\0')
    {
        if (*cursor == ',' || *cursor == '\n')
        {
            char delimiter = *cursor;

            *cursor = '\0';

            char *next_cell = cursor + 1;
            if (delimiter == '\n')
            {
                current_row++;
                ina_list_add(csv->row_start_index, &current_cell_idx);
            }
            if (*next_cell != '\0')
            {
                ina_list_add(csv->cells, &next_cell);
                current_cell_idx++;
            }
        }

        cursor++;
    }

    csv->row_count = current_row + 1;
    return true;
}

uint32_t ina_csv_row_count(ina_csv_t *csv)
{
    return csv ? csv->row_count : 0;
}
