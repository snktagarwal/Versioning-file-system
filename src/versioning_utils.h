#ifndef _VERSIONING_UTILS_H_
#define _VERSIONING_UTILS_H_
#include <glib.h>
#include "versioning.h"


int fuse_diff (const char * filepath1, const char * filepath2, const char * difffilepath); // returns number of lines in difffile

void split_file_path (const char * filepath, char *filename, char *dirpath);

int get_current_version_no(file_data *file);

char *get_file_verno_path(file_data *file,int version_no);

int extract_version_no(char * logline);

void writeHTtoFile(GHashTable *ht,FILE *f);

GHashTable *readHTfromFile(FILE *f,int i);

void insertAtTop(char *filepath, char *epoch, int off);

void deleteTop(char *filepath);

void replaceTop(char *filepath, char *epoch, int off);

int find_present_file_offset(char * filepath);

char * addspaces(char *str, int size);

int get_current_offset();

int get_present_head_offset(char * filepath);

void find_SHA(const char * filepath, char c[41]);

char * get_hash_from_offset(int offset,char  * file_tree_path);

#endif
