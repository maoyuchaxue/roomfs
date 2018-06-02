#include "dir.h"

extern struct game_engine_s *game_engine; 

struct room *ino_to_room(fuse_ino_t ino) {
    if (ino == FUSE_ROOT_ID) {
        return game_engine->entry_room;
    }

    for (int i = 0; i < game_engine->total_rooms; i++) {
        if ((fuse_ino_t)game_engine->rooms[i] == ino) {
            return game_engine->rooms[i];
        }
    }
    return NULL;
}

int is_dir(fuse_ino_t ino) {
    return (ino_to_room(ino) != NULL);
}


static void dirbuf_add(fuse_req_t req, struct dirbuf *b, const char *name,
                       fuse_ino_t ino) {
    struct stat stbuf;
    size_t oldsize = b->size;
    b->size += fuse_add_direntry(req, NULL, 0, name, NULL, 0);
    b->p = (char *) realloc(b->p, b->size);
    memset(&stbuf, 0, sizeof(stbuf));
    stbuf.st_ino = ino;
    fuse_add_direntry(req, b->p + oldsize, b->size - oldsize, name, &stbuf,
                      b->size);
}

struct dirbuf *read_dir(fuse_req_t req, fuse_ino_t ino) {
    
    struct room *cur_room = ino_to_room(ino);

    printf("read_dir: 0x%08x, %d: ", cur_room, cur_room->total_adjacent_rooms);
    struct dirbuf *b = malloc(sizeof(struct dirbuf));
    memset(b, 0, sizeof(struct dirbuf));

    dirbuf_add(req, b, ".", (fuse_ino_t)cur_room);
    for (int i = 0; i < cur_room->total_adjacent_rooms; i++) {
        printf("0x%08x,\n", cur_room->adjacent_rooms[i]);
        dirbuf_add(req, b, cur_room->adjacent_rooms[i]->name, (fuse_ino_t)(cur_room->adjacent_rooms[i]));
    }

    return b;
}

void dir_getattr(fuse_ino_t ino, struct stat *stbuf) {
    struct room *cur_room = ino_to_room(ino);
    stbuf->st_mode = S_IFDIR | 0777;
    stbuf->st_nlink = 1;
    stbuf->st_ino = (fuse_ino_t)cur_room;
}


void dir_lookup(fuse_ino_t parent, const char *name, struct fuse_entry_param *e) {

    struct room *parent_room = ino_to_room(parent);
    printf("lookup_dir: 0x%08x, %s: \n", parent_room, name);

    const char *cur_directory = ".";
    if (strcmp(name, cur_directory) == 0) {
        e->ino = (fuse_ino_t)parent_room;
        e->attr_timeout = 1.0;
        e->entry_timeout = 1.0;
        dir_getattr(e->ino, &(e->attr));
        printf("lookup_dir found: 0x%08x\n", e->ino);
        return;
    }

    for (int i = 0; i < parent_room->total_adjacent_rooms; i++) {
        struct room *cur_room = parent_room->adjacent_rooms[i];
        if (strcmp(name, cur_room->name) == 0) {
         	e->ino = (fuse_ino_t)cur_room;
            e->attr_timeout = 1.0;
            e->entry_timeout = 1.0;
            dir_getattr(e->ino, &(e->attr));
            printf("lookup_dir found: 0x%08x\n", e->ino);
            return;
        }
    }

}