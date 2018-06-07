#include "engine.h"


extern struct game_engine_s *game_engine; 

struct global_state *name_to_global_state(const char *name) {
    for (int i = 0; i < game_engine->total_global_states; i++) {
        if (strcmp(name, game_engine->global_state_names[i]) == 0) {
            return game_engine->global_states[i];
        }
    }
    return NULL;
}

void* get_global_state(struct global_state *gs) {
    switch (gs->type) {
        case GS_INT:
            return (void *)&(gs->value.v_int);
        case GS_STRING:
            return (void *)&(gs->value.v_str);
        case GS_FLOAT:
            return (void *)&(gs->value.v_float);
        default: // won't happen
            assert(0);
            return NULL;
    }
}

void set_global_state(struct global_state *gs, void *value_p) {
    if (gs) {
        switch (gs->type) {
            case GS_INT:
                gs->value.v_int = *(int *)value_p;
                break;
            case GS_STRING:
                gs->value.v_str = *(char **)value_p;
                break;
            case GS_FLOAT:
                gs->value.v_float = *(float *)value_p;
                break;
            default: // won't happen
                assert(0);
        }
    }
}

int float_cmp(float f1, float f2) {
    return (f1 - f2 <= 0.000001 && f1 - f2 >= -0.000001);
}

int check_global_state(struct global_state *gs, void *value_p, enum global_state_cmp_op op) {
    printf("check_global_state %d\n", op);
    if (gs) {
        switch (gs->type) {
            case GS_INT:
                printf("check: %d, %d\n", gs->value.v_int, *(int *)value_p);
                switch (op) {
                    case GS_GREATER:
                        return (gs->value.v_int > *(int *)value_p);
                    case GS_EQUAL:
                        return (gs->value.v_int == *(int *)value_p);
                    case GS_LESS:
                        return (gs->value.v_int < *(int *)value_p);
                    case GS_GE:
                        return (gs->value.v_int >= *(int *)value_p);
                    case GS_LE:
                        return (gs->value.v_int <= *(int *)value_p);
                    case GS_NEQUAL:
                        return (gs->value.v_int != *(int *)value_p);
                }
                
            case GS_STRING:
                printf("check: %s, %s\n", gs->value.v_str, *(char **)value_p);
                switch (op) {
                    case GS_EQUAL:
                        return (strcmp(gs->value.v_str, *(char **)value_p) == 0);
                    case GS_NEQUAL:
                        return (strcmp(gs->value.v_str, *(char **)value_p) != 0);
                    default:
                        assert(0);
                }

            case GS_FLOAT:
                printf("check: %f, %f\n", gs->value.v_float, *(char *)value_p);
                switch (op) {
                    case GS_GREATER:
                        return (gs->value.v_float > *(float *)value_p + 0.000001);
                    case GS_EQUAL:
                        return float_cmp(gs->value.v_float, *(float *)value_p);
                    case GS_LESS:
                        return (gs->value.v_float < *(float *)value_p - 0.000001);
                    case GS_GE:
                        return (gs->value.v_float > *(float *)value_p - 0.000001);
                    case GS_LE:
                        return (gs->value.v_float < *(float *)value_p + 0.000001);
                    case GS_NEQUAL:
                        return (!float_cmp(gs->value.v_float, *(float *)value_p));
                }
            default: // won't happen
                assert(0);
        }
    } else {
        return 0;
    }
}
