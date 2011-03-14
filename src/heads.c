#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <glib.h>
#include "vfs.h"
#include "log.h"
#define printf log_msg

/* Updates heads file after creation of new version
 * Calls functions: heads_in_else, replaceTop
 */
void update_heads_file(file_data  * file,TreeMd * ver,int is_first_version,int is_creating_branch)
{
	int current_offset = find_present_file_offset(file->tree_file_path);
	printf("Current offset : %d\n",current_offset);
	
	if(is_first_version)
	{
		char *b_epoch = (char*)malloc(20*sizeof(char));
		strcpy(b_epoch,"B_");
		char *tmp = (char*)malloc(20*sizeof(char));
		itoa(ver->timestamp,tmp);
		strcat(b_epoch,tmp);
		char * offs = (char *)malloc(15*sizeof(char));
		itoa(current_offset, offs);
		FILE *f = fopen(file->heads_file_path,"w");
		fprintf(f,"%s %s\n",b_epoch, addspaces(offs, 10));
		fprintf(f,"%s %s\n",b_epoch, offs);
		fclose(f);
	}
	else if(is_creating_branch)
	{
		char *b_epoch = (char*)malloc(20*sizeof(char));
		strcpy(b_epoch,"B_");
		char *tmp = (char*)malloc(20*sizeof(char));
		itoa(ver->timestamp,tmp);
		strcat(b_epoch,tmp);
		replaceTop(file->heads_file_path,b_epoch,current_offset);
		
	}
	else
	{
		char * tmp_filepath = (char * )malloc(PATH_MAX * sizeof(char));
		strcpy(tmp_filepath,file->ver_dir_path);
		strcat(tmp_filepath,"heads/tmp");

		char *b_epoch = (char*)malloc(20*sizeof(char));
		strcpy(b_epoch,"B_");
		char *tmp = (char*)malloc(20*sizeof(char));
		itoa(ver->timestamp,tmp);
		strcat(b_epoch,tmp);
		heads_in_else(file->heads_file_path,b_epoch,current_offset,tmp_filepath);

		
	}
}

/* HEADS file function
 * Creates a new version while not creating a new branch
 * in the same branch basically
 */
void heads_in_else(char *filepath , char * current_epoch,int current_offset,char * tmp_filepath)
{
	printf("\n--------------------------------------------heads in else=============================================\n");
	char *epoch = (char*)malloc(20*sizeof(char));
	char *first_epoch = (char*)malloc(20*sizeof(char));
	char * off_str = (char*)malloc(20*sizeof(char));
	int off,first_offset;
	FILE *f = fopen(filepath,"r");
	FILE * f1 = fopen(tmp_filepath,"w");
	itoa(current_offset,off_str);
	printf("-----------------------------%s\n",off_str);
	fprintf(f1,"%s %s\n",current_epoch,addspaces(off_str,10));
	fscanf(f,"%s%d",first_epoch,&first_offset);

	while(fscanf(f,"%s %d",epoch,&off) != EOF)
	{
		if(strcmp(first_epoch,epoch) != 0)
		{
			itoa(off,off_str);

			fprintf(f1,"%s %s\n",epoch,addspaces(off_str,10));
		}
	}
	
	
	itoa(current_offset,off_str);
	fprintf(f1,"%s %s\n",current_epoch,addspaces(off_str,10));
	fclose(f);
	fclose(f1);
	copy(tmp_filepath,filepath);
	delete(tmp_filepath);
	
}

/* HEADS File
 * Writes a new version while creating a new branch
 * -- replaces the first line and adds it to a new head
 */
void replaceTop(char *filepath, char *epoch, int off)
{
	printf("-----------------------------------------------in replace top=============================================\n");
	FILE *f = fopen(filepath,"r+");
	char * offs = (char *)malloc(15*sizeof(char));
	itoa(off, offs);
	fprintf(f,"%s %s\n",epoch, addspaces(offs, 10));
	fclose(f);
	printf("\nbhadwe000000000000000000000000000000000000000000000");
	f = fopen(filepath,"a");
	printf("\nbhadwe0000000000000000000000000000000000000000022222");
	fprintf(f,"%s %s\n",epoch, offs);
	printf("\nbhadwe0000000000000000000000000000000000000000079898");
	fclose(f);	
	printf("\nbhadwe000000000000000000000000000000000000000000000");
	return ;
}
