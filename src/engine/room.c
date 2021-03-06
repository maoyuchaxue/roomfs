#include "room.h"

void room_init(struct room *cur_room) {
    cur_room->total_adjacent_rooms = 0;
    cur_room->adjacent_rooms = malloc(0);
    cur_room->total_items = 0;
    cur_room->items = malloc(0);
    cur_room->description = NULL;
}

int room_has_adj_room(struct room *parent, struct room *target_room) {
    if (parent == NULL) {
        return 0;
    }

    for (int i = 0; i < parent->total_adjacent_rooms; i++) {
        if (parent->adjacent_rooms[i] == target_room) {
            return 1;
        }
    }
    return 0;
}

void room_add_adj_room(struct room *parent, struct room *target_room) {
    if (parent == NULL) {
        return ;
    }

    if (room_has_adj_room(parent, target_room)) {
        return ;
    }

    parent->total_adjacent_rooms++;
    parent->adjacent_rooms = realloc(parent->adjacent_rooms, parent->total_adjacent_rooms * sizeof(struct room *));
    parent->adjacent_rooms[parent->total_adjacent_rooms - 1] = target_room;
}

void room_remove_adj_room(struct room *parent, struct room *target_room) {
    if (parent == NULL) {
        return ;
    }

    if (!room_has_adj_room(parent, target_room)) {
        return ;
    }

    for (int i = 0; i < parent->total_adjacent_rooms; i++) {
        if (parent->adjacent_rooms[i] == target_room) {
            struct room *tmp_room = parent->adjacent_rooms[parent->total_adjacent_rooms - 1];
            parent->adjacent_rooms[parent->total_adjacent_rooms - 1] = parent->adjacent_rooms[i];
            parent->adjacent_rooms[i] = tmp_room;
            break;
        }
    }

    parent->total_adjacent_rooms--;
}


int room_has_item(struct room *parent, struct item *target_item) {
    if (parent == NULL) {
        return 0;
    }

    for (int i = 0; i < parent->total_items; i++) {
        if (parent->items[i] == target_item) {
            return 1;
        }
    }
    return 0;
}

void room_add_item(struct room *parent, struct item *target_item) {
    if (parent == NULL) {
        return ;
    }

    if (room_has_item(parent, target_item)) {
        return ;
    }

    parent->total_items++;
    parent->items = realloc(parent->items, parent->total_items * sizeof(struct item *));
    parent->items[parent->total_items - 1] = target_item;
    target_item->owner = parent;
}

void room_remove_item(struct room *parent, struct item *target_item) {
    if (parent == NULL) {
        return ;
    }

    if (!room_has_item(parent, target_item)) {
        return ;
    }

    for (int i = 0; i < parent->total_items; i++) {
        if (parent->items[i] == target_item) {
            struct item *tmp_item = parent->items[parent->total_items - 1];
            parent->items[parent->total_items - 1] = parent->items[i];
            parent->items[i] = tmp_item;
            break;
        }
    }

    parent->total_items--;
}