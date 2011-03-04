/* vfs.h
 * Defines the basic data structure and functions defined for the versioning system
 * does not cover functionality for versioning itself, rather the basic of when
 * to do versioning
 * Author: Sanket Agarwal <sanket@sanketagarwal.com
 */

#include<fuse.h>
#include<glib.h>

#define MAX_BLOCKS 100

/* Define the types of data that may be stored
 */

#define WRITE_INFO 1
// Information for a block
typedef struct _block_info{
	
	int offset;
	int size;
}BlockInfo;

// Encapsulates the write info
typedef struct _write_info{
	
	char full_path[255];
	BlockInfo b_info[MAX_BLOCKS];
}WriteInfo;

	
// Versioning info MASTER structure
typedef struct _ver_info{
	GSList *ll_wi;	/* Write info list */
}VerInfo;


/* Functions definition */

char *get_log_file_name(char *filepath);

