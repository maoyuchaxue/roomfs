#include "inventory_dir.h"


extern struct inventory inv;

void inventory_dir_getattr(fuse_ino_t ino, struct stat *stbuf) {
    stbuf->st_mode = S_IFDIR | 0777;
    stbuf->st_nlink = 1;
    stbuf->st_ino = ino;
}

void inventory_dir_lookup(fuse_ino_t parent, const char *name, struct fuse_entry_param *e) {

    const char *inv_check = "check";
    if (strcmp(name, inv_check) == 0) {
        e->ino = (fuse_ino_t)&(inv.item_list_len);
        e->attr_timeout = 1.0;
        e->entry_timeout = 1.0;
        inv_check_getattr(e->ino, &(e->attr));
        printf("lookup_dir found(look): 0x%08x\n", e->ino);
        return;
    }


    for (int i = 0; i < inv.item_list_len; i++) {
        struct item *cur_item = inv.item_list[i];
        if (strcmp(name, cur_item->name) == 0) {
         	e->ino = (fuse_ino_t)&(cur_item->inventory_flag);
            e->attr_timeout = 1.0;
            e->entry_timeout = 1.0;
            inventory_item_getattr(e->ino, &(e->attr));
            return;
        }
    }
}


int is_inventory(fuse_ino_t ino) {
    return (struct inventory *)ino == &inv;
}

int is_inventory_item(fuse_ino_t ino) {
    for (int i = 0; i < inv.item_list_len; i++) {
        if ((fuse_ino_t)&(inv.item_list[i]->inventory_flag) == ino) {
            return 1;
        }
    }
    return 0;
}

int is_inv_check(fuse_ino_t ino) {
    return ((int *)ino) == &(inv.item_list_len);
}

struct item* ino_to_inv_item(fuse_ino_t ino) {
    for (int i = 0; i < inv.item_list_len; i++) {
        if ((fuse_ino_t)&(inv.item_list[i]->inventory_flag) == ino) {
            return inv.item_list[i];
        }
    }
    return NULL;
}

struct dirbuf *read_inventory_dir(fuse_req_t req, fuse_ino_t ino) {

    struct dirbuf *b = malloc(sizeof(struct dirbuf));
    memset(b, 0, sizeof(struct dirbuf));

    dirbuf_add(req, b, "check", (fuse_ino_t)(&(inv.item_list_len)));

    for (int i = 0; i < inv.item_list_len; i++) {
        dirbuf_add(req, b, inv.item_list[i]->name, (fuse_ino_t)&(inv.item_list[i]->inventory_flag));
    }

    return b;
}


void inventory_item_getattr(fuse_ino_t ino, struct stat *stbuf) {
    char *flag_str = malloc(MAX_OBJ_NAME_LEN * sizeof(char));
    struct item *cur_item = ino_to_inv_item(ino);

    sprintf(flag_str, "%llu", cur_item->inventory_flag);
    flag_str = wrap_sh_echo(flag_str);
    printf("flagstr: %s\n", flag_str);
    stbuf->st_mode = S_IFREG | 0777;
    stbuf->st_nlink = 1;
    stbuf->st_ino = ino;
    stbuf->st_size = strlen(flag_str);
    free(flag_str);
}


void inventory_item_read(fuse_req_t req, fuse_ino_t ino, size_t size,
		                    off_t off, struct fuse_file_info *fi) {
        
    struct item *cur_item = ino_to_inv_item(ino);
    
    char *flag_str = malloc(MAX_OBJ_NAME_LEN * sizeof(char));
    sprintf(flag_str, "%llu", cur_item->inventory_flag);
    flag_str = wrap_sh_echo(flag_str);

    printf("flagstr: %s\n", flag_str);
    reply_buf_limited(req, flag_str, strlen(flag_str), off, size);
    free(flag_str);
}


char *construct_inv_check_script() {
    char *f = malloc(MAX_DESCRIPTION_LEN * sizeof(char));
    memset(f, 0, MAX_DESCRIPTION_LEN * sizeof(char));

    f = strcat(f, "if [ -z \"$1\" ]; then \n  echo \"usage: check item\" \n  exit \nfi \n");

    for (int i = 0; i < inv.item_list_len; i++) {
        char *item_name = inv.item_list[i]->name;
        char *item_description = description_gen_text(inv.item_list[i]->description);
        item_description = wrap_sh_echo(item_description);
        char tmp_f[MAX_DESCRIPTION_LEN];

        sprintf(tmp_f, "if [ \"$1\" == \"%s\" ]; then \n  %s\n  exit\nfi\n", item_name, item_description);
        free(item_description);
        f = strcat(f, tmp_f);
    }

    return f;
} 


void inv_check_getattr(fuse_ino_t ino, struct stat *stbuf) {
    char *inv_check_script = construct_inv_check_script();
    stbuf->st_mode = S_IFREG | 0777;
    stbuf->st_nlink = 1;
    stbuf->st_ino = ino;
    stbuf->st_size = strlen(inv_check_script);
}


void inv_check_read(fuse_req_t req, fuse_ino_t ino, size_t size,
		                    off_t off, struct fuse_file_info *fi) {

    char *inv_check_script = construct_inv_check_script();
    reply_buf_limited(req, inv_check_script, strlen(inv_check_script), off, size);
}