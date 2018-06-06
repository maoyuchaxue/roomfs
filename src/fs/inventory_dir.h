#ifndef _ROOMFS_INVENTORY_DIR
#define _ROOMFS_INVENTORY_DIR

#define FUSE_USE_VERSION 31

#include <fuse_lowlevel.h>
#include "../engine/engine.h"
#include "dir.h"
#include "file.h"
#include "../engine/inventory.h"
#include <stdlib.h>
#include <string.h>

int is_inventory(fuse_ino_t ino);
int is_inventory_item(fuse_ino_t ino);
int is_inv_check(fuse_ino_t ino);

struct dirbuf *read_inventory_dir(fuse_req_t req, fuse_ino_t ino);
void inventory_dir_getattr(fuse_ino_t ino, struct stat *stbuf);
void inventory_dir_lookup(fuse_ino_t parent, const char *name, struct fuse_entry_param *e);

void inventory_item_getattr(fuse_ino_t ino, struct stat *stbuf);
void inventory_item_read(fuse_req_t req, fuse_ino_t ino, size_t size,
		                    off_t off, struct fuse_file_info *fi);

void inv_check_getattr(fuse_ino_t ino, struct stat *stbuf);
void inv_check_read(fuse_req_t req, fuse_ino_t ino, size_t size,
		                    off_t off, struct fuse_file_info *fi);

#endif