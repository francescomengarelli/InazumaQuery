#include "InazumaQuery/utils/csv.h"
#include "InazumaQuery/core/errno.h"

#include <stddef.h>
#include <stdio.h>
#include <time.h>

int main()
{
    clock_t start = clock();
    char *path = INA_DEBUG_DATA_DIR "/players/ie3.csv";
    printf("Creating DB from data at %s", path);
    ina_csv_t *csv = ina_csv_create(path);
    clock_t end = clock();

    if (!csv)
    {
        ina_perror("Failed to create csv from file '%s'", path);
        return 1;
    }

    double cpu_secs = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Time taken to parse csv: %.6f seconds\n", cpu_secs);

    ina_csv_destroy(&csv);

    return 0;
}
