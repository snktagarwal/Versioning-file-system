#ifndef _VERSIONING_UTILS_H_
#define _VERSIONING_UTILS_H_
#include "versioning.h"

int fuse_diff (const char * filepath1, const char * filepath2, const char * difffilepath); // returns number of lines in difffile

void split_file_path (const char * filepath, char *filename, char *dirpath);

int get_current_version_no(file_data *file);

char *get_file_verno_path(file_data *file,int version_no);

int extract_version_no(char * logline);


#endif
