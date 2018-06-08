
#include "item.h"

void item_init(struct item *cur_item) {
    cur_item->reaction_list_len = 0;
    cur_item->reaction_list = malloc(0);
    cur_item->input_buffer = malloc(sizeof(char) * MAX_INPUT_BUFFER);
    memset(cur_item->input_buffer, 0, MAX_INPUT_BUFFER);
    cur_item->is_in_inventory = 0;
    cur_item->inventory_flag = 0;
    cur_item->description = NULL;
}

void item_add_reaction(struct item *parent, struct reaction *target_reaction) {
    parent->reaction_list_len++;
    parent->reaction_list = realloc(parent->reaction_list, parent->reaction_list_len * sizeof(struct reaction *));
    parent->reaction_list[parent->reaction_list_len - 1] = target_reaction;
}

void item_trigger_reactions(struct item *cur_item) {
    if (cur_item->is_in_inventory) {
        return ;
    }

    for (int i = 0; i < cur_item->reaction_list_len; i++) {
        int ret = trigger_reaction(cur_item->reaction_list[i]);
        if (ret > 0) break;
    }
}