#include "engine.h"

struct game_engine_s *game_engine;

char *wrap_sh_echo(char *buf) {
    int buf_len = strlen(buf);
    char *wrapped = malloc((buf_len + 10));

    char *prefix = "echo \"";
    int prefix_len = strlen(prefix);
    char *suffix = "\"";
    int suffix_len = strlen(suffix);

    strcpy(wrapped, prefix);
    strcpy(wrapped + prefix_len, buf);
    strcpy(wrapped + prefix_len + buf_len, suffix);

    free(buf);
    return wrapped;
}

char *read_multiline_description(FILE *f) {

    char *buf = malloc(MAX_DESCRIPTION_LEN * sizeof(char));
    char c;
    int i = 0;
    while (1) {
        c = fgetc(f);
        if (c == '\n') {
            if (i > 0 && buf[i-1] == '\\') {
                i--;
            } else {
                buf[i] = c;
                i++;
                buf[i] = '\0';
                break;
            }
        }
        buf[i] = c;
        i++;
    }

    return wrap_sh_echo(buf);
}

void read_room_setting(FILE *f, struct room *cur_room) {

    cur_room->description = read_multiline_description(f);
    printf("orig: %s\n", cur_room->description);

    int related_objs = 0;
    fscanf(f, "%d\n", &related_objs);
    printf("orig: %d\n", related_objs);

    char target_obj_name[MAX_OBJ_NAME_LEN];
    struct room *target_room = NULL;
    struct item *target_item = NULL;

    for (int i = 0; i < related_objs; i++) {
        fscanf(f, "%s\n", target_obj_name);

        printf("orig: %s\n", target_obj_name);
        int found = 0;

        for (int j = 0; j < game_engine->total_rooms; j++) {
            if (strcmp(game_engine->room_names[j], target_obj_name) == 0) {
                target_room = game_engine->rooms[j];
                room_add_adj_room(cur_room, target_room);
                printf("orig: 0x%08x, 0x%08x\n", cur_room, target_room);
                found = 1;
                break;
            }
        }

        if (found) {
            continue;
        }

        for (int j = 0; j < game_engine->total_items; j++) {
            if (strcmp(game_engine->item_names[j], target_obj_name) == 0) {
                target_item = game_engine->items[j];
                room_add_item(cur_room, target_item);
                printf("orig: 0x%08x, 0x%08x\n", cur_room, target_item);
                break;
            }
        }
        
    }

    printf("total: %d\n", cur_room->total_adjacent_rooms);
}

struct reaction *read_prerequisite_setting(FILE *f, struct item *cur_item) {
    struct prerequisite *pre = NULL;

    char target1[MAX_OBJ_NAME_LEN];
    fscanf(f, "%s", target1);
    printf("%s\n", target1);
    if (strcmp(target1, "begin") == 0) {
        pre = construct_pre("", "", "");
    } else {
        char op_char;
        fscanf(f, " %c", &op_char);
        char *target2 = malloc(sizeof(char) * MAX_OBJ_NAME_LEN);
        fscanf(f, " %s\n", target2);
        pre = construct_pre(target1, &op_char, target2);

        fscanf(f, "%s", target1);
    }

    struct reaction *cur_reaction = construct_reaction(cur_item, pre);
    item_add_reaction(cur_item, cur_reaction);
    return cur_reaction;
}



void read_reaction_setting(FILE *f, struct item *cur_item) {
    struct reaction *cur_reaction = read_prerequisite_setting(f, cur_item);

    struct event *cur_event;

    while (1) {
        char target1[MAX_OBJ_NAME_LEN];
        fscanf(f, "%s", target1);
        printf("%s\n", target1);
        if (strcmp(target1, "end") == 0) {
            break;
        }

        char op_char;
        fscanf(f, " %c", &op_char);

        char *target2 = malloc(sizeof(char) * MAX_OBJ_NAME_LEN);
        fscanf(f, " %s\n", target2);


        if (strcmp(target2, "description") == 0) {
            target2 = read_multiline_description(f);
        } 

        cur_event = construct_event(cur_reaction, target1, &op_char, target2);

        reaction_add_event(cur_reaction, cur_event);
    }
}

void read_item_setting(FILE *f, struct item *cur_item) {
    cur_item->description = read_multiline_description(f);
    printf("itm: %s\n", cur_item->description);

    while (1) {
        char operation[MAX_OBJ_NAME_LEN];
        fscanf(f, "%s", operation);
        if (strcmp(operation, "end") == 0) {
            break;
        } else {
            assert(strcmp(operation, "if") == 0);
            read_reaction_setting(f, cur_item);
        }
    }
}

void engine_init(const char *path) {
    inventory_init();

    game_engine = malloc(sizeof(struct game_engine_s));

    FILE *f = fopen(path, "r");
    int room_nums = 0;
    fscanf(f, "rooms: %d\n", &room_nums);
    
    char **room_names = (char **) malloc(sizeof(char *) * room_nums);
    struct room **rooms = (struct room **) malloc(sizeof(struct room *) * room_nums);

    char room_name[MAX_OBJ_NAME_LEN];
    for (int i = 0; i < room_nums; i++) {
        fscanf(f, "%s\n", room_name);

        rooms[i] = (struct room *) malloc(sizeof(struct room));

        printf("%d: 0x%08x %s\n", i, rooms[i], room_name);
        char *room_name_cpy = malloc(sizeof(char ) * (strlen(room_name) + 1));
        strcpy(room_name_cpy, room_name);

        room_init(rooms[i]);
        rooms[i]->name = room_name_cpy;
        room_names[i] = room_name_cpy;
    }

    game_engine->rooms = rooms;
    game_engine->room_names = room_names;
    game_engine->total_rooms = room_nums;
    game_engine->entry_room = rooms[0];

    int item_nums = 0;
    fscanf(f, "items: %d\n", &item_nums);
    
    char **item_names = (char **) malloc(sizeof(char *) * item_nums);
    struct item **items = (struct item **) malloc(sizeof(struct item *) * item_nums);

    char item_name[MAX_OBJ_NAME_LEN];
    for (int i = 0; i < item_nums; i++) {
        fscanf(f, "%s\n", item_name);

        items[i] = (struct item *) malloc(sizeof(struct item));

        printf("%d: 0x%08x %s\n", i, items[i], item_name);
        char *item_name_cpy = malloc(sizeof(char ) * (strlen(item_name) + 1));
        strcpy(item_name_cpy, item_name);

        item_init(items[i]);
        items[i]->name = item_name_cpy;
        item_names[i] = item_name_cpy;
    }

    game_engine->items = items;
    game_engine->item_names = item_names;
    game_engine->total_items = item_nums;

    int global_nums = 0;
    fscanf(f, "globals: %d\n", &global_nums);
    
    char **global_names = (char **) malloc(sizeof(char *) * global_nums);
    int *globals = (int *) malloc(sizeof(int) * global_nums);

    char global_name[MAX_OBJ_NAME_LEN];
    int init_value;
    for (int i = 0; i < global_nums; i++) {
        fscanf(f, "%s %d\n", global_name, &init_value);
        globals[i] = init_value;

        printf("%s, %d\n", global_name, init_value);
        char *global_name_cpy = malloc(sizeof(char) * (strlen(global_name) + 1));
        strcpy(global_name_cpy, global_name);

        global_names[i] = global_name_cpy;
    }

    game_engine->global_states = globals;
    game_engine->global_state_names = global_names;
    game_engine->total_global_states = global_nums;


    char obj_name[MAX_OBJ_NAME_LEN];
    while (fscanf(f, "%s\n", obj_name) == 1) {
        printf("obj name: %s\n", obj_name);
        struct room *cur_room = NULL;
        for (int i = 0; i < room_nums; i++) {
            if (strcmp(room_names[i], obj_name) == 0) {
                cur_room = rooms[i];
                read_room_setting(f, cur_room);
                break;
            }
        }

        if (cur_room != NULL) {
            continue;
        }

        struct item *cur_item = NULL;
        for (int i = 0; i < item_nums; i++) {
            if (strcmp(item_names[i], obj_name) == 0) {
                cur_item = items[i];
                read_item_setting(f, cur_item);
                break;
            }
        }

    }
}


struct room *name_to_room(const char *room_name) {
    for (int i = 0; i < game_engine->total_rooms; i++) {
        if (strcmp(room_name, game_engine->room_names[i]) == 0) {
            return game_engine->rooms[i];
        }
    }
    return NULL;
}


struct item *name_to_item(const char *item_name) {
    for (int i = 0; i < game_engine->total_items; i++) {
        if (strcmp(item_name, game_engine->item_names[i]) == 0) {
            return game_engine->items[i];
        } 
    }
    return NULL;
}

void engine_destroy() {
    for (int i = 0; i < game_engine->total_rooms; i++) {
        free(game_engine->rooms[i]);
    }
    free(game_engine->rooms);
    free(game_engine->room_names);
    free(game_engine);
}