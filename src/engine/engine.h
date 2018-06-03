
#ifndef _ROOMFS_ENGINE
#define _ROOMFS_ENGINE

#include "room.h"
#include "item.h"
#include "event.h"

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
};

void engine_init(const char *path);

void engine_destroy();

#endif