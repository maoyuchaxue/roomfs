#ifndef _ROOMFS_ITEM
#define _ROOMFS_ITEM

#include "room.h"

struct room;

enum item_type {
    TRIGGER_ONCE, PICKABLE
};

struct item {
    char *name;
    char *description;
    enum item_type type;
    struct room *owner;
    struct event *event;
};

#endif