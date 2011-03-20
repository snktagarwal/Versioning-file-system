/* vfs.h
 * Defines the basic data structure and functions defined for the versioning system
 * does not cover functionality for versioning itself, rather the basic of when
 * to do versioning
 * Author: Sanket Agarwal <sanket@sanketagarwal.com
 */

#include<fuse.h>
#include<glib.h>

#include "log.h"
#include "fuse_wrapper.h"

#define MAX_BLOCKS 100
#define HASH_SHA1 41
#define MAX_TAG 255
#define MAX_BNAME MAX_TAG
#define LO 0
#define PO 1
#define VER_DIR ".ver/"
#define VER_LOG "log"
#define CURR_VER "current"
#define CURR_VER_NUMBER "current_ver_no"
#define OBJECTS_FOLDER "objects/"
#define TREES_FOLDER "trees/"
#define HEADS_FOLDER "heads/"
#define OBJ_MD "OBJ_MD"
#define MD_DATA_FOLDER "md_data/"

/* Define the types of data that may be stored
 */

#define WRITE_INFO 1

/* These structures might elope once the new
 * versioning system is in place
 */
// Information for a block

typedef struct _block_info{
	
	int offset;
	int size;
}BlockInfo;

// Encapsulates the write info
typedef struct _write_info{
	
	char full_path[100];
	GSList *b_info;
}WriteInfo;
	
// Versioning info MASTER structure
typedef struct _ver_info{
	GSList *ll_wi;	/* Write info list */
}VerInfo;

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
	char * md_data_file_path;
	// full path of the OBJ_MD file in .ver
} file_data;

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

/* Data Structure used in the linked list of merging policy*/

typedef struct node_mergedata_ {
				int current_offset;			// Offset of node to be deleted/mergedata. Can be used as a pointer to it  
				int child_offset;
				int current_timestamp;
				int child_timestamp;	
				//int child_type;		// Type of child of node to be deleted/mergedata
				int current_type;		// Type of node to be deleted/mergedata
				int parent_type;		// Type of parent of node to be deleted/mergedata
				float mergedata;		// #diff/delta(t)
				int nearest_lo_offset;
			} node_mergedata;

/* type is defined as 
 * 1 = PO + untagged
 * 2 = PO + tagged
 * 3 = LO + untagged
 * 4 = LO + tagged
*/ 
 

typedef struct node_head_list{
		int offset;
		} node_head_list;

/* Deprecate them */
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

void writeHTtoFile(GHashTable *ht,FILE *f);
GHashTable *readHTfromFile(FILE *f,int i);
void deleteTop(char *filepath);
int find_present_file_offset(char * filepath); /* Make MACRO */
char * addspaces(char *str, int size);
/* Deprecate them */

/* Function prototypes */
gint GCompareWI(gconstpointer a, gconstpointer b);
void add_write_info(VerInfo ver, const char* path,size_t size,off_t offset);
void remove_write_info(VerInfo ver, const char *path);
void version_file(VerInfo ver, const char *path);
char *get_log_file_name(char *filepath);
char *get_log_file_name_new(char *filepath);
char *get_file_name(char *filepath, char filename[PATH_MAX]);
void get_file_name_new(char *dirpath, char *filename, const char* path);
void itoa(int n,char s[]);
void reverse(char s[]);
void itoa(int n, char str[]);

/* VFS Utils */
void find_SHA(const char * filepath, char c[41]);
void split_file_path (const char * filepath, char *filename, char *dirpath);
char * get_hash_from_offset(int offset, char  * file_tree_path);
int get_present_head_offset(char * filepath);
int get_current_offset();
file_data * construct_file_data(const char * filepath);

/* Debug functions */
void print_file_data(file_data * file);

/* Versioning specific constructs */
int report_release(const char * filepath);
void create_version(file_data * file,TreeMd * ver,int is_first_version);
int report_checkout(char * filepath, int req_tp);
int revert_to_version(char * filepath, int req_tp);
void heads_in_else(char *filepath , char * current_epoch,int current_offset,char * tmp_filepath);

/* Functions relevant to Tree file handling */

void update_tree_data(file_data *file,TreeMd *ver,int is_creating_branch,int is_first_version);
int isJunction(file_data *file,int offset);
TreeMd * construct_version_data(file_data * file,int is_first_version);

/* Functions relevant to Heads file handling */

void replaceTop(char *filepath, char *epoch, int off);
int write_to_head(char * heads_file_path, int tp, int off);
void update_heads_file(file_data  * file,TreeMd * ver,int is_first_version,int is_creating_branch);

/* Functions relevant to Obj_Md file handling */

void update_objmd_file(char * s1,char * obj_md_path, int mode);

/*Functions related to file cleanup*/

/* Size calculating functions */

int calc_file_size(char *fpath);			/* Calculates the size of the file */
int calc_obj_size(file_data * file);		/* Calculates the cumulative size of all objects related to an object, takes as param a struct file_data */
int calc_md_size(file_data * file);			/* Calculates the cumulative size of all metadata related to an object (objects + .head + .tree), takes as param a struct file_data */

void compare_diff_vs_time(file_data *file);	


/* Functions used in handling the linklist */

int type(int,int);
void print_iterator(gpointer item);
void print_head_iterator(gpointer item);
GSList *move_to_lo_iterator(int,FILE *, GSList *,GSList *);
gint compare_node_mergedata(gconstpointer item1, gconstpointer item2);
GSList* generate_ll(file_data*,GSList*);
GSList* generate_ll_old(file_data*,GSList*);
void cleanFile(char *filepath, double ratio);
void cleanupfile(file_data *file,float reqdratio);

/* Functions for formatted output of versions*/
void add_normal_time(int tp, char * pr);
void print_full_branch(char * tree_file_path, int off);
void print_all_versions(char *heads_file_path, char *tree_file_path);

/* Functions used in merging/deleting versions */

int getFileHashPath(int offset, char *full_hash_path, file_data *f1,int return_mode);

GSList *merge_diff(node_mergedata *node,file_data *f,GSList *node_list);

GSList *delete_diff(node_mergedata *node,file_data *file,GSList *node_list);

int create_file_fromlo_cleanup(char * file_tree_path, int d_off, int lo_off, char * obj_dir_path, int mode);       // returns -1 for no success and offset of child for success

/* Functions for compression / de-compression */
void compress(char *obj_file);
void decompress(char *obj_file);
