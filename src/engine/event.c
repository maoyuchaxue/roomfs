#include "event.h"


void op_room_change_description(int paramc, void **paramv) {
    assert(paramc == 2);
    struct room *target_room = (struct room *)paramv[0];
    char *description = (char *)paramv[1];
    target_room->description = description;
}

void op_room_add_link_to_item (int paramc, void **paramv) {
    assert(paramc == 2);
    struct room *parent = (struct room *)paramv[0];
    struct item *target_item = (struct item *)paramv[1];
    room_add_item(parent, target_item);
}

void op_room_add_link_to_room (int paramc, void **paramv) {
    assert(paramc == 2);
    struct room *parent = (struct room *)paramv[0];
    struct room *target_room = (struct room *)paramv[1];
    room_add_adj_room(parent, target_room);
}

void op_room_remove_link_to_item (int paramc, void **paramv) {
    assert(paramc == 2);
    struct room *parent = (struct room *)paramv[0];
    struct item *target_item = (struct item *)paramv[1];
    room_remove_item(parent, target_item);
}

void op_room_remove_link_to_room (int paramc, void **paramv) {
    assert(paramc == 2);
    struct room *parent = (struct room *)paramv[0];
    struct room *target_room = (struct room *)paramv[1];
    room_remove_adj_room(parent, target_room);
}

void op_item_change_description(int paramc, void **paramv) {
    assert(paramc == 2);
    struct item *target_item = (struct item *)paramv[0];
    char *description = (char *)paramv[1];
    target_item->description = description;
}

void op_item_add_to_inventory(int paramc, void **paramv) {
    assert(paramc == 1);
    struct item *target_item = (struct item *)paramv[0];
    inventory_add_item(target_item);
}

void op_item_remove_from_inventory(int paramc, void **paramv) {
    assert(paramc == 1);
    struct item *target_item = (struct item *)paramv[0];
    inventory_remove_item(target_item);
}

void op_item_unlink_from_owner(int paramc, void **paramv) {
    assert(paramc == 1);
    struct item *target_item = (struct item *)paramv[0];
    room_remove_item(target_item->owner, target_item);
    target_item->owner = NULL;
}

void op_item_clear_input_buffer(int paramc, void **paramv) {
    assert(paramc == 1);
    struct item *target_item = (struct item *)paramv[0];
    memset(target_item->input_buffer, 0, MAX_INPUT_BUFFER);
}

void op_global_state_set(int paramc, void **paramv) {
    assert(paramc == 2);
    int *target_gs = (int *)paramv[0];
    int value = *(int *)paramv[1];
    *target_gs = value;
}

void op_nop(int paramc, void **paramv) {
    // do nothing
}

void make_param(struct event *e, void *p1) {
    e->paramc = 1;
    e->paramv = malloc(sizeof(void *) * 1);
    e->paramv[0] = p1;
}

void make_params(struct event *e, void *p1, void *p2) {
    e->paramc = 2;
    e->paramv = malloc(sizeof(void *) * 2);
    e->paramv[0] = p1;
    e->paramv[1] = p2;
}

void event_init(struct event *cur_event) {
    cur_event->owner = NULL;
    cur_event->paramc = 0;
    cur_event->paramv = NULL;
    cur_event->op = op_nop;
}

struct event *construct_event(struct reaction *owner, const char *target1, const char *op, const char *target2) {
    struct event *cur_event = malloc(sizeof(struct event));
    event_init(cur_event);
    cur_event->owner = owner;

    printf("construct\n");
    struct room *room_target = name_to_room(target1);

    printf("construct 0x%08x\n", room_target);
    if (room_target != NULL) {
        printf("construct %c\n", *op);
        cur_event->type = E_OP_ON_ROOM;
        struct room *room_target2;
        struct item *item_target2;

        switch (*op) {
            case '>':
                room_target2 = name_to_room(target2);
                if (room_target2 != NULL) {
                    cur_event->sub_type.sub_type_on_room = E_ADD_LINK_TO_ROOM;
                    cur_event->op = op_room_add_link_to_room;
                    make_params(cur_event, (void *)room_target, (void *)room_target2);
                } else {
                    item_target2 = name_to_item(target2);
                    if (item_target2 != NULL) {
                        cur_event->sub_type.sub_type_on_room = E_ADD_LINK_TO_ITEM;
                        cur_event->op = op_room_add_link_to_item;
                        make_params(cur_event, (void *)room_target, (void *)item_target2);
                    }
                }
                break;

            case '+':
                printf("??\n");
                cur_event->sub_type.sub_type_on_room = E_CHANGE_ROOM_DESCRIPTION;
                cur_event->op = op_room_change_description;
                make_params(cur_event, (void *)room_target, (void *)target2);

                printf("???\n");
                break;

            case 'x':
                room_target2 = name_to_room(target2);
                if (room_target2 != NULL) {
                    cur_event->sub_type.sub_type_on_room = E_REMOVE_LINK_TO_ROOM;
                    cur_event->op = op_room_remove_link_to_room;
                    make_params(cur_event, (void *)room_target, (void *)room_target2);
                } else {
                    item_target2 = name_to_item(target2);
                    if (item_target2 != NULL) {
                        cur_event->sub_type.sub_type_on_room = E_REMOVE_LINK_TO_ITEM;
                        cur_event->op = op_room_remove_link_to_item;
                        make_params(cur_event, (void *)room_target, (void *)item_target2);
                    }
                }
                break;
        }
    } else {
        struct item *item_target = name_to_item(target1);
        if (item_target != NULL) {
            cur_event->type = E_OP_ON_ITEM;
            switch (*op) {
                case 'x':
                    if (strcmp(target2, "owner") == 0) {
                        cur_event->sub_type.sub_type_on_item = E_UNLINK_FROM_OWNER;
                        cur_event->op = op_item_unlink_from_owner;
                        make_param(cur_event, (void *)item_target);
                        break;
                    }
                    if (strcmp(target2, "input") == 0) {
                        cur_event->sub_type.sub_type_on_item = E_CLEAR_INPUT_BUFFER;
                        cur_event->op = op_item_clear_input_buffer;
                        make_param(cur_event, (void *)item_target);
                        break;
                    }
                case '+':
                    cur_event->sub_type.sub_type_on_item = E_CHANGE_ITEM_DESCRIPTION;
                    cur_event->op = op_item_change_description;
                    make_params(cur_event, (void *)item_target, (void *)target2);
                    break;
                case '>':
                    cur_event->sub_type.sub_type_on_item = E_ADD_TO_INVENTORY;
                    cur_event->op = op_item_add_to_inventory;
                    make_param(cur_event, (void *)item_target);
                    break;
                case '<':
                    cur_event->sub_type.sub_type_on_item = E_REMOVE_FROM_INVENTORY;
                    cur_event->op = op_item_remove_from_inventory;
                    make_param(cur_event, (void *)item_target);
                    break;
            }
        } else {
            int *gs = name_to_global_state(target1);
            if (gs && (*op == '=')) {
                cur_event->sub_type.sub_type_on_global_state = E_SET_STATE;
                cur_event->op = op_global_state_set;
                int *value = malloc(sizeof(int));
                sscanf(target2, "%d", value);
                make_params(cur_event, (void *)gs, (void *)value);
            }
            // TODO: global state perhaps
        }
    }

    printf("construct end\n");
    return cur_event;
}

void trigger_event(struct event *cur_event) {
    printf("triggering event: %d\n", cur_event->paramc);

    cur_event->op(cur_event->paramc, cur_event->paramv);
}