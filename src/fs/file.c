#include "file.h"

extern struct game_engine_s *game_engine; 

struct item *ino_to_item(fuse_ino_t ino) {

    for (int i = 0; i < game_engine->total_items; i++) {
        if ((fuse_ino_t)game_engine->items[i] == ino) {
            return game_engine->items[i];
        }
    }
    return NULL;
}

int is_file(fuse_ino_t ino) {
    return (ino_to_item(ino) != NULL);
}


void file_getattr(fuse_ino_t ino, struct stat *stbuf) {
    struct room *cur_item = ino_to_item(ino);
    stbuf->st_mode = S_IFREG | 0777;
    stbuf->st_nlink = 1;
    stbuf->st_ino = (fuse_ino_t)cur_item;
}