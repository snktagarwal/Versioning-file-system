/* Defines the utility functions for vfs non versioning functionality
 * headers are defined in vfs.h
 */
 
#include<glib.h>
#include<string.h> 
#include "vfs.h"
char *get_log_file_name(char *filepath)
{
	int len,len1=0;
	char filename[255];
	
	len = strlen(filepath);
	
	while(filepath[len] != '/'){
		len--;
		filename[len1++] = filepath[len];
	}
	filepath[len] = '\0';
	len1--;
	filename[len1] = '\0';
	
	g_strreverse(filename);
	
	/* Create log file name */
	strcat(filepath,"/.ver/");
	strcat(filepath, filename);
	strcat(filepath,".log");
	
	return filepath;
	
}
