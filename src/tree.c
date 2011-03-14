#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <glib.h>
#include "vfs.h"
#include "log.h"
#define printf log_msg

/* TREE File function
 * Checks whether a given offset structure is a junction or not
 */

int isJunction(file_data *file,int offset) //prevent this data structure from being edited.
{

   FILE *fp_tree, *fp_head;
   fp_tree=fopen(file->tree_file_path,"r");
   fp_head=fopen(file->heads_file_path,"r");

   if(fp_tree==NULL || fp_head==NULL) //do check if file exists.
   {
   	printf("ERROR: tree or head file does not exist");
	return -1;// return -1 if file is not opened.
   }

   //else continue.
   char *current_timestamp1=(char *)malloc(15*sizeof(char));
   int current_offset;
   fscanf(fp_head,"%s %d",current_timestamp1, &current_offset);
   char *current_timestamp=(char *)malloc(15*sizeof(char));
   int tmp_int;
   int parent_offset;
   char *tmp_char=(char *)malloc(255*sizeof(char));
          
	while (fscanf(fp_head,"%s %d",current_timestamp, &current_offset) != EOF )
	{
        	//move into the tree file
		do{
			fseek(fp_tree,current_offset,SEEK_SET);
		
			fscanf(fp_tree,"%d", &tmp_int);
			fscanf(fp_tree,"%d", &tmp_int);
			fscanf(fp_tree,"%d", &tmp_int);
			fscanf(fp_tree,"%s", tmp_char);
			fscanf(fp_tree,"%s", tmp_char);
			fscanf(fp_tree,"%d", &tmp_int);
			fscanf(fp_tree,"%d", &parent_offset);
			if(parent_offset==offset && strcmp(current_timestamp, current_timestamp1)!=0) // if we find it even once then it means it is a junction.
	  			return 1;
		
			current_offset = parent_offset;  // move to the parent
		
		}while(parent_offset != -1);

   	}
   	fclose(fp_tree);
   	fclose(fp_head);
	return 0;	
}

/* Updates tree file
 * Scenario:
 * 1. Creation of new version: Appends the version metadata
 */

void update_tree_data(file_data *file,TreeMd *ver,int is_creating_branch,int is_first_version) 
{
	//*************Add spaces to TreeMd fields
	
	char * diff_lc = (char * )malloc(15 * sizeof(char));
	char * parent = (char * )malloc(10 * sizeof(char));
	printf("Adding Spaces\n");
//	addspaces(ver->obj_hash,41);
//	printf("obj_hash : %s\n",ver->obj_hash);
	sprintf(diff_lc,"%d",ver->diff_count);
	addspaces(diff_lc,10);
//	ver->obj_hash = addspaces(ver->obj_hash,40);
	addspaces(ver->tag,255);
	itoa(ver->parent,parent);
//	sprintf(parent,"%d",ver->parent);
	addspaces(parent,10);
	printf("Tag : %s\n",ver->tag);
	printf("Parent : %s\n",parent);
	printf("Diif Line Count : %s\n",diff_lc);
	printf("obj_hash : %s\n",ver->obj_hash);
	
	//******************************************
	if(!is_creating_branch)
	{
	//	if(!is_first_version)
		{	
			int a;
			printf(" Making lo tp po\n");
			FILE * f = fopen(file->tree_file_path,"r+");
			fseek(f,ver->parent,SEEK_SET);
			fscanf(f,"%d",&a);
			printf("First scanned : %d\n",a);
			fscanf(f,"%d",&a);
			printf("First scanned : %d\n",a);
			fprintf(f," 1");
			fclose(f);
		}
		
	}
	FILE * file_tree = fopen(file->tree_file_path,"a");
	fprintf(file_tree,"%d %d %d %s %s %s %s\n",ver->valid,ver->timestamp,ver->file_type,ver->obj_hash,ver->tag,diff_lc,parent);
//	fprintf(file_tree,"%d %d %d %s %s %d %d\n",ver->valid,ver->timestamp,ver->file_type,ver->obj_hash,ver->tag,ver->diff_count,ver->parent);
	fclose(file_tree);
}

/* Constructs the Tree Metadata
 * Input: Given a file name and whether this is the first version
 */
TreeMd * construct_version_data(file_data * file,int is_first_version) 
{
	TreeMd *ver = (TreeMd *) malloc(sizeof(version_data));
	
	// version number
//	if(!does_exist( file->ver_dir_path )) {
//	return NULL;
//	}
	printf("Is first version in construct version data : %d\n",is_first_version);
	if(!is_first_version) 
	{
		printf("****************This is not the first version******************\n");
		ver->parent = get_present_head_offset(file->heads_file_path);
	}
	else
	{
		printf("****************This is the first version******************\n");
		ver->parent = -1;
	}
	
	ver->valid = 1;
	find_SHA(file->path,ver->obj_hash);
	
	// tag ---- TODO
	strcpy(ver->tag, "_");
	
	// timestamp
	ver->timestamp = (int) time(NULL);
	
	// diff_lc ----- TODO
	ver->diff_count = 0;
	ver->file_type = 0;
	return ver;
}

