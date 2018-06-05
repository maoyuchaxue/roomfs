#ifndef _ROOMFS_ITEM
#define _ROOMFS_ITEM

#include "room.h"
#include "event.h"
#include "reaction.h"

struct room;


struct item {
    char *name;
    char *description;
    // TODO: int is_in_inventory;
    struct room *owner;
    struct reaction **reaction_list;
    int reaction_list_len;
};

void item_init(struct item *cur_item);
// void item_add_event(struct item *parent, struct event *target_event);
// void item_trigger_events(struct item *cur_item);
void item_add_reaction(struct item *parent, struct reaction *target_reaction);
void item_trigger_reactions(struct item *cur_item);

#endif