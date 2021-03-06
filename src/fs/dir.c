#include "dir.h"

extern struct game_engine_s *game_engine; 
extern struct inventory inv;

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

struct room *ino_to_description(fuse_ino_t ino) {

    for (int i = 0; i < game_engine->total_rooms; i++) {
        if ((fuse_ino_t)&(game_engine->rooms[i]->description) == ino) {
            return game_engine->rooms[i];
        }
    }

    return NULL;
}



int is_dir(fuse_ino_t ino) {
    return (ino_to_room(ino) != NULL);
}

int is_dir_description(fuse_ino_t ino) {
    return (ino_to_description(ino) != NULL);
}


void dirbuf_add(fuse_req_t req, struct dirbuf *b, const char *name,
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
    dirbuf_add(req, b, "look", (fuse_ino_t)&(cur_room->description));
    for (int i = 0; i < cur_room->total_adjacent_rooms; i++) {
        printf("0x%08x,\n", cur_room->adjacent_rooms[i]);
        dirbuf_add(req, b, cur_room->adjacent_rooms[i]->name, (fuse_ino_t)(cur_room->adjacent_rooms[i]));
    }

    for (int i = 0; i < cur_room->total_items; i++) {
        printf("0x%08x,\n", cur_room->items[i]);
        dirbuf_add(req, b, cur_room->items[i]->name, (fuse_ino_t)(cur_room->items[i]));
    }
    dirbuf_add(req, b, "inventory", (fuse_ino_t)(&inv));

    return b;
}


void dir_description_getattr(fuse_ino_t ino, struct stat *stbuf) {
    struct room *cur_room = ino_to_description(ino);

    const char *dir_look_prefix = "if [ -n \"$1\" ]; then \n `bash $1` \n else \n";
    const char *dir_look_suffix = "\n fi \n";

    stbuf->st_mode = S_IFREG | 0777;
    stbuf->st_nlink = 1;
    stbuf->st_ino = (fuse_ino_t)&(cur_room->description);

    char *des_text = description_gen_text(cur_room->description);
    des_text = wrap_sh_echo(des_text);
	stbuf->st_size = strlen(des_text) + strlen(dir_look_prefix) + strlen(dir_look_suffix);
    free(des_text);
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

    const char *dir_description = "look";
    if (strcmp(name, dir_description) == 0) {
        e->ino = (fuse_ino_t)&(parent_room->description);
        e->attr_timeout = 1.0;
        e->entry_timeout = 1.0;
        dir_description_getattr(e->ino, &(e->attr));
        printf("lookup_dir found(look): 0x%08x\n", e->ino);
        return;
    }

    const char *inventory_dir = "inventory";
    if (strcmp(name, inventory_dir) == 0) {
        e->ino = (fuse_ino_t)&inv;
        e->attr_timeout = 1.0;
        e->entry_timeout = 1.0;
        inventory_dir_getattr(e->ino, &(e->attr));
        printf("lookup_dir found(look): 0x%08x\n", e->ino);
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

    for (int i = 0; i < parent_room->total_items; i++) {
        struct item *cur_item = parent_room->items[i];
        if (strcmp(name, cur_item->name) == 0) {
         	e->ino = (fuse_ino_t)cur_item;
            e->attr_timeout = 1.0;
            e->entry_timeout = 1.0;
            file_getattr(e->ino, &(e->attr));
            printf("lookup_dir found: 0x%08x\n", e->ino);
            return;
        }
    }
}   


void dir_description_read(fuse_req_t req, fuse_ino_t ino, size_t size,
	off_t off, struct fuse_file_info *fi) {
    
    printf("file read: %d %d\n", size, off);
    struct room *cur_room = ino_to_description(ino);

    const char *dir_look_prefix = "if [ -n \"$1\" ]; then \n bash $1 \n else \n";
    const char *dir_look_suffix = "\n fi \n";
    char *dir_description = malloc(MAX_DESCRIPTION_LEN * 2 * sizeof(char));
    strcpy(dir_description, dir_look_prefix);
    char *des_text = description_gen_text(cur_room->description);
    des_text = wrap_sh_echo(des_text);
    dir_description = strcat(dir_description, des_text);
    dir_description = strcat(dir_description, dir_look_suffix);

	reply_buf_limited(req, dir_description, strlen(dir_description), off, size);
    free(dir_description);
    free(des_text);
}