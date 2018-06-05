#ifndef _ROOMFS_ITEM
#define _ROOMFS_ITEM

#include "room.h"
#include "event.h"

struct room;


struct item {
    char *name;
    char *description;
    // TODO: int is_in_inventory;
    struct room *owner;
    struct event **event_list;
    int event_list_len;
};

void item_init(struct item *cur_item);
void item_add_event(struct item *parent, struct event *target_event);
void item_trigger_events(struct item *cur_item);

#endif