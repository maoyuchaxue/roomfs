#ifndef _ROOMFS_ROOM
#define _ROOMFS_ROOM


struct room {
    char *name;
    struct room **adjacent_rooms;
    int total_adjacent_rooms;
};

// void engine_init(const char *path);

#endif