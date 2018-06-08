#include "eval.h"

struct eval *construct_eval_leaf_inner(const char *input, int *pos, int len) {
    struct eval *e = malloc(sizeof(struct eval));

    printf("construct eval leaf inner: %d\n", *pos);
    if (input[*pos] >= '0' && input[*pos] <= '9') { // const
        char tmp_buf[MAX_OBJ_NAME_LEN];
        int tmp_buf_ind = 0;
        while (((input[*pos] >= '0' && input[*pos] >= '9') || input[*pos] == '.') && *pos < len) {
            tmp_buf[tmp_buf_ind] = input[*pos];
            tmp_buf_ind++;
            (*pos)++;
        }
        tmp_buf[tmp_buf_ind] = '\0';
        float f;
        sscanf(tmp_buf, "%f", &f);
        
        e->type = EN_CONST;
        e->constant = f;

    } else { // variable

        char tmp_buf[MAX_OBJ_NAME_LEN];
        int tmp_buf_ind = 0;
        while ((input[*pos] != ' ' && input[*pos] != '+' && input[*pos] != '-' && input[*pos] != '*' && input[*pos] != '/' && input[*pos] != ')')
                && *pos < len) {
            tmp_buf[tmp_buf_ind] = input[*pos];
            tmp_buf_ind++;
            (*pos)++;
        }
        tmp_buf[tmp_buf_ind] = '\0';
        
        printf("tmp_buf: %s\n", tmp_buf);
        e->type = EN_GS;
        e->gs = name_to_global_state(tmp_buf);
    }

    return e;
}

struct eval *construct_eval_inner(const char *input, int *pos, int len) {
    struct eval *e = malloc(sizeof(struct eval));

    printf("construct eval inner: %d\n", *pos);
    while (input[*pos] == ' ' && (*pos) < len) (*pos)++;
    if (*pos == len) {
        return NULL;
    }

    struct eval *l_eval = NULL;
    if (input[*pos] == '(') { // (eval)
        (*pos)++;
        l_eval = construct_eval_inner(input, pos, len);
        if (!l_eval) {
            return NULL;
        }
        while (input[*pos] != ')' && (*pos) < len) (*pos)++;

        if (*pos == len) {
            return NULL;
        }

    } else {
        l_eval = construct_eval_leaf_inner(input, pos, len);
    }

    while (input[*pos] == ' ' && (*pos) < len) (*pos)++;    
    if (*pos == len) {
        free(e);
        return l_eval;
    }

    e->type = EN_OP;
    e->left_node = l_eval;
    switch (input[*pos]) {
        case '+':
            e->op = EV_ADD;
            break;
        case '-':
            e->op = EV_MINUS;
            break;
        case '*':
            e->op = EV_MULTIPLY;
            break;
        case '/':
            e->op = EV_DIVIDE;
            break;
        default:
            free(e);
            return l_eval;
    }
    printf("op char: %c\n", input[*pos]);
    (*pos)++;


    while (input[*pos] == ' ' && (*pos) < len) (*pos)++;
    if (*pos == len) {
        free(e);
        return l_eval;
    }

    struct eval *r_eval = NULL;
    if (input[*pos] == '(') { // (eval)
        (*pos)++;
        r_eval = construct_eval_inner(input, pos, len);
        if (!r_eval) {
            return NULL;
        }
        while (input[*pos] != ')' && (*pos) < len) (*pos)++;

        if (*pos == len) {
            return NULL;
        }

    } else {
        r_eval = construct_eval_leaf_inner(input, pos, len);
    }   

    e->right_node = r_eval;

    return e;
}

struct eval *construct_eval(const char *input) {
    int i = 0;
    return construct_eval_inner(input, &i, strlen(input));
}

float gen_result(struct eval *e) {
    float l, r;

    printf("gen result type: %d\n", e->type);
    switch (e->type) {
        case EN_CONST:
            return e->constant;
        case EN_GS:
            switch (e->gs->type) {
                case GS_FLOAT:
                    return e->gs->value.v_float;
                case GS_INT:
                    return (float)e->gs->value.v_int;
                default:
                    assert(0);    
            }
        case EN_OP:
            l = gen_result(e->left_node);
            printf("gen result l: %f\n", l);
            r = gen_result(e->right_node);
            printf("gen result r: %f\n", r);
            switch (e->op) {
                case EV_ADD:
                    return l+r;
                case EV_MINUS:
                    return l-r;
                case EV_MULTIPLY:
                    return l*r;
                case EV_DIVIDE:
                    return l/r;
            }
    }
}