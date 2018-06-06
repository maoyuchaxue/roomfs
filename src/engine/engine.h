
#ifndef _ROOMFS_ENGINE
#define _ROOMFS_ENGINE

#include "room.h"
#include "item.h"
#include "event.h"
#include "const.h"
#include "inventory.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct game_engine_s {
    struct room *entry_room;

    struct room **rooms;
    char **room_names;
    int total_rooms;

    struct item **items;
    char **item_names;
    int total_items;

    int *global_states;
    char **global_state_names;
    int total_global_states;
};

void engine_init(const char *path);
void engine_destroy();
struct room *name_to_room(const char *room_name);
struct item *name_to_item(const char *item_name);

#endif