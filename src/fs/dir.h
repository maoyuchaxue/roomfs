#ifndef _ROOMFS_DIR
#define _ROOMFS_DIR

#define FUSE_USE_VERSION 31

#include <fuse_lowlevel.h>
#include "../engine/engine.h"
#include "file.h"
#include <stdlib.h>
#include <string.h>

struct dirbuf {
    char *p;
    size_t size;
};

struct room *ino_to_room(fuse_ino_t ino);
int is_dir(fuse_ino_t ino);
int is_dir_description(fuse_ino_t ino);
struct dirbuf *read_dir(fuse_req_t req, fuse_ino_t ino);
void dir_getattr(fuse_ino_t ino, struct stat *stbuf);
void dir_lookup(fuse_ino_t parent, const char *name, struct fuse_entry_param *e);

void dir_description_read(fuse_req_t req, fuse_ino_t ino, size_t size,
	off_t off, struct fuse_file_info *fi);
#endif