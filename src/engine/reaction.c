#include "reaction.h"


int pre_check_global_state(int paramc, void **paramv) {
    assert(paramc == 2);
    int *gs = (int *)paramv[0];
    int *value = (int *)paramv[1];
    printf("check global: %d, %d\n", *gs, *value);
    return (gs && value && *gs == *value) ? 1 : 0;
}

int pre_check_in_inventory(int paramc, void **paramv) {
    // TODO:
    return 1;
}

int pre_check_input(int paramc, void **paramv) {
    printf("prerequisite checker\n");
    assert(paramc == 2);
    struct item *cur_item = (struct item *)paramv[0];
    char *target_result = (char *)paramv[1];
    
    printf("compare input: %s, %s\n", cur_item->input_buffer, target_result);
    return (strcmp(cur_item->input_buffer, target_result) == 0) ? 1 : 0;
}

int pre_check_input_item(int paramc, void **paramv) {
    assert(paramc == 2);
    struct item *cur_item = (struct item *)paramv[0];
    struct item *target_item = (struct item *)paramv[1];

    unsigned long long cur_flag = 0;
    sscanf(cur_item->input_buffer, "%llu\n", &cur_flag);

    if (target_item->inventory_flag == 0) {
        return 0;
    }

    return (cur_flag == target_item->inventory_flag);
}

int pre_nop(int paramc, void **paramv) {
    return 1;
}


void make_pre_param(struct prerequisite *p, void *p1) {
    p->paramc = 1;
    p->paramv = malloc(sizeof(void *) * 1);
    p->paramv[0] = p1;
}

void make_pre_params(struct prerequisite *p, void *p1, void *p2) {
    p->paramc = 2;
    p->paramv = malloc(sizeof(void *) * 2);
    p->paramv[0] = p1;
    p->paramv[1] = p2;
}

struct prerequisite *construct_pre(const char *target1, const char *op, const char *target2) {
    struct prerequisite *cur_pre = malloc(sizeof(struct prerequisite));
    pre_init(cur_pre);

    if (strlen(target1) == 0) {
        cur_pre->type = P_NO_CHECK;
        cur_pre->checker = pre_nop;
    }

    struct item *item_target = name_to_item(target1);
    if (item_target) {
        char *target_result = NULL;
        struct item *item_target2 = NULL;
        switch (*op) {
            case '>':
                cur_pre->type = P_IN_DIRECTORY;
                cur_pre->checker = pre_check_in_inventory;
                make_pre_param(cur_pre, (void *)item_target);
                break;
            case '=':
                target_result = malloc(sizeof(char) * MAX_INPUT_BUFFER);
                strcpy(target_result, target2);
                cur_pre->type = P_INPUT_CHECK;
                cur_pre->checker = pre_check_input;
                make_pre_params(cur_pre, (void *)item_target, (void *)target_result);
                break;
            case '<':
                item_target2 = name_to_item(target2);
                if (item_target2 != NULL) {
                    cur_pre->type = P_INPUT_ITEM_CHECK;
                    cur_pre->checker = pre_check_input_item;
                    make_pre_params(cur_pre, (void *)item_target, (void *)item_target2);
                    break;
                }

        }
    } else {
        int *gs_target = name_to_global_state(target1);
        int *target_value = malloc(sizeof(int));
        sscanf(target2, "%d", target_value);

        if (gs_target) {
            switch (*op) {
                case '=':
                    cur_pre->type = P_GLOBAL_STATE;
                    cur_pre->checker = pre_check_global_state;
                    make_pre_params(cur_pre, (void *)gs_target, (void *)target_value);
                    break;
            }
        }
    }

    return cur_pre;
}

void pre_init(struct prerequisite *pre) {
    pre->owner = NULL;
    pre->paramc = 0;
    pre->paramv = NULL;
    pre->checker = pre_nop;
}

int check_prerequisite(struct prerequisite *pre) {

    return pre->checker(pre->paramc, pre->paramv);
}

int check_prerequisites(struct reaction *cur_reaction) {
    for (int i = 0; i < cur_reaction->pre_list_len; i++) {
        printf("check_pre: %d\n", i);
        int ret = check_prerequisite(cur_reaction->pre_list[i]);
        if (ret <= 0) {
            return ret;
        }
    }
    return 1;
}

struct reaction *construct_reaction(struct item *owner) {
    struct reaction *cur_reaction = malloc(sizeof(struct reaction));
    cur_reaction->owner = owner;
    cur_reaction->event_list_len = 0;
    cur_reaction->event_list = malloc(0);
    cur_reaction->pre_list_len = 0;
    cur_reaction->pre_list = malloc(0);
    return cur_reaction;
}

void reaction_add_event(struct reaction *parent, struct event *target_event) {
    parent->event_list_len++;
    parent->event_list = realloc(parent->event_list, parent->event_list_len * sizeof(struct event *));
    parent->event_list[parent->event_list_len - 1] = target_event;
}

void reaction_add_pre(struct reaction *parent, struct prerequisite *pre) {
    parent->pre_list_len++;
    parent->pre_list = realloc(parent->pre_list, parent->pre_list_len * sizeof(struct prerequisite *));
    parent->pre_list[parent->pre_list_len - 1] = pre;
    pre->owner = parent;
}

int trigger_reaction(struct reaction *cur_reaction) {
    printf("trigger reaction!\n");

    if (check_prerequisites(cur_reaction) > 0) {
        for (int i = 0; i < cur_reaction->event_list_len; i++) {
            printf("trigger event! %d\n", i);
            trigger_event(cur_reaction->event_list[i]); 
        }
        return 1;
    }
    return 0;
}