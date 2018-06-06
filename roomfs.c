/*
  FUSE: Filesystem in Userspace
  Copyright (C) 2001-2007  Miklos Szeredi <miklos@szeredi.hu>

  This program can be distributed under the terms of the GNU GPL.
  See the file COPYING.
*/

/** @file
 *
 * minimal example filesystem using low-level API
 *
 * Compile with:
 *
 *     gcc -Wall hello_ll.c `pkg-config fuse3 --cflags --libs` -o hello_ll
 *
 * ## Source code ##
 * \include hello_ll.c
 */

#define FUSE_USE_VERSION 31

#include <fuse_lowlevel.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stddef.h>
#include <assert.h>
#include "src/engine/engine.h"
#include "src/fs/dir.h"
#include "src/fs/inventory_dir.h"

/* Command line parsing */
struct options {
    char *path;
};

static struct options options = {
    .path = "./res/demo.conf"
};

#define OPTION(t, p)                           \
    { t, offsetof(struct options, p), 1 }
    
static const struct fuse_opt option_spec[] = {
    OPTION("--path=%s", path),
    FUSE_OPT_END
};


static void roomfs_lookup(fuse_req_t req, fuse_ino_t parent, const char *name)
{
	printf("roomfs lookup: ino - 0x%08x, name - %s\n", parent, name);
	struct fuse_entry_param e;
    if (is_dir(parent)) {

		memset(&e, 0, sizeof(e));
		dir_lookup(parent, name, &e);
		fuse_reply_entry(req, &e);

	} else if (is_inventory(parent)) {

		memset(&e, 0, sizeof(e));
		inventory_dir_lookup(parent, name, &e);
		fuse_reply_entry(req, &e);

	} else {
		fuse_reply_err(req, ENOENT);
	}

}

static void roomfs_getattr(fuse_req_t req, fuse_ino_t ino,
                        struct fuse_file_info *fi) {

    (void) fi;
	printf("roomfs getattr: ino - 0x%08x\n", ino);

	struct stat stbuf;
	memset(&stbuf, 0, sizeof(stbuf));

    if (is_dir(ino)) {
        dir_getattr(ino, &stbuf);
        fuse_reply_attr(req, &stbuf, 30);
		printf("roomfs getattr: ino - 0x%08x, found!\n", ino);

    } else if (is_dir_description(ino)) {
        dir_description_getattr(ino, &stbuf);
        fuse_reply_attr(req, &stbuf, 30);
		printf("roomfs getattr: ino - 0x%08x, found!\n", ino);

	} else if (is_file(ino)) {
		file_getattr(ino, &stbuf);
        fuse_reply_attr(req, &stbuf, 30);
		printf("roomfs getattr: ino - 0x%08x, found!\n", ino);

	} else if (is_inventory(ino)) {
		inventory_dir_getattr(ino, &stbuf);
        fuse_reply_attr(req, &stbuf, 30);
		printf("roomfs getattr: ino - 0x%08x, found!\n", ino);

	} else if (is_inventory_item(ino)) {
		inventory_item_getattr(ino, &stbuf);
        fuse_reply_attr(req, &stbuf, 30);
		printf("roomfs getattr: ino - 0x%08x, found!\n", ino);

	} else if (is_inv_check(ino)) {
		inv_check_getattr(ino, &stbuf);
        fuse_reply_attr(req, &stbuf, 30);
		printf("roomfs getattr: ino - 0x%08x, found!\n", ino);

	} else {
		printf("roomfs getattr: ino - 0x%08x, not found!\n", ino);
        fuse_reply_err(req, ENOENT);
    }

}



static void roomfs_readdir(fuse_req_t req, fuse_ino_t ino, size_t size,
                        off_t off, struct fuse_file_info *fi) {

	printf("roomfs readdir: ino - 0x%08x, size - %d, off- %d\n", ino, size, off);
    (void) fi;
    if (is_dir(ino)) {
        struct dirbuf *b = read_dir(req, ino);

        fuse_reply_buf(req, b->p + off,
                    min(b->size - off, size));
		free(b->p);
		free(b);
    } else if (is_inventory(ino)) {
		struct dirbuf *b = read_inventory_dir(req, ino);

        fuse_reply_buf(req, b->p + off,
                    min(b->size - off, size));
		free(b->p);
		free(b);
	} else {
        fuse_reply_err(req, ENOTDIR);
    }
}


static void roomfs_opendir(fuse_req_t req, fuse_ino_t ino, struct fuse_file_info *fi) {
	(void) fi;
	printf("roomfs opendir: 0x%08x\n", ino);
	if (is_dir(ino)) {
		fuse_reply_open(req, fi);
	} else if (is_inventory(ino)) {
		fuse_reply_open(req, fi);
	} else {
        fuse_reply_err(req, ENOTDIR);
	}
}


static void roomfs_open(fuse_req_t req, fuse_ino_t ino,
		struct fuse_file_info *fi) {
	if (is_dir_description(ino)) {
		fuse_reply_open(req, fi);
	} else if (is_file(ino)) {
		fuse_reply_open(req, fi);
	} else if (is_inv_check(ino)) {
		fuse_reply_open(req, fi);
	} else if (is_inventory_item(ino)) {
		fuse_reply_open(req, fi);
	} else {
		fuse_reply_err(req, ENOENT);
	}
}

static void roomfs_read(fuse_req_t req, fuse_ino_t ino, size_t size,
		off_t off, struct fuse_file_info *fi) {
	if (is_dir_description(ino)) {
		dir_description_read(req, ino, size, off, fi);
	} else if (is_file(ino)) {
		file_read(req, ino, size, off, fi);
	} else if (is_inv_check(ino)) {
		inv_check_read(req, ino, size, off, fi);
	} else if (is_inventory_item(ino)) {
		inventory_item_read(req, ino, size, off, fi);
	} else {
		fuse_reply_err(req, ENOENT);
	}
}

 
static void roomfs_write(fuse_req_t req, fuse_ino_t ino, const char *buf,
		size_t size, off_t off, struct fuse_file_info *fi) {
	if (is_file(ino)) {
		file_write(req, ino, buf, size, off, fi);
	} else {
		fuse_reply_err(req, ENOENT);
	}
}

static void roomfs_release(fuse_req_t req, fuse_ino_t ino, struct fuse_file_info *fi) {
	if (is_file(ino)) {
		file_close(ino);
	}
	fuse_reply_err(req, 0);
	
}


static struct fuse_lowlevel_ops roomfs_ll_oper = {
	.lookup		= roomfs_lookup,
	.getattr	= roomfs_getattr,
	.readdir	= roomfs_readdir,
	.opendir    = roomfs_opendir,
	.open		= roomfs_open,
	.read		= roomfs_read,
	.release    = roomfs_release,
	.write      = roomfs_write,
};

int main(int argc, char *argv[])
{
	struct fuse_args args = FUSE_ARGS_INIT(argc, argv);
	struct fuse_session *se;
	struct fuse_cmdline_opts opts;
	int ret = -1;


    if (fuse_opt_parse(&args, &options, option_spec, NULL) == -1)
        return 1;

	if (fuse_parse_cmdline(&args, &opts) != 0)
		return 1;


	if (opts.show_help) {
		printf("usage: %s [options] <mountpoint>\n\n", argv[0]);
		fuse_cmdline_help();
		fuse_lowlevel_help();
		ret = 0;
		goto err_out1;
	} else if (opts.show_version) {
		printf("FUSE library version %s\n", fuse_pkgversion());
		fuse_lowlevel_version();
		ret = 0;
		goto err_out1;
	}

    engine_init(options.path);

	se = fuse_session_new(&args, &roomfs_ll_oper,
			      sizeof(roomfs_ll_oper), NULL);
	if (se == NULL)
	    goto err_out1;

	if (fuse_set_signal_handlers(se) != 0)
	    goto err_out2;

	if (fuse_session_mount(se, opts.mountpoint) != 0)
	    goto err_out3;

	fuse_daemonize(opts.foreground);

	/* Block until ctrl+c or fusermount -u */
	if (opts.singlethread)
		ret = fuse_session_loop(se);
	else
		ret = fuse_session_loop_mt(se, opts.clone_fd);

	fuse_session_unmount(se);
err_out3:
	fuse_remove_signal_handlers(se);
err_out2:
	fuse_session_destroy(se);
err_out1:
	free(opts.mountpoint);
	fuse_opt_free_args(&args);

    engine_destroy();
	return ret ? 1 : 0;
}
