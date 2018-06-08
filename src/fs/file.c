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
    struct item *cur_item = ino_to_item(ino);
    stbuf->st_mode = S_IFREG | 0777;
    stbuf->st_nlink = 1;
    stbuf->st_ino = (fuse_ino_t)cur_item;
    char *des_text = wrap_sh_echo(description_gen_text(cur_item->description));
	stbuf->st_size = strlen(des_text);
    free(des_text);
}


int reply_buf_limited(fuse_req_t req, const char *buf, size_t bufsize,
			     off_t off, size_t maxsize)
{
    printf("reply_buf_limited %d %d %d\n", bufsize, off, maxsize);
	if (off < bufsize)
		return fuse_reply_buf(req, buf + off,
				      min(bufsize - off, maxsize));
	else
		return fuse_reply_buf(req, NULL, 0);
}


void file_read(fuse_req_t req, fuse_ino_t ino, size_t size,
	off_t off, struct fuse_file_info *fi) {

    printf("file read: %d %d\n", size, off);
    
    struct item *cur_item = ino_to_item(ino);
    if (cur_item->is_in_inventory) {
        char l[MAX_INPUT_BUFFER];
        sprintf(l, "%llu\n", cur_item->inventory_flag);
    	reply_buf_limited(req, l, strlen(l), off, size);
    } else {
        char *des_text = description_gen_text(cur_item->description);
        des_text = wrap_sh_echo(des_text);
    	reply_buf_limited(req, des_text, strlen(des_text), off, size);
        free(des_text);
    }

}


void file_write(fuse_req_t req, fuse_ino_t ino, const char *buf,
		size_t size, off_t off, struct fuse_file_info *fi) {
    struct item *cur_item = ino_to_item(ino);
    size_t write_size = size;
    if (write_size + off > MAX_INPUT_BUFFER-1) {
        write_size = MAX_INPUT_BUFFER - off - 1;
    }

    strncpy(cur_item->input_buffer + off, buf, write_size);
    cur_item->input_buffer[off + write_size] = '\0';
    if (cur_item->input_buffer[off + write_size - 1] == '\n') {
        cur_item->input_buffer[off + write_size - 1] = '\0';
    }
    printf("after write: %s\n", cur_item->input_buffer);
    fuse_reply_write(req, write_size);
}

void file_close(fuse_ino_t ino) {
    struct item *cur_item = ino_to_item(ino);
    item_trigger_reactions(cur_item);
}