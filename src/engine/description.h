

#ifndef _ROOMFS_ENGINE
#define _ROOMFS_ENGINE

#include "global_state.h"
#include <string.h>
#include <stdlib.h>

enum description_item_type {
    D_STRING, D_GS
};

struct description_item {
    enum description_item_type type;
    char *text;
    struct global_state *related_gs;
};

struct description {
    struct item *owner;
    struct description_item **item_list;
    int item_list_len;
};

// void description_init();


#endif