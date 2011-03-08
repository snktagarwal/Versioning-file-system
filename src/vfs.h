/* vfs.h
 * Defines the basic data structure and functions defined for the versioning system
 * does not cover functionality for versioning itself, rather the basic of when
 * to do versioning
 * Author: Sanket Agarwal <sanket@sanketagarwal.com
 */

#include<fuse.h>
#include<glib.h>
#include "log.h"

#define MAX_BLOCKS 100
#define HASH_SHA1 41
#define MAX_TAG 255
#define MAX_BNAME MAX_TAG
#define LO 0
#define PO 1

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

/* New versioning Data Structures */

// Structure to be written onto tree/file.tree

typedef struct _tree_md{
	
	int valid;	/* Tells whether this structure is valid row */
	char obj_hash[HASH_SHA1];
	char tag[MAX_TAG];
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

/* Function prototypes */
gint GCompareWI(gconstpointer a, gconstpointer b);
void add_write_info(VerInfo ver, const char* path,size_t size,off_t offset);
void remove_write_info(VerInfo ver, const char *path);
void version_file(VerInfo ver, const char *path);
char *get_log_file_name(char *filepath);
void itoa(int n,char s[]);
void reverse(char s[]);
