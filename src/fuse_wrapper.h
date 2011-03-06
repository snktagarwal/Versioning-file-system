#ifndef _FUSE_WRAPPER_H_
#define _FUSE_WRAPPER_H_

int mkdir_ver (const char *dirpath);

int does_exist(const char *filepath);

int copy(const char * source, const char * dest);

int move(const char * source, const char * dest);

void diff(const char * filepath1, const char * filepath2, const char * diff_filepath );

void rem(const char * filepath);

#endif
