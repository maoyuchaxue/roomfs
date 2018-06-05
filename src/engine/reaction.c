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
    // TODO:
    return 1;
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
        switch (*op) {
            case '>':
                cur_pre->type = P_IN_DIRECTORY;
                cur_pre->checker = pre_check_in_inventory;
                make_pre_param(cur_pre, (void *)item_target);
                break;
            case '=':
                target_result = malloc(sizeof(char) * MAX_OBJ_NAME_LEN);
                strcpy(target_result, target2);
                cur_pre->type = P_INPUT_CHECK;
                cur_pre->checker = pre_check_input;
                make_pre_params(cur_pre, (void *)target1, (void *)target_result);
                break;
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
    printf("check prerequisite\n");
    return pre->checker(pre->paramc, pre->paramv);
}

struct reaction *construct_reaction(struct item *owner, struct prerequisite *pre) {
    struct reaction *cur_reaction = malloc(sizeof(struct reaction));
    cur_reaction->owner = owner;
    cur_reaction->pre = pre;
    cur_reaction->event_list_len = 0;
    cur_reaction->event_list = malloc(0);
    pre->owner = cur_reaction;
    return cur_reaction;
}

void reaction_add_event(struct reaction *parent, struct event *target_event) {
    parent->event_list_len++;
    parent->event_list = realloc(parent->event_list, parent->event_list_len * sizeof(struct event *));
    parent->event_list[parent->event_list_len - 1] = target_event;
}

int trigger_reaction(struct reaction *cur_reaction) {
    printf("trigger reaction!\n");
    if (check_prerequisite(cur_reaction->pre) > 0) {
        for (int i = 0; i < cur_reaction->event_list_len; i++) {
            printf("trigger event! %d\n", i);
            trigger_event(cur_reaction->event_list[i]); 
        }
        return 1;
    }
    return 0;
}