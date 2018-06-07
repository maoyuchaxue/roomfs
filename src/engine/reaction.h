#ifndef _ROOMFS_REACTION
#define _ROOMFS_REACTION

#include "room.h"
#include "event.h"
#include "global_state.h"

struct room;
struct item;
struct event;

enum prerequisite_type {
    P_IN_DIRECTORY,
    P_GLOBAL_STATE,
    P_NO_CHECK,
    P_INPUT_CHECK,
    P_INPUT_ITEM_CHECK,
};

typedef int (*pre_checker)(int , void **);

struct prerequisite {
    enum prerequisite_type type;
    pre_checker checker;
    int paramc;
    void **paramv; 
    struct reaction *owner;
};

struct reaction {
    struct item *owner;
    struct prerequisite **pre_list;
    int pre_list_len;
    struct event **event_list;
    int event_list_len;
};

struct prerequisite *construct_pre(const char *target1, const char *op, const char *target2);
void pre_init(struct prerequisite *pre);
int check_prerequisite(struct prerequisite *pre);

struct reaction *construct_reaction(struct item *owner);
void reaction_add_pre(struct reaction *parent, struct prerequisite *pre);
void reaction_add_event(struct reaction *parent, struct event *target_event);
int check_prerequisites(struct reaction *cur_reaction);
int trigger_reaction(struct reaction *cur_reaction);

#endif