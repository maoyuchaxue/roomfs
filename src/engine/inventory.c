#include "inventory.h"

struct inventory inv;

void inventory_init() {
    inv.item_list = malloc(0);
    inv.item_list_len = 0;

    srandom(time(0));
}

unsigned long long allocate_new_inventory_tag() {
    while (1) {
        unsigned long long r = random();

        int has_used = 0;
        for (int i = 0; i < inv.item_list_len; i++) {
            if (inv.item_list[i]->inventory_flag == r) {
                has_used = 1;
            }
        }

        if (!has_used) {
            return r;
        }
    }
}


int inventory_has_item(struct item *cur_item) {
    for (int i = 0; i < inv.item_list_len; i++) {
        if (inv.item_list[i] == cur_item) {
            return 1;
        }
    }
    return 0;
}

void inventory_add_item(struct item *cur_item) {
    if (inventory_has_item(cur_item)) {
        return ;
    }

    cur_item->is_in_inventory = 1;
    if (cur_item->inventory_flag == 0) {
        unsigned long long r = allocate_new_inventory_tag();
        cur_item->inventory_flag = r;
    }

    inv.item_list_len++;
    inv.item_list = realloc(inv.item_list, inv.item_list_len * sizeof(struct item *));
    inv.item_list[inv.item_list_len - 1] = cur_item;
}

void inventory_remove_item(struct item *cur_item) {

    for (int i = 0; i < inv.item_list_len; i++) {
        if (inv.item_list[i] == cur_item) {
            for (int j = i; j < inv.item_list_len-1; j++) {
                inv.item_list[j] = inv.item_list[j+1];
            }
            inv.item_list_len--;
            struct item *removed_item = inv.item_list[inv.item_list_len];
            removed_item->is_in_inventory = 0;
            break;
        }
    }
}
