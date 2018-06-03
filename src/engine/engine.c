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

    char *buf = malloc(1000 * sizeof(char));
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

    cur_room->adjacent_rooms = (struct room **) malloc(sizeof(struct room *) * related_objs);
    char target_obj_name[100];
    struct room *target_room = NULL;
    struct item *target_item = NULL;

    cur_room->total_adjacent_rooms = 0;
    cur_room->total_items = 0;

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

void read_item_setting(FILE *f, struct item *cur_item) {

    cur_item->description = read_multiline_description(f);
    printf("itm: %s\n", cur_item->description);
}

void engine_init(const char *path) {
    game_engine = malloc(sizeof(struct game_engine_s));

    FILE *f = fopen(path, "r");
    int room_nums = 0;
    fscanf(f, "rooms: %d\n", &room_nums);
    
    char **room_names = (char **) malloc(sizeof(char *) * room_nums);
    struct room **rooms = (struct room **) malloc(sizeof(struct room *) * room_nums);

    char room_name[100];
    for (int i = 0; i < room_nums; i++) {
        fscanf(f, "%s\n", room_name);

        rooms[i] = (struct room *) malloc(sizeof(struct room));

        printf("%d: 0x%08x %s\n", i, rooms[i], room_name);
        char *room_name_cpy = malloc(sizeof(char ) * (strlen(room_name) + 1));
        strcpy(room_name_cpy, room_name);

        rooms[i]->name = room_name_cpy;
        rooms[i]->description = "";
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

    char item_name[100];
    for (int i = 0; i < item_nums; i++) {
        fscanf(f, "%s\n", item_name);

        items[i] = (struct item *) malloc(sizeof(struct item));

        printf("%d: 0x%08x %s\n", i, items[i], item_name);
        char *item_name_cpy = malloc(sizeof(char ) * (strlen(item_name) + 1));
        strcpy(item_name_cpy, item_name);

        items[i]->name = item_name_cpy;
        item_names[i] = item_name_cpy;
    }

    game_engine->items = items;
    game_engine->item_names = item_names;
    game_engine->total_items = item_nums;


    char obj_name[100];
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

void engine_destroy() {
    for (int i = 0; i < game_engine->total_rooms; i++) {
        free(game_engine->rooms[i]);
    }
    free(game_engine->rooms);
    free(game_engine->room_names);
    free(game_engine);
}