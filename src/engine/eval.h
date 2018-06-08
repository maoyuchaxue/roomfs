

#ifndef _ROOMFS_EVAL
#define _ROOMFS_EVAL

#include "global_state.h"
#include <string.h>
#include <stdlib.h>

enum eval_node_type {
    EN_OP, // a op b
    EN_CONST, // constant
    EN_GS, // variable
};

enum eval_op {
    EV_ADD,
    EV_MINUS,
    EV_MULTIPLY,
    EV_DIVIDE,
};

struct eval {
    enum eval_node_type type;
    enum eval_op op;    
    struct eval *left_node;
    struct eval *right_node;
    float constant;
    struct global_state *gs;
};

struct eval *construct_eval_inner(const char *input, int *pos, int len);

struct eval *construct_eval(const char *input);

float gen_result(struct eval *e);


#endif