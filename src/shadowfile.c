#define FUSE_USE_VERSION 26
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef linux
#define _XOPEN_SOURCE 700
#endif

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

