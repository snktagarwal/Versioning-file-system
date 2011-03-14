#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <glib.h>
#include "vfs.h"
#include "log.h"
#define printf log_msg
/* Updates the obj_md file
 * 1. After a revert
 * 2. Creation of a new version -- or a new branch creation
 */

void update_objmd_file(char * s1,file_data * file)
{
	char * s = (char *)malloc(HASH_SHA1 * sizeof(char));
	int tmp = 0;
	int reference_count = 0;
	int is_present = 0;
	int pos  = 0;
	FILE * f = fopen(file->OBJ_MD_file_path,"r+");
	
	while(fscanf(f,"%s",s)!=-1)
	{
		printf("Reading a hash\n");
		
//		if(!strcmp(s,ver->obj_hash))
		if(!strcmp(s,s1))
		{
			printf("Same string found\n");
			is_present = 1;
			pos = ftell(f);
			fscanf(f,"%d",&reference_count);
			fseek(f,pos,SEEK_SET);
			fprintf(f," %d\n",reference_count+1);
			fclose(f);
			return;
		}
		fscanf(f,"%d",&tmp);
		is_present = 0;
	}
		fprintf(f,"%s %d\n",s1,reference_count+1);
		fclose(f);
		return;		
}


