#include "InazumaQuery/utils/utils.h"
#include "InazumaQuery/core/core.h"

#include <ctype.h>
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wctype.h>

#include "InazumaQuery/core/errno.h"


char *ina_file_readall(const char *path_to_file)
{
    FILE *fp = fopen(path_to_file, "rb");
    if (!fp)
    {
        ina_errno = INA_ERRT_STD;
        ina_stderrno = errno;
        return NULL;
    }

    if (fseek(fp, 0, SEEK_END) != 0)
    {
        fclose(fp);
        ina_errno = INA_ERRT_STD;
        ina_stderrno = errno;
        return NULL;
    }

    long filesize = ftell(fp);
    if (filesize < 0)
    {
        fclose(fp);
        ina_errno = INA_ERRT_STD;
        ina_stderrno = errno;
        return NULL;
    }

    rewind(fp);

    char *buffer = (char *)malloc((size_t)filesize + 1);
    if (!buffer)
    {
        fclose(fp);
        ina_errno = INA_ERRT_STD;
        ina_stderrno = ENOMEM;
        return NULL;
    }

    size_t bytes_read = fread(buffer, 1, (size_t)filesize, fp);
    fclose(fp);

    if (bytes_read != (size_t)filesize)
    {
        free(buffer);
        ina_errno = INA_ERRT_STD;
        ina_stderrno = errno;
        return NULL;
    }

    buffer[bytes_read] = '\0';
    return buffer;
}

void ina_normalise_string(char const *str, char *result, size_t len)
{
    size_t current = 0;

    // Process each character until '\0', skipping spaces
    for (size_t i = 0; str[i] != '\0' && current + 1 < len; ++i)
    {
        unsigned char c = (unsigned char)str[i];
        if (isspace(c)) continue;
        result[current++] = (char)tolower(c);
    }

    // Always terminate within bounds
    result[current] = '\0';
}

int ina_strcmp_normalised(char const *a, char const *b)
{
    size_t na = strlen(a) + 1;
    size_t nb = strlen(b) + 1;
    char norm1[INA_NAME_MAX_LEN], norm2[INA_NAME_MAX_LEN];

    ina_normalise_string(a, norm1, na);
    ina_normalise_string(b, norm2, nb);

    return strcmp(norm1, norm2);
}
