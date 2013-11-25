/**
 * fusemain.c
 * 
 * First flailing attempt at mounting a filesystem
 * @time 2013-11-17 21:46
 */

#define FUSE_USE_VERSION 26

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
static const char *fuck_str = "fuck you!\n";
static const char *fuck_path = "/balls";
static int fuck_getattr(const char *path, struct stat *stbuf)
{
        int res = 0;
        memset(stbuf, 0, sizeof(struct stat));
        if (strcmp(path, "/") == 0) {
                stbuf->st_mode = S_IFDIR | 0755;
                stbuf->st_nlink = 2;
        } else if (strcmp(path, fuck_path) == 0) {
                stbuf->st_mode = S_IFREG | 0444;
                stbuf->st_nlink = 1;
                stbuf->st_size = strlen(fuck_str);
        } else
                res = -ENOENT;
        return res;
}
static int fuck_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                         off_t offset, struct fuse_file_info *fi)
{
        (void) offset;
        (void) fi;
        if (strcmp(path, "/") != 0)
                return -ENOENT;
        filler(buf, ".", NULL, 0);
        filler(buf, "..", NULL, 0);
        filler(buf, fuck_path + 1, NULL, 0);
        return 0;
}
static int fuck_open(const char *path, struct fuse_file_info *fi)
{
        if (strcmp(path, fuck_path) != 0)
                return -ENOENT;
        if ((fi->flags & 3) != O_RDONLY)
                return -EACCES;
        return 0;
}
static int fuck_read(const char *path, char *buf, size_t size, off_t offset,
                      struct fuse_file_info *fi)
{
        size_t len;
        (void) fi;
        if(strcmp(path, fuck_path) != 0)
                return -ENOENT;
        len = strlen(fuck_str);
        if (offset < len) {
                if (offset + size > len)
                        size = len - offset;
                memcpy(buf, fuck_str + offset, size);
        } else
                size = 0;
        return size;
}

static struct fuse_operations fuck_oper = {
	.getattr		= fuck_getattr,
	.readdir        = fuck_readdir,
	.open           = fuck_open,
	.read           = fuck_read,
};

int main(int argc, char *argv[])
{
	umask(0);
	int ret = fuse_main(argc, argv, &fuck_oper, NULL);
	return ret;
}
