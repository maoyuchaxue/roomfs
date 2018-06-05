#include "engine.h"


extern struct game_engine_s *game_engine; 

int *name_to_global_state(const char *name) {
    for (int i = 0; i < game_engine->total_global_states; i++) {
        if (strcmp(name, game_engine->global_state_names[i]) == 0) {
            return &(game_engine->global_states[i]);
        }
    }
    return NULL;
}

int get_global_state(const char *name) {
    int *gs = name_to_global_state(name);

    return (gs == NULL) ? -1 : *gs;
}

void set_global_state(const char *name, int value) {
    int *gs = name_to_global_state(name);
    if (gs) {
        *gs = value;
    }
}

int check_global_state(const char *name, int value) {
    int *gs = name_to_global_state(name);
    if (gs && *gs == value) {
        return 1;
    } else {
        return 0;
    }
}
