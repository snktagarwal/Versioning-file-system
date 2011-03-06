#ifndef _VERSIONING_UTILS_H_
#define _VERSIONING_UTILS_H_

int fuse_diff (const char * filepath1, const char * filepath2, const char * difffilepath); // returns number of lines in difffile

char * getdirpath (const char * filepath);

char * getfilename (const char * filepath);

#endif
