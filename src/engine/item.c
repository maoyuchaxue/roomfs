
#include "item.h"

void item_init(struct item *cur_item) {
    cur_item->event_list_len = 0;
    cur_item->event_list = malloc(0);
}

void item_add_event(struct item *parent, struct event *target_event) {
    parent->event_list_len++;
    parent->event_list = realloc(parent->event_list, parent->event_list_len * sizeof(struct event *));
    parent->event_list[parent->event_list_len - 1] = target_event;
}

void item_trigger_events(struct item *cur_item) {
    for (int i = 0; i < cur_item->event_list_len; i++) {
        trigger_event(cur_item->event_list[i]);
    }
}