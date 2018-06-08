#include "description.h"


struct description * new_description() {
    struct description *des = malloc(sizeof(struct description));
    des->item_list = malloc(0);
    des->item_list_len = 0;
    des->owner = NULL;
    return des;
}

void description_add_item(struct description *parent, struct description_item *des_item) {
    parent->item_list_len++;
    parent->item_list = realloc(parent->item_list, parent->item_list_len * sizeof(struct description_item *));
    parent->item_list[parent->item_list_len - 1] = des_item;
}

char *description_item_gen_text(struct description_item *item) {
    char *text = NULL;
    switch (item->type) {
        case D_STRING:
            text = malloc(sizeof(char) * MAX_DESCRIPTION_LEN);
            strcpy(text, item->text);
            return text;
        case D_GS:
            return global_state_to_text(item->related_gs, item->text);
        default:
            assert(0);
    }
}

char *description_gen_text(struct description *des) {
    char *text = malloc(sizeof(char) * MAX_DESCRIPTION_LEN);
    text[0] = '\0';
    for (int i = 0; i < des->item_list_len; i++) {
        struct description_item *item = des->item_list[i];
        char *item_text = description_item_gen_text(item);
        text = strcat(text, item_text);
        printf("text slice: %s\n", item_text);
        free(item_text);
    }
    return text;
}