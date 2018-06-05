#ifndef _ROOMFS_ROOM
#define _ROOMFS_ROOM

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "item.h"

struct room {
    char *name;
    char *description;
    struct room **adjacent_rooms;
    struct item **items;
    int total_adjacent_rooms;
    int total_items;
};

// void engine_init(const char *path);

void room_init(struct room *cur_room);

int room_has_adj_room(struct room *parent, struct room *target_room);
void room_add_adj_room(struct room *parent, struct room *target_room);
void room_remove_adj_room(struct room *parent, struct room *target_room);

int room_has_item(struct room *parent, struct item *target_item);
void room_add_item(struct room *parent, struct item *target_item);
void room_remove_item(struct room *parent, struct item *target_item);

#endif