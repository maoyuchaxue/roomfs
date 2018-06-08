

#ifndef _ROOMFS_DESCRIPTION
#define _ROOMFS_DESCRIPTION

#include "global_state.h"
#include <string.h>
#include <stdlib.h>
#include "const.h"

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

struct description * new_description();

void description_add_item(struct description *parent, struct description_item *des_item);
char *description_gen_text(struct description *des);


#endif