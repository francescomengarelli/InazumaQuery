#ifndef INAZUMAQUERY_UTILS_CSV_H_
#define INAZUMAQUERY_UTILS_CSV_H_

#include <stdbool.h>
#include <stdint.h>

#include "InazumaQuery/core/core.h"

typedef struct ina_csv_t ina_csv_t;

// TODO create a csv export function
// may be useful

INA_BEGIN_EXTERN_C

INA_API ina_csv_t *ina_csv_create(char const *path);

INA_API char const *ina_csv_get_cell(ina_csv_t *csv, uint32_t row,
                                     uint32_t col);

INA_API uint32_t ina_csv_row_count(ina_csv_t *csv);

INA_API void ina_csv_destroy(ina_csv_t **csv);

INA_END_EXTERN_C

#endif // INAZUMAQUERY_UTILS_CSV_H_
