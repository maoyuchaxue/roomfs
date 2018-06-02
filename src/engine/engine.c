#include "engine.h"

struct game_engine_s *game_engine;


void read_room_setting(FILE *f, struct room *cur_room) {

    int adj_rooms = 0;
    fscanf(f, "%d\n", &adj_rooms);
    printf("orig: %d\n", adj_rooms);

    cur_room->adjacent_rooms = (struct room **) malloc(sizeof(struct room *) * adj_rooms);
    char target_room_name[100];
    struct room *target_room = NULL;

    for (int i = 0; i < adj_rooms; i++) {
        fscanf(f, "%s\n", target_room_name);

        for (int j = 0; j < game_engine->total_rooms; j++) {
            if (strcmp(game_engine->room_names[j], target_room_name) == 0) {
                target_room = game_engine->rooms[j];
                break;
            }
        }
        cur_room->adjacent_rooms[i] = target_room;
        printf("orig: 0x%08x, 0x%08x\n", cur_room, target_room);
        printf("orig: %s\n", target_room_name);
    }

    cur_room->total_adjacent_rooms = adj_rooms;
    printf("total: %d\n", cur_room->total_adjacent_rooms);
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
        room_names[i] = room_name_cpy;
    }

    game_engine->rooms = rooms;
    game_engine->room_names = room_names;
    game_engine->total_rooms = room_nums;
    game_engine->entry_room = rooms[0];


    char obj_name[100];
    while (fscanf(f, "%s\n", obj_name) == 1) {
        struct room *cur_room = NULL;
        for (int i = 0; i < room_nums; i++) {
            if (strcmp(room_names[i], room_name) == 0) {
                cur_room = rooms[i];
                break;
            }
        }

        read_room_setting(f, cur_room);
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