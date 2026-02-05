#include "InazumaQuery/mdb.h"

#include <errno.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "InazumaQuery/core/core.h"
#include "InazumaQuery/core/errno.h"

#include "InazumaQuery/move.h"

#include "InazumaQuery/utils/csv.h"
#include "InazumaQuery/utils/filter.h"
#include "InazumaQuery/utils/hash_map.h"
#include "InazumaQuery/utils/list.h"
#include "InazumaQuery/utils/sort.h"
#include "InazumaQuery/utils/utils.h"


struct ina_mdb_t
{
    ina_list_t *moves;
    ina_hash_map_t *name_hmap;
};


bool ina_mdb_accept_all(ina_move_t const *e)
{
    return true;
}

bool assign_by_col(ina_move_t *mv, size_t col, char const *cell_content);

int ina_mdb_default_compare(ina_move_t const *e, ina_move_t const *e2)
{
    return e2->hex_id - e->hex_id;
}

ina_mdb_t *ina_mdb_create(char const *csv_path)
{
    ina_mdb_t *db = malloc(sizeof(*db));

    if (!db)
    {
        INA_STDERR;
        return NULL;
    }

    db->moves = ina_list_create(sizeof(ina_move_t));

    if (!db->moves)
    {
        INA_STDERR;
        ina_mdb_destroy(&db);
        return NULL;
    }

    db->name_hmap = ina_hash_map_create(INA_DB_MAX_COUNT);

    if (!db->name_hmap)
    {
        INA_STDERR;
        ina_mdb_destroy(&db);
        return NULL;
    }

    ina_csv_t *csv = ina_csv_create(csv_path);

    if(!csv) {
        INA_STDERR;
        ina_mdb_destroy(&db);
        return NULL;
    }
    for (size_t row = 0; row < ina_csv_row_count(csv); ++row)
    {
        ina_move_t move;
        for (size_t col = 0; col < 10; ++col)
        {
            char const *cell = ina_csv_get_cell(csv, row, col);
            assign_by_col(&move, col, cell);
        }

        ina_list_add(db->moves, &move);
        ina_hash_map_add(db->name_hmap, move.name_normalised,
                         ina_list_count(db->moves) - 1);
    }

    return db;
}

static ina_move_filter_fn_t g_filter_fn;
static ina_move_compare_fn_t g_compare_fn;

static bool filter_wrapper(void const *e)
{
    return g_filter_fn((ina_move_t const *)e);
}

static int compare_wrapper(void const *e, void const *e2)
{
    return g_compare_fn((ina_move_t const *)e, (ina_move_t const *)e2);
}

static void set(void *e, void const *e2)
{
    ina_move_t *move = e;

    ina_move_t const *move2 = e2;

    memcpy(move, move2, sizeof(ina_move_t));
}

ina_list_t *ina_mdb_query(ina_mdb_t const *db, uint16_t max_count,
                          ina_move_filter_fn_t filter_fn,
                          ina_move_compare_fn_t compare_fn)
{
    if (!db)
    {
        ina_errno = INA_ERRT_PARAM_NULL;
        return NULL;
    }

    g_compare_fn = compare_fn ? compare_fn : ina_mdb_default_compare;
    g_filter_fn = filter_fn ? filter_fn : ina_mdb_accept_all;

    ina_list_t *filtered = ina_filter(db->moves, filter_wrapper);
    ina_sort(filtered, compare_wrapper, set);

    if (max_count == 0) max_count = ina_list_count(db->moves);
    size_t count = max_count > ina_list_count(filtered)
                       ? ina_list_count(filtered)
                       : max_count;

    ina_list_t *result = ina_list_create(sizeof(ina_move_t));
    if (!ina_list_copy(result, filtered, 0, count))
    {
        ina_list_destroy(&filtered);
        ina_list_destroy(&result);
        return NULL;
    }

    ina_list_destroy(&filtered);

    return result;
}

ina_move_t const *ina_mdb_find_name(ina_mdb_t const *db, char const *name)
{
    if (!db || !name)
    {
        ina_errno = INA_ERRT_PARAM_NULL;
        return NULL;
    }
    char name_normalised[INA_NAME_MAX_LEN];

    ina_normalise_string(name, name_normalised, INA_NAME_MAX_LEN);

    bool found;
    unsigned int id = ina_hash_map_get(db->name_hmap, name_normalised, &found);

    if (!found) return NULL;

    return ina_list_at(db->moves, id);
}

void ina_mdb_destroy(ina_mdb_t **db)
{
    if (!(db) || (*db))
    {
        return;
    }

    if ((*db)->moves) ina_list_destroy(&(*db)->moves);

    if ((*db)->name_hmap) ina_hash_map_destroy(&(*db)->name_hmap);

    free(*db);
    return;
}

size_t ina_mdb_count(ina_mdb_t *db)
{
    return ina_list_count(db->moves);
}

#define ASSIGN_AND_BREAK(stat, base)                                           \
    do                                                                         \
    {                                                                          \
        mv->stat = strtol(cell_content, NULL, base);                           \
        break;                                                                 \
    } while (0)

bool assign_by_col(ina_move_t *mv, size_t col, char const *cell_content)
{
    switch (col)
    {
    case 0: // name
    {
        strncpy(mv->name, cell_content, INA_NAME_MAX_LEN);
        ina_normalise_string(mv->name, mv->name_normalised, INA_NAME_MAX_LEN);

        break;
    }
    case 1: // type
    {
        int is_block = ina_strcmp_normalised("block", cell_content) == 0;
        int is_dribble = ina_strcmp_normalised("dribble", cell_content) == 0;
        int is_keeper = ina_strcmp_normalised("keeper", cell_content) == 0;
        int is_shoot = ina_strcmp_normalised("shoot", cell_content) == 0;

        if (is_block)
            mv->type = INA_MOVE_TYPE_BLOCK;
        else if (is_dribble)
            mv->type = INA_MOVE_TYPE_DRIBBLE;
        else if (is_keeper)
            mv->type = INA_MOVE_TYPE_KEEPER;
        else if (is_shoot)
            mv->type = INA_MOVE_TYPE_SHOOT;
        else
        {
            ina_errno = INA_ERRT_PDB_CSV_INVALID_CONTENT;
            return 0;
        }

        break;
    }
    case 2: // element
    {
        int is_air = ina_strcmp_normalised("wind", cell_content) == 0;
        int is_earth = ina_strcmp_normalised("earth", cell_content) == 0;
        int is_fire = ina_strcmp_normalised("fire", cell_content) == 0;
        int is_wood = ina_strcmp_normalised("wood", cell_content) == 0;

        if (is_air)
            mv->element = INA_ELEMENT_WIND;
        else if (is_earth)
            mv->element = INA_ELEMENT_EARTH;
        else if (is_fire)
            mv->element = INA_ELEMENT_FIRE;
        else if (is_wood)
            mv->element = INA_ELEMENT_WOOD;
        else
        {
            ina_errno = INA_ERRT_PDB_CSV_INVALID_CONTENT;
            return 0;
        }

        break;
    }
    case 3: // fouls rate
    {
        ASSIGN_AND_BREAK(fouls_rate, 10);
        mv->fouls_rate /= 100.0f;
    }
    case 4: // base power
    {
        ASSIGN_AND_BREAK(base_power, 10);
    }
    case 5: // power at max level
    {
        ASSIGN_AND_BREAK(max_power, 10);
    }
    case 6: // tp cost
    {
        ASSIGN_AND_BREAK(tp_cost, 10);
    }
    case 7: // secondary type
    {
        int is_longshot = ina_strcmp_normalised("longshot", cell_content) == 0;
        int is_shotblock =
            ina_strcmp_normalised("shotblock", cell_content) == 0;
        int is_none = ina_strcmp_normalised("", cell_content) == 0;

        if (is_longshot)
            mv->secondary_type = INA_MOVE_SECONDARY_TYPE_LONGSHOT;
        else if (is_shotblock)
            mv->secondary_type = INA_MOVE_SECONDARY_TYPE_LONGSHOT;
        else if (is_none)
            mv->secondary_type = INA_MOVE_SECONDARY_TYPE_NONE;
        else
        {
            ina_errno = INA_ERRT_PDB_CSV_INVALID_CONTENT;
            return 0;
        }

        break;
    }
    case 8: // evolution type
    {
        int is_l_slow = ina_strcmp_normalised("L (slow)", cell_content) == 0;
        int is_l_medium =
            ina_strcmp_normalised("L (medium)", cell_content) == 0;
        int is_l_fast = ina_strcmp_normalised("L (fast)", cell_content) == 0;
        int is_shin_slow =
            ina_strcmp_normalised("Shin (slow)", cell_content) == 0;
        int is_shin_medium =
            ina_strcmp_normalised("Shin (medium)", cell_content) == 0;
        int is_shin_fast =
            ina_strcmp_normalised("Shin (fast)", cell_content) == 0;

        if (is_l_slow)
            mv->evolution_type = INA_MOVE_EVOLUTION_TYPE_L_SLOW;
        else if (is_l_medium)
            mv->evolution_type = INA_MOVE_EVOLUTION_TYPE_L_MEDIUM;
        else if (is_l_fast)
            mv->evolution_type = INA_MOVE_EVOLUTION_TYPE_L_FAST;
        else if (is_shin_slow)
            mv->evolution_type = INA_MOVE_EVOLUTION_TYPE_SHIN_SLOW;
        else if (is_shin_medium)
            mv->evolution_type = INA_MOVE_EVOLUTION_TYPE_SHIN_MEDIUM;
        else if (is_shin_fast)
            mv->evolution_type = INA_MOVE_EVOLUTION_TYPE_SHIN_FAST;
        else
        {
            ina_errno = INA_ERRT_PDB_CSV_INVALID_CONTENT;
            return 0;
        }

        break;
    }
    case 9: // hex id
    {
        ASSIGN_AND_BREAK(hex_id, 16);
    }
    default:
    {
        ina_stderrno = INA_ERRT_UNKNOWN;
        return false;
    }
    }
    return true;
}
