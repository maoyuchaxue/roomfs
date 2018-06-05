#ifndef _ROOMFS_GLOBAL_STATE
#define _ROOMFS_GLOBAL_STATE

#include "engine.h"

int *name_to_global_state(const char *name);

int get_global_state(const char *name);
void set_global_state(const char *name, int value);

int check_global_state(const char *name, int value); // returns 1 if global_state[name] == value

#endif