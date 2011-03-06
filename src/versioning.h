#ifndef _VERSIONING_H_
#define _VERSIONING_H_

// the version directory name in each directory on the file system
#define VER_DIR ".ver/"
#define VER_LOG "log"
#define CURR_VER "current"
#define CURR_VER_NUMBER "current_ver_number"

/* Version information strct */
typedef struct version
{
	int number;
	/* id is required inspite of vnumber in the case 
	 * when a a deleted directory has to be reverted
	 * back. In this case only those files are revived
	 * which have same version_id as that of dir to be
	 * revived. Simply reviving latest versions wont
	 * work, because in the case when a file in the dir
	 * is deleted before dir was deleted that file will
	 * be wrongfully revived on reviving dir if reviving
	 * based on latest versions.
	 */
	char tag[512]; 
	int timestamp;
	int diff_lc; // line count of diff with previous version
} version;

typedef  version version_data;

typedef struct file_data_ {
	const char * path;	/* full file path */
	char * name;		/* file name */
	
	/* full path of the directory in which
	 * the file is stored
	 */
	char * dir_path;	
	
	/* full path of the version directory
	 * associated with the file
	 */
	char * ver_dir_path;							char * current_ver_path;				// full path of the current version copy inside the version directory
	char * ver_log_path;						// full path of the version log of the file
	char * current_ver_number_path;	// full path of the current version number file
} file_data;


/* Functions follow */
int report_write(const char * filepath);
int report_filedelete(const char * filepath);
struct version * get_versionlist(const char * filepath);
int changeto_version(const char * filepath, struct version v);
int report_release(const char * filepath);
int report_filedelete(const char * filepath);
int handle_filerename(const char * filepath);
struct version * get_versionlist(const char * filepath);
int changeto_version(const char * filepath, struct version v);
void print_file_data(file_data * file);
void print_version_data(version_data * ver);

#endif
