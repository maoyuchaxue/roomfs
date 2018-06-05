
#include "item.h"

void item_init(struct item *cur_item) {
    cur_item->reaction_list_len = 0;
    cur_item->reaction_list = malloc(0);
}

void item_add_reaction(struct item *parent, struct reaction *target_reaction) {
    parent->reaction_list_len++;
    parent->reaction_list = realloc(parent->reaction_list, parent->reaction_list_len * sizeof(struct reaction *));
    parent->reaction_list[parent->reaction_list_len - 1] = target_reaction;
}

void item_trigger_reactions(struct item *cur_item) {
    for (int i = 0; i < cur_item->reaction_list_len; i++) {
        int ret = trigger_reaction(cur_item->reaction_list[i]);
        if (ret > 0) break;
    }
}