#ifndef _ROOMFS_EVENT
#define _ROOMFS_EVENT

#include "room.h"
#include "item.h"

struct item;
struct room;

typedef void (*event_op_no_param)();
typedef void (*event_op_on_item)(struct item *);
typedef void (*event_op_on_room)(struct room *);

enum event_type {
    E_ADD_TO_INVENTORY,
    E_REMOVE_FROM_INVENTORY,
    E_CHANGE_ATTRIBUTE,
};

enum event_op_type {
    E_OP_NO_PARAM,
    E_OP_ON_ITEM,
    E_OP_ON_ROOM,
};

union event_op {
    event_op_no_param op_no_param;
    event_op_on_item op_on_item;
    event_op_on_room op_on_room;
};

struct event {
    char *name;
    char *description;
    struct item *owner;
    enum event_type type;
    union event_op op;
};

#endif