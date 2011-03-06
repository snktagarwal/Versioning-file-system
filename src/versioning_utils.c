#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <glib.h>
#include "versioning_utils.h"

void split_file_path (const char * filepath, char *filename, char *dirpath) {
	int len,len1=0;
	
	len = strlen(filepath);
	strcpy(dirpath,filepath);
	while(dirpath[len] != '/'){
		len--;
		filename[len1++] = dirpath[len];
	}
	dirpath[len] = '\0';
	len1--;
	filename[len1] = '\0';
	strcat(dirpath,"/");
	g_strreverse(filename);
}
