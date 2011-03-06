// There are a couple of symbols that need to be #defined before
// #including all the headers.

#ifndef _PARAMS_H_
#define _PARAMS_H_

// The FUSE API has been changed a number of times.  So, our code
// needs to define the version of the API that we assume.  As of this
// writing, the most current API version is 26
#define FUSE_USE_VERSION 26

// need this to get pwrite().  I have to use setvbuf() instead of
// setlinebuf() later in consequence.
#define _XOPEN_SOURCE 500

// the version directory name in each directory on the file system
#define VER_DIR ".ver/"
#define VER_LOG "log"
#define CURR_VER "current"
#define CURR_VER_NUMBER "current_ver_number"
#define PATH_MAX 512

// maintain vfsfs state in here
#include <limits.h>
#include <stdio.h>
struct vfs_state {
    FILE *logfile;
    char *rootdir;
};
#define BB_DATA ((struct vfs_state *) fuse_get_context()->private_data)

#endif
