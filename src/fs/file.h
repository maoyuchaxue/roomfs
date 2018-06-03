#ifndef _ROOMFS_FILE
#define _ROOMFS_FILE

#define FUSE_USE_VERSION 31

#include <fuse_lowlevel.h>
#include "../engine/engine.h"
#include <stdlib.h>
#include <string.h>


struct item *ino_to_item(fuse_ino_t ino);
int is_file(fuse_ino_t ino);

void file_getattr(fuse_ino_t ino, struct stat *stbuf);

#endif