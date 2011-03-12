#ifndef _VERSIONING_H_
#define _VERSIONING_H_

// the version directory name in each directory on the file system
#define VER_DIR ".ver/"
#define VER_LOG "log"
#define CURR_VER "current"
#define CURR_VER_NUMBER "current_ver_no"
#define OBJECTS_FOLDER "objects/"
#define TREES_FOLDER "trees/"
#define HEADS_FOLDER "heads/"
#define OBJ_MD "OBJ_MD"
#define HASH_SHA1 41
#define MAX_TAG 255
#define MAX_BNAME MAX_TAG
#define LO 0
#define PO 1
//#define OBJ_MD_file_path "OBJ_MD"
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
	char * ver_dir_path;						
	char * objects_dir_path;				// full path of the objects folder inside the version directory
	char * tree_file_path;					// full path of the tree record of the file in .ver
	char * heads_file_path;					// full path of the heads record of the file in .ver
	char * OBJ_MD_file_path;
	// full path of the OBJ_MD file in .ver
} file_data;


/* New versioning Data Structures */

// Structure to be written onto tree/file.tree

typedef struct _tree_md{
	
	int valid;	/* Tells whether this structure is valid row */
	char obj_hash[HASH_SHA1];
	char tag[MAX_TAG];
	int diff_count;
	int timestamp;
	int file_type; /* LO/PO */
	int parent;    /* Parent offset in the file */
}TreeMd;

// Structure to be written to .ver/OBJ_MD

typedef struct _obj_md{
	
	int ref_count;
	char obj_hash[HASH_SHA1];
}ObjMd;

// Structure to be written to .ver/heads/file.heads

typedef struct _heads_data{
	
	int tree_offset;
	char b_name[MAX_BNAME];
}HeadsData;


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
void print_version_data(TreeMd * ver);
void list_all_versions(const char * filepath);
int report_checkout(char * filepath,int version_no);
int revert_to_version(char * filepath,int reqd_version_no);
void update_objmd_file(char * s1,file_data * file);

#endif
