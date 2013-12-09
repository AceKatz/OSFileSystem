
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

//gets file, directory attributes
static int sh_getattr(const char *path, struct stat *stbuf) {
    int res = 0;
    struct user* user = find_user(root, path+1);
    
    char uname[9];
    user_from_path(path, uname);
    struct user* user2 = find_user(root, uname);
    
    printf("getattr\n");
    memset(stbuf, 0, sizeof(struct stat));
    
    if (strcmp(path, "/") == 0) {         //get attributes of root
        stbuf->st_mode = S_IFDIR | 0755;
	stbuf->st_nlink = 2;
    }
    else if(user != NULL) {               //get attr of a user
        stbuf->st_mode = S_IFDIR | 0755;
	stbuf->st_nlink = 1;
    }
    else if(user2 != NULL) {              //get attr of file
        stbuf->st_mode = S_IFREG | 0444;
	stbuf->st_nlink = 1;
	stbuf->st_size = 15;
    }
    else
        res = -ENOENT;
    
    return res;
}

static int sh_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
			 off_t offset, struct fuse_file_info *fi) {
    (void) offset;
    (void) fi;
    printf("readdir\n");
    
    struct user* user = root->head;

    //in root, display users
    if (strcmp(path, "/") == 0) {
        filler(buf, ".", NULL, 0);
	filler(buf, "..", NULL, 0);
	
	while(user) {
	  filler(buf, user->username, NULL, 0);
	  user = user->next;
	}
    }
    //in a user, display files
    else {
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
	filler(buf, "days_before_warning", NULL, 0);
	filler(buf, "days_until_expiration", NULL, 0);
	filler(buf, "days_since_account_deactivated", NULL, 0);
	if(user->reserved != NULL)
	    filler(buf, "reserved", NULL, 0);
    }
    
    return 0;
}

//not completely sure why this is here
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
    
    struct user* user = find_user(root, path+1);
    char uname[9];
    
    //only want to read if in a user directory
    if(user != NULL || strcmp(path, "/") == 0)
        return -ENOENT;
    
    int i = user_from_path(path, uname);
    user = find_user(root, uname);
    i+=2;
    
    if(user == NULL)
        return -ENOENT;
    
    if(strcmp(path+i, "hash") == 0) {
        memcpy(buf, user->hash + offset, size);
    }
    else if(strcmp(path+i, "days_since_changed") == 0) {
        char s[10];
	sprintf(s, "%d", user->dsc);
	memcpy(buf, s + offset, sizeof(int));
	size = sizeof(int);
    }
    else if(strcmp(path+i, "days_until_can_change") == 0) {
        char s[10];
	sprintf(s, "%d", user->dcc);
	memcpy(buf, s + offset, sizeof(int));
	size = sizeof(int);
    }
    else if(strcmp(path+i, "days_until_must_change") == 0) {
        char s[10];
	sprintf(s, "%d", user->dmc);
	memcpy(buf, s + offset, sizeof(int));
	size = sizeof(int);
    }
    else if(strcmp(path+i, "days_before_warning") == 0) {
        char s[10];
	sprintf(s, "%d", user->dw);
	memcpy(buf, s + offset, sizeof(int));
	size = sizeof(int);
    }
    else if(strcmp(path+i, "days_until_expiration") == 0) {
        char s[10];
	sprintf(s, "%d", user->de);
	memcpy(buf, s + offset, sizeof(int));
	size = sizeof(int);
    }
    else if(strcmp(path+i, "days_since_account_deactivated") == 0) {
        char s[10];
	sprintf(s, "%d", user->dd);
	memcpy(buf, s + offset, sizeof(int));
	size = sizeof(int);
    }
    else if(strcmp(path+i, "reserved") == 0) {
        if(user->reserved != NULL)
	    memcpy(buf, user->reserved + offset, size);
    }
    return size;
}

static int sh_write(const char *path, char *buf, size_t size, off_t offset,
		    struct fuse_file_info *fi) {
    struct user* user = find_user(root, path+1);
    char uname[9];
    printf("write\n");
    if(user != NULL || strcmp(path, "/") == 0)
        return -ENOENT;
    
    int i = user_from_path(path, uname);
    user = find_user(root, uname);
    i+=2;
    
    if(user == NULL)
        return -ENOENT;
    
    if(strcmp(path+i, "hash") == 0) {
		char* hashed = hashword(buf);
		update_hash(root, uname, hashed);

    }
    else if(strcmp(path+i, "days_since_changed") == 0) {
      //user->dsc = atoi(buf);
	update_daysSinceChanged(root, uname, atoi(buf));
    }
    else if(strcmp(path+i, "days_until_can_change") == 0) {
      //user->dcc = atoi(buf);
	update_daysUntilCanChange(root, uname, atoi(buf));
    }
    else if(strcmp(path+i, "days_until_must_change") == 0) {
      //user->dmc = atoi(buf);
      update_daysUntilMustChange(root, uname, atoi(buf));
    }
    else if(strcmp(path+i, "days_before_warning") == 0) {
      //user->dw = atoi(buf);
	update_daysBeforeWarning(root, uname, atoi(buf));
    }
    else if(strcmp(path+i, "days_until_expiration") == 0) {
      //user->de = atoi(buf);
	update_daysUntilExpiration(root, uname, atoi(buf));
    }
    else if(strcmp(path+i, "days_since_account_deactivated") == 0) {
      //user->dd = atoi(buf);
	update_daysSinceDeactivation(root, uname, atoi(buf));
    }
    else if(strcmp(path+i, "reserved") == 0) {
      //strcpy(user->reserved, buf);
	update_reserved(root, uname, atoi(buf));
    }
    return size;
}

//thought I needed this for write
static int sh_truncate(const char *path, off_t size) {
    return 0;
}

//creates user
static int sh_mkdir(const char *path, mode_t mode) {
    int x = sf_tree_add_user(root, path+1);
    
    if(x < 0)
        return -EEXIST;
    
    return 0;
}

//removes user
static int sh_rmdir(const char *path) {
    struct user* user = find_user(root, path+1);
    
    if(user == NULL)
        return -ENOENT;
    
    return sf_tree_delete_user(root, path+1);
}

//renames user
static int sh_rename(const char *from, const char *to) {
    struct user* user = find_user(root, from+1);
    
    if(user == NULL)
        return -EACCES;
    
    strcpy(user->username, to+1);
    
    return 0;
}

static int sh_unlink(const char *path) {
    struct user* user = find_user(root, path+1);
    char uname[9];
    printf("unlink\n");
    if(user != NULL || strcmp(path, "/") == 0)
        return -ENOENT;
    
    int i = user_from_path(path, uname);
    user = find_user(root, uname);
    i+=2;
    
    if(user == NULL)
        return -ENOENT;
    
    if(strcmp(path+i, "hash") == 0) {
        strcpy(user->hash, "");
    }
    else if(strcmp(path+i, "days_since_changed") == 0) {
        user->dsc = 0;
    }
    else if(strcmp(path+i, "days_until_can_change") == 0) {
        user->dcc = 0;
    }
    else if(strcmp(path+i, "days_until_must_change") == 0) {
        user->dmc = 99999;
    }
    else if(strcmp(path+i, "days_before_warning") == 0) {
        user->dw = 7;
    }
    else if(strcmp(path+i, "days_until_expiration") == 0) {
        user->de = 0;
    }
    else if(strcmp(path+i, "days_since_account_deactivated") == 0) {
        user->dd = 0;
    }
    else if(strcmp(path+i, "reserved") == 0) {
        user->reserved = NULL;
    }
    return 0;
}

//only edits reserved attribute for user
static int sh_mknod(const char *path, mode_t mode, dev_t rdev) {
    struct user* user = find_user(root, path+1);
    char uname[9];
    printf("mknod\n");
    if(user != NULL || strcmp(path, "/") == 0)
        return -ENOENT;
    
    int i = user_from_path(path, uname);
    user = find_user(root, uname);
    i+=2;
    
    user->reserved = malloc(50);
    strcpy(user->reserved, path+i);
    printf("%s\n\n", user->reserved);
    
    return 0;
}

static int sh_utimens(const char *path, const struct timespec ts[2]) {
    printf("utimens\n");
    return 0;
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
    .rename     = sh_rename,
    .unlink     = sh_unlink,
    .mknod      = sh_mknod,
    .utimens    = sh_utimens,
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
