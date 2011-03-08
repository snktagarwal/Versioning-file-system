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

void report_checkout(const char * filepath,int reqd_version_no);
void list_all_versions(const char * filepath);

void log_version_data(file_data *file, version_data *ver) {
	FILE *log = fopen(file->ver_log_path,"a");
	fprintf(log,"%d;%d;%d;%s\n",ver->number,ver->timestamp,ver->diff_lc,ver->tag);
	fclose(log);
}

// creates a new version of a file
void create_version(file_data * file,version_data * ver) {
	char * old_current_ver_source = file->current_ver_path;
	char * old_current_ver_dest = (char *) malloc( (strlen(file->ver_dir_path)+5) * sizeof(char) );
	sprintf(old_current_ver_dest,"%s%s",file->ver_dir_path,"old");
	char * new_current_ver = file->current_ver_path;

	char * diff_path = (char *) malloc( (strlen(file->ver_dir_path)+5) * sizeof(char) );
	sprintf(diff_path,"%s%d",file->ver_dir_path,(ver->number)-1);

	#ifdef DEBUG
		printf("\told_current_ver_source: %s\n",old_current_ver_source);
		printf("\told_current_ver_dest: %s\n",old_current_ver_dest);
		printf("\tnew_current_ver: %s\n",new_current_ver);
	#endif
	
	if(ver->number != 1) {
		move(old_current_ver_source,old_current_ver_dest);
	}
	copy(file->path,new_current_ver);
	
	if(ver->number != 1) {
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
		
		char ver_list_path[PATH_MAX];
		strcpy(ver_list_path,file->ver_dir_path);
		strcat(ver_list_path,"list");
		FILE *list = fopen(ver_list_path,"a");
		fprintf(list,"%s\n",diff_path);
		fclose(list);

		#ifdef DEBUG
			char *command3 = (char *) malloc(2000 * sizeof(char));
			strcpy(command3,"cat ");
			strcat(command3,old_current_ver_dest);
			printf("\n\n---------------\n");
			system(command3);
			printf("---------------\n");
		#endif
	}
	// update the current version number in the current_ver_number file
	FILE *current_ver_number = fopen(file->current_ver_number_path,"w");
	fprintf(current_ver_number,"%d",ver->number);
	fclose(current_ver_number);
	
	// append the version data to the file
	log_version_data(file,ver);
}

// constructs version data
version_data * construct_version_data(file_data * file) {
	version_data *ver = (version_data *) malloc(sizeof(version_data));
	
	// version number
	if(!does_exist( file->ver_dir_path )) {
		return NULL;
	}
	else {
		FILE *current_ver_number = fopen(file->current_ver_number_path,"r");
		fscanf(current_ver_number,"%d",&(ver->number));
		(ver->number)++;
		fclose(current_ver_number);
	}
	
	// tag ---- TODO
	strcpy(ver->tag, "");
	
	// timestamp
	ver->timestamp = (int) time(NULL);
	
	// diff_lc ----- TODO
	ver->diff_lc = 0;
	
	return ver;
}

// initializes file_data structure
void init_file_data(file_data * file) {
	file->name = (char *) malloc(PATH_MAX*sizeof(char));
	file->dir_path = (char *) malloc(PATH_MAX*sizeof(char));
	file->ver_dir_path = (char *) malloc(PATH_MAX*sizeof(char));
	file->ver_log_path = (char *) malloc(PATH_MAX*sizeof(char));
	file->current_ver_path = (char *) malloc(PATH_MAX*sizeof(char));
	file->current_ver_number_path = (char *) malloc(PATH_MAX*sizeof(char));
}

// constructs file data
file_data * construct_file_data(const char * filepath) {
	file_data *file = (file_data *) malloc(sizeof(file_data));
	init_file_data(file);
	
	file->path = filepath;
	split_file_path(filepath,file->name,file->dir_path);
	
	// version directory path
	strcpy(file->ver_dir_path,file->dir_path);
	strcat(file->ver_dir_path,VER_DIR);
	strcat(file->ver_dir_path,file->name);
	strcat(file->ver_dir_path,"/");
	#ifdef DEBUG
		//printf("\n\t[versioning/construct_file_data] %s\n",file->ver_dir_path);
	#endif
	
	// version log path
	strcpy(file->ver_log_path,file->ver_dir_path);
	strcat(file->ver_log_path,VER_LOG);
	#ifdef DEBUG
		//printf("\n%s\n,file->ver_log_path);
	#endif
	
	// current version path
	strcpy(file->current_ver_path,file->ver_dir_path);
	strcat(file->current_ver_path,CURR_VER);
	
	// current version number file path
	strcpy(file->current_ver_number_path,file->ver_dir_path);
	strcat(file->current_ver_number_path,CURR_VER_NUMBER);
	
	return file;
}

int report_release(const char * filepath) {
	#ifdef DEBUG
		printf ("\n[versioning] received 'report_release for %s'\n", filepath);
	#endif
	
	// construct file data (i.e., path, name, directory path, version directory path, version log path, current version path, etc.)
	#ifdef DEBUG
		printf ("\n[versioning] constructing file data for %s...\n", filepath);
	#endif
	file_data *file = construct_file_data(filepath);
	#ifdef DEBUG
		print_file_data(file);
	#endif
	
	// construct latest version structure
	#ifdef DEBUG
		printf ("\n[versioning] constructing latest version data for %s...\n", filepath);
	#endif
	version_data *latest_version = construct_version_data(file);
	#ifdef DEBUG
		print_version_data(latest_version);
	#endif
	
	// create the new version itself
	#ifdef DEBUG
		printf ("\n[versioning] creating new version for %s...\n", filepath);
	#endif
	create_version(file,latest_version);
	
	#ifdef DEBUG
		printf ("[versioning] created new version for file %s\n\n", filepath);
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
void print_file_data(file_data * file) {
	printf("\tpath: %s\n",file->path);
	printf("\tname: %s\n",file->name);
	printf("\tdir_path: %s\n",file->dir_path);
	printf("\tver_dir_path: %s\n",file->ver_dir_path);
	printf("\tcurrent_ver_path: %s\n",file->current_ver_path);
	printf("\tver_log_path: %s\n",file->ver_log_path);
	printf("\tcurrent_ver_number_path: %s\n",file->current_ver_number_path);
}

void report_checkout(const char * filepath,int reqd_version_no)
{
	file_data * file = construct_file_data(filepath);

	char * diff_filepath;

	char * patchedtmp_filepath = (char *) malloc(PATH_MAX*sizeof(char));
	char * new_version_filepath = (char *) malloc(PATH_MAX*sizeof(char));

	strcpy(patchedtmp_filepath,file->ver_dir_path);
	strcat(patchedtmp_filepath,"patchedtmp");
	copy(file->current_ver_path,patchedtmp_filepath);
	int ver_no = get_current_version_no(file);
	//new_version_filepath = get_file_verno_path(file,ver_no + 1);
	while(ver_no>reqd_version_no)
	{
		#ifdef DEBUG
		printf("Version NO : %d\n",ver_no);
	//	printf("Original file path : %s\n",orig_filepath);
		#endif
		ver_no--;
		diff_filepath = get_file_verno_path(file,ver_no);
		patch(patchedtmp_filepath,diff_filepath);
	//	getchar();
	//	getchar();
	}

	printf("**********File version No : %d\n",ver_no);
	printf("Checkout version filepath : %s\n",get_file_verno_path(file,ver_no));
	//copy(get_file_verno_path(file,ver_no),new_version_filepath);
	copy(patchedtmp_filepath,filepath);

	report_release(filepath);

	printf("Patched tmp filepath : %s\n",patchedtmp_filepath);
	delete(patchedtmp_filepath);
}
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
}
void revert_to_version(const char * filepath,int reqd_version_no)
{
	int i;
	char * path = (char *) malloc(PATH_MAX*sizeof(char));
	report_checkout(filepath,reqd_version_no);
	file_data * file = construct_file_data(filepath);
/*	FILE * current_ver_number = fopen(file->current_ver_number_path,"w");
	fprintf(current_ver_number,"%d",reqd_version_no);
	fclose(current_ver_number);*/
	for(i = reqd_version_no;i<=(get_current_version_no(file) + 1);i++)
	{
	 path = get_file_verno_path(file,i);
	 printf("Going to delete : %s\n",path);
	 if(does_exist(path))
		delete(path);
	}
	FILE * current_ver_number = fopen(file->current_ver_number_path,"w");
	fprintf(current_ver_number,"%d",reqd_version_no);
	fclose(current_ver_number);
	update_log_file(file,reqd_version_no);
}
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
}

void print_version_data(version_data * ver) {
	printf("\tnumber: %d\n",ver->number);
	printf("\ttag: %s\n",ver->tag);
	printf("\ttimestamp: %d\n",ver->timestamp);
	printf("\tdiff_lc: %d\n",ver->diff_lc);
}
