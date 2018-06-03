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
	stbuf->st_size = strlen(cur_item->description);
}


int reply_buf_limited(fuse_req_t req, const char *buf, size_t bufsize,
			     off_t off, size_t maxsize)
{
	if (off < bufsize)
		return fuse_reply_buf(req, buf + off,
				      min(bufsize - off, maxsize));
	else
		return fuse_reply_buf(req, NULL, 0);
}


void file_read(fuse_req_t req, fuse_ino_t ino, size_t size,
	off_t off, struct fuse_file_info *fi) {
    
    struct item *cur_item = ino_to_item(ino);
	reply_buf_limited(req, cur_item->description, strlen(cur_item->description), off, size);
}