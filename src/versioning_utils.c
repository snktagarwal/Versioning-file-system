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

int get_current_version_no(file_data * file)
{
	int ver_no;
	FILE *current_ver_number = fopen(file->current_ver_number_path,"r");
	fscanf(current_ver_number,"%d",&ver_no);
	fclose(current_ver_number);
	return ver_no;
}
char * get_file_verno_path(file_data * file,int version_no)
{
	char * path = (char *) malloc(PATH_MAX*sizeof(char));
	sprintf(path,"%s%d",file->ver_dir_path,version_no);
//	printf("I am returning path : %s\n",path);
	return path;
	
}
int extract_version_no(char * logline)
{
int i;
char * v_no = (char * )malloc(255 * sizeof(char));
 for(i=0; i<strlen(logline); i++)
               {
                       if(logline[i]==';')
                               break;
                       v_no[i] = logline[i];
               }
               return(atoi(v_no));
}

