
#define FUSE_USE_VERSION 26

#include <fuse.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include "shadowTree.h"

struct sf_root* root;

static int sh_getattr(const char *path, struct stat *stbuf) {
    int res = 0;
    struct user* user = find_user(root, path+1);
    
    printf("getattr\n");
    memset(stbuf, 0, sizeof(struct stat));
    
    if (strcmp(path, "/") == 0) {
        stbuf->st_mode = S_IFDIR | 0755;
	stbuf->st_nlink = 2;
    } else if(user != NULL) {
        stbuf->st_mode = S_IFDIR | 0755;
	stbuf->st_nlink = 1;
    } else {
        stbuf->st_mode = S_IFREG | 0444;
	stbuf->st_nlink = 1;
	stbuf->st_size = 15;
    }
    //res = -ENOENT;
    //NEED CASE FOR WRONG PATH INPUT
    return res;
}

static int sh_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
			 off_t offset, struct fuse_file_info *fi) {
    (void) offset;
    (void) fi;
    printf("readdir\n");
    
    struct user* user = root->head;
    
    if (strcmp(path, "/") == 0) {
        filler(buf, ".", NULL, 0);
	filler(buf, "..", NULL, 0);
	
	while(user) {
	  filler(buf, user->username, NULL, 0);
	  user = user->next;
	}
    } else {
        user = find_user(root, path+1);
	if(user == NULL) {
	    printf("user null\n");
	    return -ENOENT;
	}

	filler(buf, ".", NULL, 0);
	filler(buf, "..", NULL, 0);
	filler(buf, "hash", NULL, 0);
	filler(buf, "days_since_changed", NULL, 0);
	filler(buf, "days_until_can_change", NULL, 0);
	filler(buf, "days_until_must_change", NULL, 0);
	//finish user attr/info here
    }
    
    return 0;
}

static int sh_open(const char *path, struct fuse_file_info *fi) {
  printf("open\n");
  /*if (strcmp(path, hello_path) != 0)
		return -ENOENT;

	if ((fi->flags & 3) != O_RDONLY)
		return -EACCES;
  */
	return 0;
}

static int sh_read(const char *path, char *buf, size_t size, off_t offset,
		      struct fuse_file_info *fi) {
    printf("read\n");
    size_t len;
    (void) fi;
    printf("path = %s\n\n", path);
    struct user* user = find_user(root, path+1);
    char uname[9];
    
    if(user != NULL || strcmp(path, "/") == 0)
        return -ENOENT;
    
    int i;
    for(i=1; i<10; i++) {
        if(path[i] == '/') break;
	uname[i-1] = path[i];
    }
    uname[i-1] = '\0';
    user = find_user(root, uname);
    i++;
    
    if(user == NULL)
        return -ENOENT;
    
    printf("%s\n", user->hash);

    if(strcmp(path+i, "hash") == 0) {
        memcpy(buf, user->hash + offset, size);
    }
    else if(strcmp(path+i, "days_since_changed") == 0) {
        char s[10];
	sprintf(s, "%d\n", user->dsc);
	memcpy(buf, s + offset, sizeof(int));
    }
    
    return size;
}

static struct fuse_operations sh_oper = {
  .getattr	= sh_getattr,
  .readdir	= sh_readdir,
  .open		= sh_open,
  .read		= sh_read,
};



int main(int argc, char *argv[]) {
  root = sf_tree_init("blank");
  
  int a = sf_tree_add_user(root, "jack");
  strcpy(root->head->hash, "test");
  root->head->dsc = 5;
  
  return fuse_main(argc, argv, &sh_oper, NULL);
}
