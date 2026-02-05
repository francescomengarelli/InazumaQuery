#include "InazumaQuery/inazuma.h" // IWYU pragma: keep
#include "InazumaQuery/player.h"
#include "InazumaQuery/utils/list.h"

#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

// take a look at
// https://github.com/mengacpp/inazuma-api/blob/main/examples/query.c to
// understand how querying works first

// we use this simple function to calculate if a has alphabetical priority over
// b
int compare(ina_player_t const *a, ina_player_t const *b)
{
    size_t len = strlen(a->fullname_normalised) > strlen(b->fullname_normalised)
                     ? strlen(b->fullname_normalised)
                     : strlen(a->fullname_normalised);

    for (size_t i = 0; i < len; ++i)
    {
        uint8_t ca = tolower(a->fullname_normalised[i]);
        uint8_t cb = tolower(b->fullname_normalised[i]);

        if (ca == cb) continue;

        return ca - cb;
    };

    return 0;
}

bool filter(ina_player_t const *p)
{
    return true;
}

int main()
{
    ina_pdb_t *ie3_db = ina_pdb_create("ie3.csv");

    // query 100 players in alphabetical order starting from A.
    ina_list_t *players = ina_pdb_query(ie3_db, 100, filter, compare);

    ina_list_destroy(&players);

    ina_pdb_destroy(&ie3_db);

    return 0;
}
