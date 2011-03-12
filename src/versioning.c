#define DEBUG

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <fuse.h>

#include "vfs.h"
#include "log.h"
#include "versioning.h"
#include "versioning_utils.h"
#include "fuse_wrapper.h"
#include "params.h"
#define printf log_msg
/**
* logs version data in the following format:
* <version_number>;<timestamp>;<diff_lc>;<tag>
*/

int report_checkout(char * filepath, int req_tp);

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

void iterate1(gpointer key,gpointer val,gpointer f)
{
	char *keystr = (char*)key;
	int *keyval = (int*)val;
	printf("\n---------------------------------in iterator-----------------------------------------------\n");
	printf("hikjkknkjfnek\n");
	//printf("sdsa\t%s\t%s", (gchar*)key, (gchar*)val);
} 

int remove_from_everything(file_data * file, char * hash)
{
	FILE * fp = fopen(file->OBJ_MD_file_path, "r");
	GHashTable * ht = readHTfromFile(fp, 1);
	fclose(fp);
	int i =1;
	g_hash_table_foreach(ht,iterate1, &i);
	char * hash1 = NULL;
	int ref = 0;
	char ** hashp = &hash1; 
	int * refp = &ref;
	
	gboolean pointer_ref_ct = g_hash_table_lookup_extended(ht, (gconstpointer)hash, (gpointer*)hashp, (gpointer*)refp);
	if(!pointer_ref_ct)
	{
		printf("ERROR: there is no entry of this object %s in OBJ_MD file", hash);
		return 0;
	} 
	printf("\n============== %s\t%d===============\n", hash1, ref);
	//printf("=========ref pointer= %d", *pointer_ref_ct);
	/*
	int ref_ct = *pointer_ref_ct;
	if(ref_ct==1)
	{
		g_hash_table_remove(ht, (gconstpointer)hash);		
		char * obj_path = (char *)malloc(PATH_MAX*sizeof(char));
		strcpy(obj_path, file->objects_dir_path);
		strcat(obj_path, hash);
		remove(obj_path);
		return 1;
	}
	ref_ct--;
	g_hash_table_replace(ht, (gpointer)hash, &ref_ct);
	fp = fopen(file->OBJ_MD_file_path, "w");
	writeHTtoFile(ht, fp);
	fclose(fp);
	*/return 0;
}

int isJunction(file_data *file,int offset) //prevent this data structure from being edited.
{
   int counter=1;//because we are in it's branch.
   FILE *fp_tree, *fp_head;
   fp_tree=fopen(file->tree_file_path,"r");
   fp_head=fopen(file->tree_file_path,"r");

   if(fp_tree==NULL || fp_head==NULL) //do check if file exists.
   {
   	printf("ERROR: tree or head file does not exist");
	return -1;// return -1 if file is not opened.
   }

   //else continue.
   char *current_timestamp=(char *)malloc(15*sizeof(char));
   int current_offset;
   fscanf(fp_head,"%s %d",current_timestamp, &current_offset);
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
			if(parent_offset==offset) // if we find it even once then it means it is a junction.
	  			return 1;
		
			current_offset = parent_offset;  // move to the parent
		
		}while(parent_offset != -1);

   	}
   	fclose(fp_tree);
   	fclose(fp_head);
	return 0;	
}

int search_tp_tree(file_data * file, int off, int req_tp)
{
	printf("==================================================");
	printf("\n%d %d", off, req_tp);
	FILE * fp = fopen(file->tree_file_path, "r");
	if(fp==NULL)
	{
		printf("ERROR: TREE FILE DOESNOT EXIST\n");                                   //display as a error message
		return 0;
	}
	fseek(fp, off, SEEK_SET);
	int valid;
	int lp, diff_ct, p_off = off;
	fscanf(fp, "%d", &valid);
	if(valid==0)
	{
		printf("ERROR: no record of latest head in tree.");
		return 0;
	}
	char * temp_object = (char *)malloc(PATH_MAX*sizeof(char));
	strcpy(temp_object, file->objects_dir_path);
	char * curr_object = (char *)malloc(PATH_MAX*sizeof(char));
	strcpy(curr_object, file->objects_dir_path);
	int ctp;
	char * has = (char *)malloc(50*sizeof(char));
	fscanf(fp, "%d %d %s", &ctp, &lp, has);
	printf("\n===================================aaaaaaaaaaaaaaaaaeeeeeeee==========================\n%s", has);
	strcat(temp_object, "copy");
	strcat(curr_object, has);
	
	char *hash = (char*)malloc(40*sizeof(char));
	char *tag = (char*)malloc(255*sizeof(char));
	
	// create new file with filepath temp_object and copy 
	copy(curr_object, temp_object);	
	printf("\ncheck1 %d %d===================\n", ctp, req_tp);	
	while(ctp!=req_tp)
	{
		fscanf(fp, "%s %d %d", tag, &diff_ct, &p_off);
		printf("\n %s | %d | %d ||||||||||||||\n", tag, diff_ct, p_off);
		if(p_off==-1)
			return 0;
		fseek(fp, p_off, SEEK_SET);
		fscanf(fp, "%d", &valid);
		if(valid==0)
		{
			printf("ERROR: no record of latest head in tree.");
			return 0;
		}
		
		fscanf(fp, "%d %d %s", &ctp, &lp, hash);
		int i = strlen(curr_object) - 1;
		while(curr_object[i]!='/')
			i--;
		curr_object[i+1] = '\0';
		strcat(curr_object, hash);
			
		
		// check if the node is a loose object or not. if loose then dont take diff and store this object as the current ver and if not patch this po to previous object........................................
		if(lp==0)
		{
			// make this loose object the current object
			copy(curr_object, temp_object);	
		}
		else
		{
			// patch this pach object with current object and  uodate to current object
			patch(temp_object, curr_object);
		}
		
		printf("\t\t%d", ctp);
	}
	fclose(fp);
	printf("\n========================================\ncopy %s ----- to-----%s\n========================================", temp_object, file->path);
	copy(temp_object, file->path);
	copy(temp_object, curr_object);
	delete(temp_object);
	
	fp = fopen(file->tree_file_path, "r+");
	fseek(fp, p_off, SEEK_SET);
	int garbage;
	fscanf(fp, "%d", &garbage);
	fscanf(fp, "%d", &garbage);
	fprintf(fp, " 0");
	fclose(fp);
	return p_off;
}

int write_to_head(char * heads_file_path, int tp, int off)
{
	char * b_name = (char *)malloc(15*sizeof(char));
	strcpy(b_name, "B_");
	char *str_tp = (char*)malloc(20*sizeof(char));
	itoa(tp,str_tp);
	strcat(b_name, str_tp);
	printf("\n===============++++++++++++++++ %s ++++++++++++===========\n", b_name);
	
	FILE * fp = fopen(heads_file_path, "r+");
	if(fp==NULL)
	{
		printf("ERROR: HEAD FILE DOESNOT EXIST\n");                                   //display as a error message
		return 0;
	}
	char *str_off = (char*)malloc(20*sizeof(char));
	itoa(off,str_off);
	fprintf(fp, "%s ", b_name);
	fprintf(fp, "%s\n", addspaces(str_off, 10));
	fclose(fp);
	return 1;
}


int tp_from_branch_name(char * b_name)
{
	int len1=0;
	int len = strlen(b_name)-1;
	char * stp = (char *)malloc(12*sizeof(char));
	while(b_name[len]!='_')
	{
		stp[len1++] = b_name[len--];
	}
	stp[len1] = '\0';
	g_strreverse(stp);
	return (atoi(stp));
}

int get_tp_from_head(char * filepath, int tp)          //returns the offset if ver with this timestamp is one of the heads or returns -1 times tp of current head 
{
	printf("");
	FILE * fp = fopen(filepath, "r");
	if(fp==NULL)
		return 0;
	int ctp = 0, ret = 0, off = 0;
	char * tst = (char *)malloc(15*sizeof(char));
	fscanf(fp, "%s", tst);
	ctp = tp_from_branch_name(tst);
	//printf("======%d=====\n", ctp);
	fscanf(fp, "%d", &off);
	if(ctp==tp)
		return off;
	else
		ret = -1*off;
	while(fscanf(fp, "%s", tst)!=EOF)
	{
		//printf("==========\n");
		ctp = tp_from_branch_name(tst);
		//printf("======%d=====\n", ctp);
		fscanf(fp, "%d", &off);
		if(ctp==tp)
			return off;
		
	}
	fclose(fp);
	return ret;
}

void list_all_versions(const char * filepath);

/*void log_version_data(file_data *file, version_data *ver) {
	FILE *log = fopen(file->ver_log_path,"a");
	fprintf(log,"%d;%d;%d;%s\n",ver->number,ver->timestamp,ver->diff_lc,ver->tag);
	fclose(log);
}*/
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
		char *b_epoch = (char*)malloc(20*sizeof(char));
		strcpy(b_epoch,"B_");
		char *tmp = (char*)malloc(20*sizeof(char));
		itoa(ver->timestamp,tmp);
		strcat(b_epoch,tmp);
		deleteTop(file->heads_file_path);
		insertAtTop(file->heads_file_path,b_epoch,current_offset);
		
	}
}
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


// creates a new version of a file
void create_version(file_data * file,TreeMd * ver,int is_first_version) 
{
	//Constructing new version path
	char * new_current_ver = (char *) malloc((strlen(file->objects_dir_path)+50)*sizeof(char));
	sprintf(new_current_ver,"%s%s",file->objects_dir_path,ver->obj_hash);
	int is_creating_branch = 0;
	char *epoch = (char*)malloc(20*sizeof(char));
	FILE * f = fopen(file->heads_file_path,"r");
	fscanf(f,"%s",epoch);
	fclose(f);
	f = fopen(file->heads_file_path,"r");
	GHashTable *ght = g_hash_table_new(g_str_hash,g_str_equal);
	ght = readHTfromFile(f,2);
	fclose(f);
	if(g_hash_table_lookup(ght,epoch) != NULL)
	{
		is_creating_branch = 0;
		printf("\n000000000000000000000000000 NOT CREATING A NEW BRANCH0000000000000000000000\n");
	}
	else
	{
		printf("\n000000000000000000000000000 CREATING A NEW BRANCH0000000000000000000000\n");
		is_creating_branch = 1;
	}
	if((is_first_version)||(is_creating_branch))
	{
		copy(file->path,new_current_ver);	
			
	}
	else
	{
		char * old_ver_source = (char *)malloc(50 * sizeof(char));
		old_ver_source = get_hash_from_offset(ver->parent,file->tree_file_path);
	
		//Construct temp file to store previous version lo
		char * old_current_ver_dest = (char *) malloc( (strlen(file->objects_dir_path)+5) * sizeof(char) );
		sprintf(old_current_ver_dest,"%s%s",file->objects_dir_path,"old");
	
	
	
		//
		char * diff_path = (char *) malloc((strlen(file->objects_dir_path)+50) * sizeof(char));
		sprintf(diff_path,"%s%s",file->objects_dir_path,old_ver_source);

		#ifdef DEBUG
			printf("\told_current_ver_source: %s\n",old_ver_source);
			printf("\told_current_ver_dest: %s\n",old_current_ver_dest);
			printf("\tnew_current_ver: %s\n",new_current_ver);
		#endif

	
		//	move(old_current_ver_source,old_current_ver_dest);
			move(diff_path,old_current_ver_dest);
		
	
		copy(file->path,new_current_ver);
	
			#ifdef DEBUG
				char *command1 = (char *) malloc(2000 * sizeof(char));
				strcpy(command1,"cat ");
				strcat(command1,old_current_ver_dest);
				printf("\n\n---------------\n");
				system(command1);
				printf("---------------\n");

				char *command2 = (char *) malloc(2000 * sizeof(char));
				strcpy(command2,"cat ");
				strcat(command2,new_current_ver);
				printf("\n\n---------------\n");
				system(command2);
				printf("---------------\n");
			#endif
		
			diff(new_current_ver,old_current_ver_dest,diff_path);
			rem(old_current_ver_dest);
	/*	
			char ver_list_path[PATH_MAX];
			strcpy(ver_list_path,file->ver_dir_path);
			strcat(ver_list_path,"list");
			FILE *list = fopen(ver_list_path,"a");
			fprintf(list,"%s\n",diff_path);
			fclose(list);
	*/
			#ifdef DEBUG
				char *command3 = (char *) malloc(2000 * sizeof(char));
				strcpy(command3,"cat ");
				strcat(command3,old_current_ver_dest);
				printf("\n\n---------------\n");
				system(command3);
				printf("---------------\n");
			#endif

		// update the current version number in the current_ver_number file
	/*	FILE *current_ver_number = fopen(file->current_ver_number_path,"w");
		fprintf(current_ver_number,"%d",ver->number);
		fclose(current_ver_number);
*/	}
	// append the version data to the file
//	add_spaces_to_version_data(TreeMd * ver);
//	update_tree_data(file,ver);
	update_heads_file(file,ver,is_first_version,is_creating_branch);
	update_tree_data(file,ver,is_creating_branch,is_first_version);
	update_objmd_file(ver->obj_hash,file);
}
// constructs version data

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
// initializes file_data structure
void init_file_data(file_data * file) {
	file->name = (char *) malloc(PATH_MAX*sizeof(char));
	file->dir_path = (char *) malloc(PATH_MAX*sizeof(char));
	file->ver_dir_path = (char *) malloc(PATH_MAX*sizeof(char));
	file->objects_dir_path = (char *) malloc(PATH_MAX*sizeof(char));
	file->tree_file_path = (char *) malloc(PATH_MAX*sizeof(char));
	file->heads_file_path = (char *) malloc(PATH_MAX*sizeof(char));
	file->OBJ_MD_file_path = (char *) malloc(PATH_MAX*sizeof(char));
}

file_data * construct_file_data(const char * filepath) {
	file_data *file = (file_data *) malloc(sizeof(file_data));
	init_file_data(file);
	
	file->path = filepath;
	split_file_path(filepath,file->name,file->dir_path);
	
	// version directory path
	strcpy(file->ver_dir_path,file->dir_path);
	strcat(file->ver_dir_path,VER_DIR);
	//strcat(file->ver_dir_path,file->name);
	//strcat(file->ver_dir_path,"/");
	#ifdef DEBUG
		//printf("\n\t[versioning/construct_file_data] %s\n",file->ver_dir_path);
	#endif
	
	// objects folder path
	strcpy(file->objects_dir_path,file->ver_dir_path);
	strcat(file->objects_dir_path,OBJECTS_FOLDER);
	#ifdef DEBUG
		//printf("\n%s\n,file->objects_dir_path);
	#endif
	
	// tree file path
	strcpy(file->tree_file_path, file->ver_dir_path);
	strcat(file->tree_file_path, TREES_FOLDER);
	strcat(file->tree_file_path, file->name);
	strcat(file->tree_file_path, ".tree");
	
	// head file path
	strcpy(file->heads_file_path, file->ver_dir_path);
	strcat(file->heads_file_path, HEADS_FOLDER);
	strcat(file->heads_file_path, file->name);
	strcat(file->heads_file_path, ".head");
	
	// OBJ_MD file path
	strcpy(file->OBJ_MD_file_path,file->ver_dir_path);
	strcat(file->OBJ_MD_file_path,"OBJ_MD");
	
	return file;
}

int report_release(const char * filepath) {
	#ifdef DEBUG
		printf ("\n[versioning] received 'report_release for %s'\n", filepath);
	#endif
	int is_first_version = 0;
	// construct file data (i.e., path, name, directory path, version directory path, version log path, current version path, etc.)
	#ifdef DEBUG
		printf ("\n[versioning] constructing file data for %s...\n", filepath);
	#endif
	file_data *file = construct_file_data(filepath);
	#ifdef DEBUG
		print_file_data(file);
	#endif
	FILE * f = fopen(file->heads_file_path,"a");
	
	if(!ftell(f))
	{
		printf("Checking if first version\n");
		is_first_version = 1;
		#ifdef DEBUG
		printf("First Version Created\n");
		#endif
	}
	fclose(f);
	// construct latest version structure
	#ifdef DEBUG
		printf ("\n[versioning] constructing latest version data for %s...\n", filepath);
	#endif
	TreeMd *latest_version = construct_version_data(file,is_first_version);
	#ifdef DEBUG
		print_version_data(latest_version);
	#endif
	
	// create the new version itself
	#ifdef DEBUG
	printf ("\n[versioning] creating new version for %s...\n", filepath);
	#endif
	
	create_version(file,latest_version,is_first_version);
	
	#ifdef DEBUG
//		printf ("[versioning] created new version for file %s\n\n", filepath);
	#endif
	return 0;
}

int report_filedelete(const char * filepath) {
	#ifdef DEBUG
		printf ("[versioning] received 'report_filedelete for %s'\n", filepath);
	#endif

	// TODO : delete all files with names 'filepath*'

	return 0;
}

// debugging functions
void print_file_data(file_data * file) 
{
	printf("\tpath: %s\n",file->path);
	printf("\tname: %s\n",file->name);
	printf("\tdir_path: %s\n",file->dir_path);
	printf("\tver_dir_path: %s\n",file->ver_dir_path);
	printf("\tobjects_dir_path: %s\n",file->objects_dir_path);
	printf("\tTree file path: %s\n",file->tree_file_path);
	printf("\tHeads file path: %s\n",file->heads_file_path);
	printf("\tOBJ_MD file path: %s\n",file->OBJ_MD_file_path);
}

int report_checkout(char * filepath, int req_tp)
{
	log_msg("sndksa");
	log_msg("%s %d", filepath, req_tp);
	file_data * file = construct_file_data(filepath);
	printf("%s %d\n%s", filepath, req_tp, file->heads_file_path);
	int off = get_tp_from_head(file->heads_file_path, req_tp);
	printf("%%%%%%%%%% %d %%%%%%%%%%%%", off);
	if(off==0)
	{
		printf("ERROR: HEAD FILE DOESNOT EXIST\n");                                    //display as a error msg
		return 0;
	}
	if(off>0)
	{
		write_to_head(file->heads_file_path, req_tp, off);
		
		// make this object which is a loose object the current file  
		char * hash_target = (char *)malloc(50*sizeof(char));
		hash_target = get_hash_from_offset(off, file->tree_file_path);	
		char * target_file_path = (char *)malloc(PATH_MAX*sizeof(char));
		strcpy(target_file_path, file->objects_dir_path);
		strcat(target_file_path, hash_target);
		//copy the target_file_type to filepath.
		copy(target_file_path, file->path);
		
		return 1;
	}
	else
	{
		off = search_tp_tree(file, -1*off, req_tp);
		printf("---------------%d----------", off);
		if(off==0)
			return 0;
		write_to_head(file->heads_file_path, req_tp, off);
		return 1;	
	}
}
/*
void update_log_file(file_data * file,int reqd_version_no)
{
	FILE * f1 = fopen(file->ver_log_path,"r");
	char * path = (char *)malloc(PATH_MAX * sizeof(char));
	strcpy(path,file->ver_dir_path);
	strcat(path,"tmp");
	FILE * f2 = fopen(path,"w");
	int ver_no;
	char * logline = (char *)malloc(MAX_LOG_LINE_SIZE * sizeof(char));
	fgets(logline,MAX_LOG_LINE_SIZE,f1);
	while(logline!=NULL)
	{
	    ver_no = extract_version_no(logline);
	    printf("Version no is : %d\n",ver_no);
	    printf("Logline is %s\n",logline);
	    if(ver_no<=reqd_version_no)
	    fprintf(f2,"%s",logline);
	    else
	    break;
	  fgets(logline,MAX_LOG_LINE_SIZE,f1);  
	}
	//copy(path,file->ver_log_path);
	delete(file->ver_log_path);
	move(path,file->ver_log_path);
	fclose(f1);
	fclose(f2);
}*/
int revert_to_version(char * filepath, int req_tp)                     // returns 1 for success and 0 for failure
{
	file_data * file = construct_file_data(filepath);
	
	FILE * fph = fopen(file->heads_file_path, "r");
	char * b_name = (char *)malloc(15*sizeof(char));
	int off;
	fscanf(fph, "%s %d", b_name, &off);
	fclose(fph);	
	
	fph = fopen(file->heads_file_path, "r");
	GHashTable * ht = readHTfromFile(fph, 2);
	fclose(fph);

	if((int *)g_hash_table_lookup(ht, b_name)==NULL)
	{
		printf("ERROR: you cannot revert from the current version.");
		return 0;
	}	
	
	printf("-======================YAHAN TAK PAHUCH GYA===========================-\n");
	
	char * temp_object = (char *)malloc(PATH_MAX*sizeof(char));
	strcpy(temp_object, file->objects_dir_path);
	char * curr_object = (char *)malloc(PATH_MAX*sizeof(char));
	strcpy(curr_object, file->objects_dir_path);
	
	FILE * fpt = fopen(file->tree_file_path, "r+");
	fseek(fpt, off, SEEK_SET);
	int valid, tp, lp, diff_ct, p_off;
	char * hash = (char *)malloc(50*sizeof(char));
	char * tag = (char *)malloc(255*sizeof(char));
	fscanf(fpt, "%d", &valid);
	if(valid==0)
	{
		printf("ERROR: no record of latest head in tree.");
		return 0;
	}
	//fclose(fpt);
	fscanf(fpt, "%d %d %s %s %d %d", &tp, &lp, hash, tag, &diff_ct, &p_off);
	if(tp==req_tp)
	{
		printf("ERROR: you cannot revert to where you are");                   // display as a error message
		return 0;
	}
	
	strcat(curr_object, hash);
	strcat(temp_object, "copy");
	copy(curr_object, temp_object);
	printf("\n ===copy  %s ------> %s", curr_object, temp_object);
	printf("-======================YAHAN TAK PAHUCH GYA===========================-\n");
	
	
	while(tp!=req_tp)
	{
		fseek(fpt, off, SEEK_SET);
		fprintf(fpt, "0");
		char * bname = (char *)malloc(15*sizeof(char));
		strcpy(bname, "B_");
		char *str_tp= (char*)malloc(20*sizeof(char));
		itoa(tp,str_tp);
		strcat(bname, str_tp);
		printf("\npogasjgdkasgdlkgaskjdbajsgdhkjas\n%s", bname);
		g_hash_table_remove(ht, (gconstpointer)bname);
		printf("\n====%s===\n", hash);
		remove_from_everything(file, hash);                                // removes the current version objects folder if ref_count=1 
		break;
		/*off = p_off;
		fseek(fpt, off, SEEK_SET);
		fscanf(fpt, "%d", &valid);
		if(valid==0)
		{
			printf("ERROR: no record of parent version of the current version in tree.");
			return 0;
		}
		fscanf(fpt, "%d %d %s %s %d %d", &tp, &lp, hash, tag, &diff_ct, &p_off);
		
		fclose(fpt);
		if(isJunction(file, off)==1)
		{
			char * offs = (char *)malloc(15*sizeof(char));
			itoa(off, offs);
			fph = fopen(file->heads_file_path, "w");
			fprintf(fph, "B_%d %s\n", tp, addspaces(offs, 10));
			writeHTtoFile(ht, fph);
			fclose(fph);
			report_checkout(filepath, req_tp);
			return 1;
		}
		fpt = fopen(file->tree_file_path, "r+");
		int i = strlen(curr_object) - 1;
		while(curr_object[i]!='/')
			i--;
		curr_object[i] = '\0';
		strcat(curr_object, hash);
		if(lp==0)
			copy(curr_object, temp_object);
		else
			patch(temp_object, curr_object);*/
	}
	copy(temp_object, filepath);
	copy(temp_object, curr_object);
	delete(temp_object);
	fclose(fpt);
	return 1;
}
/*
void list_all_versions(const char * filepath)
{
	file_data * file = construct_file_data(filepath);
	FILE * f = fopen(file->ver_log_path,"r");
	char * logline = (char * )malloc(MAX_LOG_LINE_SIZE * sizeof(char));
	while(fgets(logline,MAX_LOG_LINE_SIZE,f)!=NULL)
	{
	//get_data_from_logline();
	printf("%s",logline);

	}
}*/
void add_spaces_to_version_data(TreeMd * ver)
{
	
//	printf("obj_hash : %s\n",ver->obj_hash);
//	printf("timestamp: %d\n",ver->timestamp);
//	printf("diff_count: %d\n",ver->diff_count);
//	printf("file_type: %d\n",ver->file_type);
//	printf("parent : %d\n",ver->parent);
//	printf("tag : %s\n",ver->tag);
}
void print_version_data(TreeMd * ver) 
{
	printf("Version Information\n");
	printf("Version valid : %d\n",ver->valid);
	printf("obj_hash : %s\n",ver->obj_hash);
	printf("timestamp: %d\n",ver->timestamp);
	printf("diff_count: %d\n",ver->diff_count);
	printf("file_type: %d\n",ver->file_type);
	printf("parent : %d\n",ver->parent);
	printf("tag : %s\n",ver->tag);
}
