#ifndef _ROOMFS_GLOBAL_STATE
#define _ROOMFS_GLOBAL_STATE

#include "engine.h"
#include <assert.h>

enum global_state_type {
    GS_INT, GS_STRING, GS_FLOAT
};

enum global_state_cmp_op {
    GS_GREATER, GS_EQUAL, GS_LESS, GS_GE, GS_LE, GS_NEQUAL
};

union gs_value {
    int v_int;
    char *v_str;
    float v_float;
};

struct global_state {
    enum global_state_type type;
    union gs_value value;
};

struct global_state *name_to_global_state(const char *name);

void* get_global_state(struct global_state *gs);
void set_global_state(struct global_state *gs, void *value_p);
int check_global_state(struct global_state *gs, void *value_p, enum global_state_cmp_op op); // returns 1 if global_state[name] op value


char *global_state_to_text(struct global_state *gs, const char *fmt);

#endif