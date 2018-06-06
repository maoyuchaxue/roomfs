#ifndef _ROOMFS_INVENTORY
#define _ROOMFS_INVENTORY

#include "item.h"
#include <time.h>
#include <stdlib.h>


struct inventory {
    struct item **item_list;
    int item_list_len;
};

void inventory_init();

int inventory_has_item(struct item *cur_item);

void inventory_add_item(struct item *cur_item);
void inventory_remove_item(struct item *cur_item);

#endif