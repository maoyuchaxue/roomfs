#ifndef _ROOMFS_ITEM
#define _ROOMFS_ITEM

#include "room.h"
#include "event.h"
#include "reaction.h"
#include "description.h"

struct room;

struct item {
    char *name;
    struct description *description;
    char *input_buffer;
    struct room *owner;
    struct reaction **reaction_list;
    int reaction_list_len;

    int is_in_inventory;
    unsigned long long inventory_flag;
};

void item_init(struct item *cur_item);
// void item_add_event(struct item *parent, struct event *target_event);
// void item_trigger_events(struct item *cur_item);
void item_add_reaction(struct item *parent, struct reaction *target_reaction);
void item_trigger_reactions(struct item *cur_item);



#endif