
#define FUSE_USE_VERSION 26

#include <fuse.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include "shadowTree.h"
#include "parse.h"

struct sf_root* root;

//extracts username from path, returns length of username
int user_from_path(const char *path, char *uname) {
    int i;
    for(i=1; i<10; i++) {
        if(path[i] == '/') break;
	uname[i-1] = path[i];
    }
    uname[i-1] = '\0';
    return i-1;
}

static int sh_getattr(const char *path, struct stat *stbuf) {
    int res = 0;
    struct user* user = find_user(root, path+1);
    
    char uname[9];
    user_from_path(path, uname);
    struct user* user2 = find_user(root, uname);
    
    printf("getattr\n");
    memset(stbuf, 0, sizeof(struct stat));
    
    if (strcmp(path, "/") == 0) {
        stbuf->st_mode = S_IFDIR | 0755;
	stbuf->st_nlink = 2;
    } else if(user != NULL) {
        stbuf->st_mode = S_IFDIR | 0755;
	stbuf->st_nlink = 1;
    } else if(user2 != NULL) {
        stbuf->st_mode = S_IFREG | 0444;
	stbuf->st_nlink = 1;
	stbuf->st_size = 15;
    } else
        res = -ENOENT;
    
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
	filler(buf, "days_before_deactivation", NULL, 0);
	filler(buf, "days_since_failed_to_change_pw", NULL, 0);
	filler(buf, "days_since_account_deactivated", NULL, 0);
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
    //printf("path = %s\n\n", path);
    struct user* user = find_user(root, path+1);
    char uname[9];
    
    if(user != NULL || strcmp(path, "/") == 0)
        return -ENOENT;
    
    int i = user_from_path(path, uname);
    /*for(i=1; i<10; i++) {
        if(path[i] == '/') break;
	uname[i-1] = path[i];
    }
    uname[i-1] = '\0';*/
    //printf("%d %s\n", i, uname);
    user = find_user(root, uname);
    i+=2;
    
    if(user == NULL)
        return -ENOENT;
    
    //printf("%s\n", path+i);
    if(strcmp(path+i, "hash") == 0) {
        memcpy(buf, user->hash + offset, size);
    }
    else if(strcmp(path+i, "days_since_changed") == 0) {
        char s[10];
	//user->dsc = 5;
	sprintf(s, "%d", user->dsc);
	memcpy(buf, s + offset, sizeof(int));
    }
    else if(strcmp(path+i, "days_until_can_change") == 0) {
        char s[10];
	//user->dsc = 5;
	sprintf(s, "%d", user->dcc);
	memcpy(buf, s + offset, sizeof(int));
    }
    else if(strcmp(path+i, "days_until_must_change") == 0) {
        char s[10];
	//user->dsc = 5;
	sprintf(s, "%d", user->dmc);
	memcpy(buf, s + offset, sizeof(int));
    }
    else if(strcmp(path+i, "days_before_deactivation") == 0) {
        char s[10];
	//user->dsc = 5;
	sprintf(s, "%d", user->dw);
	memcpy(buf, s + offset, sizeof(int));
    }
    else if(strcmp(path+i, "days_since_failed_to_change_pw") == 0) {
        char s[10];
	//user->dsc = 5;
	sprintf(s, "%d", user->de);
	memcpy(buf, s + offset, sizeof(int));
    }
    else if(strcmp(path+i, "days_since_account_deactivated") == 0) {
        char s[10];
	//user->dsc = 5;
	sprintf(s, "%d", user->dd);
	memcpy(buf, s + offset, sizeof(int));
    }
    else{}
    //TODO:  add reserved functionality
    return size;
}

static int sh_write(const char *path, char *buf, size_t size, off_t offset,
		    struct fuse_file_info *fi) {
    struct user* user = find_user(root, path+1);
    char uname[9];
    
    if(user != NULL || strcmp(path, "/") == 0)
        return -ENOENT;
    
    int i = user_from_path(path, uname);
    user = find_user(root, uname);
    i+=2;
    
    if(user == NULL)
        return -ENOENT;
    
    if(strcmp(path+i, "hash") == 0) {
        strcpy(user->hash, buf);
    }
    else if(strcmp(path+i, "days_since_changed") == 0) {
        user->dsc = atoi(buf);
    }
    else if(strcmp(path+i, "days_until_can_change") == 0) {
        user->dcc = atoi(buf);
    }
    else if(strcmp(path+i, "days_until_must_change") == 0) {
        user->dmc = atoi(buf);
    }
    else if(strcmp(path+i, "days_before_deactivation") == 0) {
        user->dw = atoi(buf);
    }
    else if(strcmp(path+i, "days_since_failed_to_change_pw") == 0) {
        user->de = atoi(buf);
    }
    else if(strcmp(path+i, "days_since_account_deactivated") == 0) {
        user->dd = atoi(buf);
    }
    else{}

    return size;
}

static int sh_truncate(const char *path, off_t size) {
    return 0;
}

//creates a user
static int sh_mkdir(const char *path, mode_t mode) {
  //figure out way to check if not in root directory
  //if(strcmp(path, "/") != 0)
  //    return -ENOENT;
    
    int x = sf_tree_add_user(root, path+1);
    /*  
    if(x < 0)
        return -EEXIST;
    */
    return 0;
}

static int sh_rmdir(const char *path) {
    struct user* user = find_user(root, path+1);
    
    if(user == NULL)
        return -ENOENT;
    
    return sf_tree_delete_user(root, path+1);
}
	
static struct fuse_operations sh_oper = {
    .getattr	= sh_getattr,
    .readdir	= sh_readdir,
    .open	= sh_open,
    .read	= sh_read,
    .write      = sh_write,
    .truncate   = sh_truncate,
    .mkdir      = sh_mkdir,
    .rmdir      = sh_rmdir,
};

int main(int argc, char *argv[]) {
    root = init_parse(argv[argc-2], "blank");
    
    argv[argc-2] = argv[argc-1];
    argv[argc-1] = NULL;
    argc--;
    
    int f = fuse_main(argc, argv, &sh_oper, NULL);
    
    int d = sf_deparse(root, "shadow.out");
    
    return f;
}
