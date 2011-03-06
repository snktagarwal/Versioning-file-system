#ifndef _VERSIONING_H_
#define _VERSIONING_H_

int report_release(const char * filepath);
int report_filedelete(const char * filepath);
int handle_filerename(const char * filepath);
//int report_dirdelete(const char * dirpath); // recursively deletes all files under dirpath

struct version * get_versionlist(const char * filepath);

int changeto_version(const char * filepath, struct version v);

typedef struct version_data_ {
	int number;
	char *tag; // id is required inspite of vnumber in the case when a a deleted directory has to be reverted back. In this case only those files are revived which have same version_id as that of dir to be revived. Simply reviving latest versions wont work, because in the case when a file in the dir is deleted before dir was deleted that file will be wrongfully revived on reviving dir if reviving based on latest versions.
	int timestamp;
	int diff_lc; // line count of diff with previous version
} version_data;

typedef struct file_data_ {
	const char * path;							// full file path
	char * name;										// file name
	char * dir_path;								// full path of the directory in which the file is stored
	char * ver_dir_path;						// full path of the version directory associated with the file
	char * current_ver_path;				// full path of the current version copy inside the version directory
	char * ver_log_path;						// full path of the version log of the file
	char * current_ver_number_path;	// full path of the current version number file
} file_data;

void print_file_data(file_data * file);
void print_version_data(version_data * ver);

#endif
