#ifndef _VERSIONING_UTILS_H_
#define _VERSIONING_UTILS_H_

int fuse_diff (const char * filepath1, const char * filepath2, const char * difffilepath); // returns number of lines in difffile

void split_file_path (const char * filepath, char *filename, char *dirpath);

#endif
