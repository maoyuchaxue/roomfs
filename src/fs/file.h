#ifndef _ROOMFS_FILE
#define _ROOMFS_FILE

#define FUSE_USE_VERSION 31

#include <fuse_lowlevel.h>
#include "../engine/engine.h"
#include <stdlib.h>
#include <string.h>

#define min(x, y) ((x) < (y) ? (x) : (y))

struct item *ino_to_item(fuse_ino_t ino);
int is_file(fuse_ino_t ino);

void file_getattr(fuse_ino_t ino, struct stat *stbuf);

void file_read(fuse_req_t req, fuse_ino_t ino, size_t size,
		off_t off, struct fuse_file_info *fi);

void file_close(fuse_ino_t ino);

int reply_buf_limited(fuse_req_t req, const char *buf, size_t bufsize,
        off_t off, size_t maxsize);

#endif