#define FUSE_USE_VERSION 26
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#define _XOPEN_SOURCE 700
#include <stdlib.h>
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/time.h>

#ifdef HAVE_SETXATTR
#include <sys/xattr.h>
#endif

static int shadow_getattr(const char* path, struct stat* stbuf){
  int res;
  res = lstat(path, stbuf);
  if( res == -1 ) return -errno;
  return res;
}

static int shadow_access(const char* path, int mask){
  int res;
  res = access(path, mask);
  if( res == -1) return -errno;
  buf[res] = '\0';
  return 0;
}

static int shadow_mknod(const char* path, mode_t mode, dev_t rdev){
  int res;
  mknod(path, mode, rdev);
  return 0;
}

static int shadow_mkdir(const char* path, mode_t mode){
  int res;
  res = mkdir(path, mode);
  if(res == -1) 
    return -errno;
  return 0;
}

static int shadow_unlink(const char* path){
  int res;
  res = unlink(path);
  if(res == -1) 
    return -errno;
  return 0;
}

static int shadow_rmdir(const char* path){
  int res;
  res =  rmdir(path);
  if(res == -1) 
    return -errno;
  return 0;
}

static int shadow_symlink(const char* from, const char* to){
  int res;
  res = symlink(from,to);
  if(res == -1) 
    return -errno;
  return 0;
}

static int shadow_rename(const char* from, const char* to){
  int res;
  res = rename(from, to);
  if(res == -1) 
    return -errno;
  return 0;
}

static int shadow_link(const char* from, const char* to){
  int res;
  res = link(from, to);
  if(res ==-1)
    return -errno;
  return 0;
}

static struct fuse_operations shadow_oper = {
  .getattr            = shadow_getattr,
  .access             = shadow_access;
  .readlink           = shadow_readlink,
  .readdir            = shadow_readdir,
  .open               = shadow_open,
  .read               = shadow_read,
  .write              = shadow_write,
}
