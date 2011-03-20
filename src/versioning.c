#define DEBUG
#define DELAY_TIME (10)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <fuse.h>

#include "vfs.h"
#include "fuse_wrapper.h"
	
#include "params.h"
#define printf log_msg
/**
* logs version data in the following format:
* <version_number>;<timestamp>;<diff_lc>;<tag>
*/

/* Deprecated */


/* ------- Deprecated -------- */

int report_checkout(char * filepath, int req_tp);

int remove_from_everything(file_data * file, char * hash)
{
	FILE * fp = fopen(file->OBJ_MD_file_path, "r+");
	/*GHashTable * ht = readHTfromFile(fp, 1);
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
	*/
	//int ref_ct = *pointer_ref_ct;
	
	char * hash1 = (char *)malloc(50*sizeof(char));
	char * new_str = (char *)malloc(10000*sizeof(char));
	char * temp = (char *)malloc(60*sizeof(char));
	int ref;
	int flag = 0;
	int pos = ftell(fp);
	fscanf(fp, "%s %s", hash1, temp);
	
	while(strcmp(hash, hash1)!=0)
	{
		pos = ftell(fp);
		if(flag==0)
			strcpy(new_str, hash1);
		else
	 	strcat(new_str, hash1);
		strcat(new_str, " ");
		if(flag==0)
			strcat(new_str, temp);
		else
		{
			char * tmper = (char *)malloc(5*sizeof(char));
			itoa(ref, tmper);
			strcat(new_str, tmper);
		}
		strcat(new_str, "\n");
		flag++;
		if(fscanf(fp, "%s %d", hash1, &ref)==EOF)
		{
			printf("IMPOSSIBLE\n");
			return 0;
		}	
	}
	printf("===============done%d\n", ref);
	if(ref==1)
	{
		printf("==========CAME HERE\n");
		//if()
		while(fscanf(fp, "%s %s", hash1, temp)!=EOF);
		{
			printf("%s %s\n", hash1, temp);
			if(strcmp(hash, hash1)!=0)
			{
				if(flag==0)
					strcpy(new_str, hash1);
				else
				 	strcat(new_str, hash1);
				strcat(new_str, " ");
				strcat(new_str, temp);
				strcat(new_str, "\n");
				printf("%s\n", new_str);
				flag++;
			}
			printf("=============================aaaaaaaaaaiiilaaaaaaaaaaaaaaaaaaaaaaaa============\n");
		}
		printf("=============================aaaaaaaaaaiiilaaaaaaaaaaaaaaaaaaaaaaaa============\n");
		fclose(fp);
		printf("========%s\n", file->OBJ_MD_file_path);
		fp = fopen(file->OBJ_MD_file_path, "w");
		fprintf(fp, "%s", new_str);
		printf("%s", new_str);
		fclose(fp);
		printf("=============================aaaaaaaaaaiiilaaaaaaaaaaaaaaaaaaaaaaaa============\n");
		char * obj_path = (char *)malloc(PATH_MAX*sizeof(char));
		strcpy(obj_path, file->objects_dir_path);
		strcat(obj_path, hash);
		printf("============removing:: %s\n", obj_path);
		remove(obj_path);
		printf("=============================aaaaaaaaaaiiilaaaaaaaaaaaaaaaaaaaaaaaa============\n");
		return 1;
	}
	fseek(fp, pos, SEEK_SET);
	ref--;
	fscanf(fp, "%s", hash1);
	fprintf(fp, " %d\n", ref);
	//fseek()
	/*char * hash1 = (char *)malloc(50*sizeof(char));
	int ref;
	GHashTable * ht = g_hash_table_new(g_str_hash, g_str_equal);
	while(fscanf(fp, "%s %d", hash1, &ref)!=EOF)
		g_hash_table_insert(hash, hash1, "1");
	fclose(fp);
	printf("There are %d keys in the hash\n", g_hash_table_size(hash));
	printf("The capital of Texas is %d\n", g_hash_table_lookup(hash, hash));
	*/
	/*if(ref_ct==1)
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
	*/fclose(fp);
	printf("\n=================EXITING FROM REMOVE FROM EVERYTHING==========\n");
	return 1;
}


int search_tp_tree(file_data * file, int off, int req_tp)
{
	printf("==================================================");
	printf("\n%d %d", off, req_tp);
	FILE * fp = fopen(file->tree_file_path, "r");
	if(fp==NULL)
	{
		printf("ERROR: TREE FILE DOESNOT EXIST\n");                                   //display as a error message
		return -1;
	}
	fseek(fp, off, SEEK_SET);
	int valid;
	int lp, diff_ct, p_off = off;
	fscanf(fp, "%d", &valid);
	if(valid==0)
	{
		printf("ERROR: no record of latest head in tree.");
		return -1;
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
	decompress(curr_object);	
	copy(curr_object, temp_object);	
	compress(curr_object);
	printf("\ncheck1 %d %d===================\n", ctp, req_tp);	
	while(ctp!=req_tp)
	{
		fscanf(fp, "%s %d %d", tag, &diff_ct, &p_off);
		printf("\n %s | %d | %d ||||||||||||||\n", tag, diff_ct, p_off);
		if(p_off==-1)
			return -1;
		fseek(fp, p_off, SEEK_SET);
		fscanf(fp, "%d", &valid);
		if(valid==0)
		{
			printf("ERROR: no record of latest head in tree.");
			return -1;
		}
		
		fscanf(fp, "%d %d %s", &ctp, &lp, hash);
		int i = strlen(curr_object) - 1;
		while(curr_object[i]!='/')
			i--;
		curr_object[i+1] = '\0';
		strcat(curr_object, hash);
			
		decompress(curr_object);
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
		compress(curr_object);
		printf("\t\t%d", ctp);
	}
	fclose(fp);
	printf("\n========================================\ncopy %s ----- to-----%s\n========================================", temp_object, file->path);
	copy(temp_object, file->path);
	decompress(curr_object);
	copy(temp_object, curr_object);
	delete(temp_object);
	compress(curr_object);
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

/* Updates the metadata size in <file>.md
 */
void update_sizemd_file(file_data *file, int timestamp)
{
	int md_size = calc_md_size(file);
	int file_size = calc_file_size(file->path);
	float ratio = file_size/(float)(md_size+file_size);
	log_msg("file size: %d ----- metadata size: %d totalsize = %d\n",file_size,md_size,md_size+file_size);
	FILE *fp;
	fp = fopen(file->md_data_file_path,"a");
	fprintf(fp,"%d %d %d %f\n",timestamp,file_size,md_size,ratio);
	fclose(fp);
}


/* Creates a version on report file release
 * It calls to update the TREE, HEADS and OBJ_MD
 */
void compress(char *obj_file){

	
	///* Assuming that the file is available */

	char mv_obj[255], command[400];;
	char * filename = (char *)malloc(PATH_MAX * sizeof(char));
	char * dirpath = (char *)malloc(PATH_MAX * sizeof(char));
	split_file_path(obj_file,filename,dirpath);
	strcpy(mv_obj, filename);
	strcat(mv_obj, ".tmp");

	/* Now compress */

	sprintf(command, "cd %s ; mv %s %s; tar -czvf %s %s; rm %s",dirpath,filename, mv_obj,filename, mv_obj, mv_obj);
	log_msg("Compressing: %s\n", command);
	system(command);

	/* Log that you compressed the file */

	log_msg("Compressed the Obj File: %s\n", obj_file);

}

/* De-Compresses a given filename using the gzipped format */
void decompress(char *obj_file){
	
	/* Assuming that the file is available */

	char mv_obj[255], command[400];;
	char * filename = (char *)malloc(PATH_MAX * sizeof(char));
	char * dirpath = (char *)malloc(PATH_MAX * sizeof(char));
	split_file_path(obj_file,filename,dirpath);

	strcpy(mv_obj, filename);
	strcat(mv_obj, ".tmp");

	/* Now compress */

	sprintf(command, "cd %s; tar -xzvf %s; rm %s; mv %s %s;", dirpath, filename, filename,  mv_obj, filename);
	log_msg("DeCompressing: %s\n", command);
	system(command);

	/* Log that you compressed the file */

	log_msg("De-Compressed the Obj File: %s\n", obj_file);
}
void create_version(file_data * file,TreeMd * ver,int is_first_version) 
{
	int off;
	//Constructing new version path
	char * new_current_ver = (char *) malloc((strlen(file->objects_dir_path)+50)*sizeof(char));
	sprintf(new_current_ver,"%s%s",file->objects_dir_path,ver->obj_hash);
	int is_creating_branch =  1;
	char * epoch = (char*)malloc(20*sizeof(char));
	char * tmp_epoch = (char*)malloc(20*sizeof(char));
	FILE * f = fopen(file->heads_file_path,"r");
	fscanf(f,"%s",epoch);
	fscanf(f,"%d",&off);
	//	fclose(f);
//	-------------------------------------------------------------------------------------------------------------------------
	
	while(fscanf(f,"%s %d",tmp_epoch,&off) != EOF)
	{
		if(strcmp(tmp_epoch,epoch) == 0)
		{
			is_creating_branch = 0;	
		}
	}
	
	
	
//	--------------------------------------------------------------------------------------------------------------------------------------------
/*	f = fopen(file->heads_file_path,"r");
	GHashTable *ght = g_hash_table_new(g_str_hash,g_str_equal);
	ght = readHTfromFile(f,2);*/
	fclose(f);
	if(is_creating_branch)
		printf("=====================CREATING A NEW BRANCH========================================\n");
	else
		printf("=====================NOT Creating a new branch====================================\n");
	/*if(g_hash_table_lookup(ght,epoch) != NULL)
	{
		is_creating_branch = 0;
		printf("\n000000000000000000000000000 NOT CREATING A NEW BRANCH0000000000000000000000\n");
	}
	else
	{
		printf("\n000000000000000000000000000 CREATING A NEW BRANCH0000000000000000000000\n");
		is_creating_branch = 1;
	}*/
	if((is_first_version)||(is_creating_branch))
	{
		
		copy(file->path,new_current_ver);
		printf("Compressing %s\n",new_current_ver);	
		compress(new_current_ver);
			
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

			printf("DeCompressing %s\n",diff_path);	
			decompress(diff_path);
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
			printf("Compressing %s\n",diff_path);	
			compress(diff_path);
			printf("Compressing %s\n",new_current_ver);
			compress(new_current_ver);	
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
	update_objmd_file(ver->obj_hash,file->OBJ_MD_file_path,0);
	update_sizemd_file(file,ver->timestamp);	
}
// constructs version data


// initializes file_data structure
void init_file_data(file_data * file) {
	file->name = (char *) malloc(PATH_MAX*sizeof(char));
	file->dir_path = (char *) malloc(PATH_MAX*sizeof(char));
	file->ver_dir_path = (char *) malloc(PATH_MAX*sizeof(char));
	file->objects_dir_path = (char *) malloc(PATH_MAX*sizeof(char));
	file->tree_file_path = (char *) malloc(PATH_MAX*sizeof(char));
	file->heads_file_path = (char *) malloc(PATH_MAX*sizeof(char));
	file->OBJ_MD_file_path = (char *) malloc(PATH_MAX*sizeof(char));
	file->md_data_file_path = (char *) malloc(PATH_MAX*sizeof(char));
}

/* Constructs the paths of:
 * tree
 * heads
 * OBJ_MD
 * returns the data structure
 */
 
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
	
	// md_data file path
	strcpy(file->md_data_file_path,file->ver_dir_path);
	strcat(file->md_data_file_path,MD_DATA_FOLDER);
	strcat(file->md_data_file_path,file->name);
	strcat(file->md_data_file_path,".md");
	
	return file;
}

/* Called from FUSE over writing */

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
		//print_version_data(latest_version);
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

/* Logic of checking out */

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
		decompress(target_file_path);
		copy(target_file_path, file->path);
		compress(target_file_path);
		
		return 1;
	}
	else
	{
		off = search_tp_tree(file, -1*off, req_tp);
		printf("---------------%d----------", off);
		if(off==-1)
			return 0;
		write_to_head(file->heads_file_path, req_tp, off);
		return 1;	
	}
}

/* Revert Logic */

int make_write(file_data * file, char * write)
{
	FILE * fph = fopen(file->heads_file_path, "r");
	char * b_name = (char *)malloc(15*sizeof(char));
	int off;
	fscanf(fph, "%s %d", b_name, &off);
	char * t = (char *)malloc(15*sizeof(char));
	char * t1 = (char *)malloc(15*sizeof(char));
	int flg = 0;
	while(fscanf(fph, "%s %s", t, t1)!=EOF)
	{
		if(strcmp(t, b_name)!=0)
		{
			if(flg==0)
				strcpy(write, t);
			else
				strcat(write, t);
			strcat(write, " ");
			strcat(write, addspaces(t1, 10));
			strcat(write, "\n");
			flg++;
		}
	}
	fclose(fph);
	return off;	
}

int check_feasibilty(file_data * file, int req_tp, char * curr_bname)
{
	strcpy(curr_bname, "B_");
	char * rtp = (char *)malloc(15*sizeof(char));
	itoa(req_tp, rtp);
	strcat(curr_bname, rtp);
	FILE * fph = fopen(file->heads_file_path, "r");
	char * c_bname = (char *)malloc(15*sizeof(char));
	int c_tp;
	fscanf(fph, "%s %d", c_bname, &c_tp);
	int flag1 = 0;
	while(strcmp(c_bname, curr_bname)!=0)
	{
		if(fscanf(fph, "%s %d", c_bname, &c_tp)==EOF)
		{
			flag1=1;
			break;
		}
	}
	fclose(fph);
	if(flag1==0)
	{
		printf("ERROR: YOU CANNOT REVERT TO THIS POSITION.\n");
		return 0;	
	}
	return 1;
}

int revert_to_version(char * filepath, int req_tp)                     // returns 1 for success and 0 for failure
{
	file_data * file = construct_file_data(filepath);
	char * write = (char *)malloc(10000*sizeof(char));
	FILE * fph = fopen(file->heads_file_path, "r");
	int off = make_write(file, write);
	
	char * curr_bname = (char *)malloc(15*sizeof(char));
	if(check_feasibilty(file, req_tp, curr_bname)==0)
		return 0;
		
	char * c_tp = (char *)malloc(15*sizeof(char));
	fscanf(fph, "%s", c_tp);
	c_tp = c_tp + 2;
	
	if(report_checkout(filepath, req_tp)==0)
	{
		printf("ERROR: YOU CANNOT REVERT TO THIS VERSION\n");
		return 0;
	}
	else
		report_checkout(filepath, atoi(c_tp));
	
	char * temp_object = (char *)malloc(PATH_MAX*sizeof(char));
	strcpy(temp_object, file->objects_dir_path);
	char * curr_object = (char *)malloc(PATH_MAX*sizeof(char));
	strcpy(curr_object, file->objects_dir_path);
	
	int valid, tp, lp, diff_ct, p_off;
	char * hash = (char *)malloc(50*sizeof(char));
	char * tag = (char *)malloc(255*sizeof(char));
	
	FILE * fpt = fopen(file->tree_file_path, "r+");
	char * root_bname = (char *)malloc(15*sizeof(char));
	strcpy(root_bname, "B_");
	fscanf(fpt, "%d", &valid);
	if(valid==0)
	{
		printf("ERROR: no record of latest head in tree.");
		return 0;
	}
	
	fscanf(fpt, "%d %d %s %s %d %d", &tp, &lp, hash, tag, &diff_ct, &p_off);
	char * tmper = (char *)malloc(15*sizeof(char));
	itoa(tp, tmper);
	strcat(root_bname, tmper);
	log_msg("==================%s=========\n", root_bname);
	fseek(fpt, off, SEEK_SET);
	
	fscanf(fpt, "%d", &valid);
	if(valid==0)
	{
		printf("ERROR: no record of latest head in tree.");
		return 0;
	}
	
	fscanf(fpt, "%d %d %s %s %d %d", &tp, &lp, hash, tag, &diff_ct, &p_off);
	if(tp==req_tp)
	{
		printf("ERROR: you cannot revert to where you are");                   // display as a error message
		return 0;
	}
	
	strcat(curr_object, hash);
	strcat(temp_object, "copy");
	decompress(curr_object);
	copy(curr_object, temp_object);
	compress(curr_object);
	while(tp!=req_tp)
	{
		fseek(fpt, off, SEEK_SET);
		fprintf(fpt, "0");
		remove_from_everything(file, hash);                                // removes the current version objects file if ref_count=1 
		off = p_off;
		fseek(fpt, off, SEEK_SET);
		fscanf(fpt, "%d", &valid);
		if(valid==0)
		{
			printf("ERROR: no record of parent version of the current version in tree.");
			return 0;
		}
		fscanf(fpt, "%d %d %s %s %d %d", &tp, &lp, hash, tag, &diff_ct, &p_off);
		fclose(fpt);
		if(isJunction(file, off)==1 && off!=0)
		{
			char * offs = (char *)malloc(15*sizeof(char));
			itoa(off, offs);
			fph = fopen(file->heads_file_path, "w");
			fprintf(fph, "B_%d %s\n", tp, addspaces(offs, 10));
			fprintf(fph, "%s", write);
			fclose(fph);
			report_checkout(filepath, req_tp);
			return 1;
		}
		fpt = fopen(file->tree_file_path, "r+");
		int i = strlen(curr_object);
		while(curr_object[i]!='/')
			i--;
		curr_object[i+1] = '\0';
		strcat(curr_object, hash);
		decompress(curr_object);
		if(lp==0)
		{
			
			copy(curr_object, temp_object);
			

		}
		else
		{
			
			patch(temp_object, curr_object);
			
		}
	}
	compress(curr_object);
	copy(temp_object, filepath);
	decompress(curr_object);
	copy(temp_object, curr_object);
	delete(temp_object);
	compress(curr_object);
	fclose(fpt);
	fph = fopen(file->heads_file_path, "w");
	char * offs = (char *)malloc(15*sizeof(char));
	itoa(off, offs);
	log_msg("-----------%s----%s-----", root_bname, curr_bname);
	fprintf(fph, "%s %s\n", curr_bname, addspaces(offs, 10));
	if(strcmp(curr_bname, root_bname)!=0)
		fprintf(fph, "%s %s\n", curr_bname, offs);
	fprintf(fph, "%s", write);
	fclose(fph);
	return 1;
}
