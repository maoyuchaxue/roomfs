#ifndef _ROOMFS_EVENT
#define _ROOMFS_EVENT

#include <assert.h>
#include "room.h"
#include "item.h"
#include "global_state.h"
#include "engine.h"
#include "eval.h"
#include "inventory.h"
#include "description.h"

struct item;
struct room;


enum event_op_type {
    E_OP_NO_PARAM,
    E_OP_ON_ITEM,
    E_OP_ON_ROOM,
    E_OP_ON_GLOBAL_STATE,
};

typedef void (*event_op)(int , void **);

enum event_sub_type_no_param {
    E_NOTHING,
};

enum event_sub_type_on_item {
    E_ADD_TO_INVENTORY,
    E_REMOVE_FROM_INVENTORY,
    E_CHANGE_ITEM_DESCRIPTION,
    E_UNLINK_FROM_OWNER,
    E_CLEAR_INPUT_BUFFER,
};

enum event_sub_type_on_room {
    E_CHANGE_ROOM_DESCRIPTION,
    E_ADD_LINK_TO_ITEM,
    E_ADD_LINK_TO_ROOM,
    E_REMOVE_LINK_TO_ITEM,
    E_REMOVE_LINK_TO_ROOM,
};

enum event_sub_type_on_global_state {
    E_SET_STATE,
};


union event_sub_type {
    enum event_sub_type_no_param sub_type_no_param;
    enum event_sub_type_on_item sub_type_on_item;
    enum event_sub_type_on_room sub_type_on_room;
    enum event_sub_type_on_global_state sub_type_on_global_state;
};

struct event {
    struct reaction *owner;
    enum event_op_type type;
    event_op op;
    union event_sub_type sub_type;
    void **paramv;
    int paramc;
};

struct event *construct_event(struct reaction *owner, const char *target1, const char *op, const char *target2);

void event_init(struct event *cur_event);
void trigger_event(struct event *cur_event);

#endif