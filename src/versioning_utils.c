#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <glib.h>
#include "vfs.h"
#include "log.h"
#define MAX_SIZE 1024*1024
#define printf log_msg

/* Deprecate them later */
void iterate(gpointer key,gpointer val,gpointer f)
{
	char *keystr = (char*)key;
	int *keyval = (int*)val;
	char * offs = (char *)malloc(15*sizeof(char));
	itoa(*keyval, offs);
	printf("\n---------------------------------in iterator-----------------------------------------------\n");
	fprintf((FILE*)f,"%s %s\n",keystr,addspaces(offs, 10));
} 
void writeHTtoFile(GHashTable *ht, FILE *f)
{
	g_hash_table_foreach(ht,(GHFunc)iterate,f);		
}
GHashTable *readHTfromFile(FILE *f, int i)
{
	int c = 1;
	char *epoch = (char*)malloc(20*sizeof(char));
	int off;
	for(c = 1; c<i; c++)
	{
		if(fscanf(f,"%s %d",epoch,&off) == EOF)
			return NULL;
	}
	GHashTable *ht = g_hash_table_new(g_str_hash,g_str_equal);
	while(fscanf(f,"%s %d",epoch,&off) != EOF)
	{
		//printf("============================================\n%s\t%d\n", epoch, off);
		g_hash_table_insert(ht,epoch,&off);
	}
	return ht;
}

void deleteTop(char *filepath)
{
	printf("\n----------------------------------------------in delete top=============================================\n");
	char *epoch = (char*)malloc(20*sizeof(char));
	int off;
	FILE *f = fopen(filepath,"r");
	fscanf(f,"%s %d",epoch,&off);
	GHashTable *ht = g_hash_table_new(g_str_hash,g_str_equal);
	char *f_epoch = (char*)malloc(20*sizeof(char));
	int f_off;
	while(fscanf(f,"%s %d",f_epoch,&f_off) != EOF)
	{
		if(strcmp(f_epoch,epoch) != 0)
			g_hash_table_insert(ht,f_epoch,&f_off);
	}
	fclose(f);
	f = fopen(filepath,"w");
	g_hash_table_foreach(ht,(GHFunc)iterate,f);		
	fclose(f);
}


/* Add spaces for padding */
char * addspaces(char *str, int size)
{
	int i;
	//printf("himsnhu\n");
	char * ext = (char *)malloc((size-strlen(str))*sizeof(char));
	printf("%d  %d \n",size,(int)strlen(str));
	if(size>strlen(str))
		strcpy(ext, " ");
	//printf("himsnhu\n");	
	for(i=0; i<(size-strlen(str)-1); i++)
	{
		strcat(ext, " ");
		//printf("himsnhu\n");
	}
	strcat(str, ext);
	printf("himsdsfsfsfdsdnhu\n");
	return str;

}


/* Tells the current File offset
 * Convert to a MACRO
 */

int find_present_file_offset(char * filepath)
{
 FILE * f = fopen(filepath,"a");
 return (ftell(f));
}

/* ---- Deprecate them ----- */


/* Finds the hash of a given file for a given file path */
/*void find_SHA(const char * filepath, char c[41])
{ 
    guchar  data[MAX_SIZE]; 
    gsize   size = 0; 
    FILE   *input; 
    gchar  *sum;  

    input = fopen( filepath, "rb" ); 
    size = fread( (void *)data, sizeof( guchar ), MAX_SIZE, input ); 
    fclose( input ); 

    sum = g_compute_checksum_for_data( G_CHECKSUM_MD5, data, size ); 
    strcpy(c,sum);
} */

void find_SHA(const char * filepath, char c[41]) 
{ 
    GChecksum   *cs;
    guchar       data[MAX_SIZE];
    gsize        size = 0;
    FILE        *input;
    const gchar *sum;
    
    cs = g_checksum_new( G_CHECKSUM_MD5 );    
    
    input = fopen( filepath, "rb" );
    do
    {
        size = fread((void *)data, sizeof(guchar), MAX_SIZE, input );
        g_checksum_update( cs, data, size );
  
    }
    while( size == MAX_SIZE );
    fclose( input );

    sum = g_checksum_get_string( cs );    
    strcpy(c,sum);

    g_checksum_free( cs ); 
 
} 

/* Returns the dirpath and file name from a file path
 * Example: /path/to/filename -> /path/to/ AND filename
 */
void split_file_path (const char * filepath, char *filename, char *dirpath){

	int len,len1=0;
	
	len = strlen(filepath);
	strcpy(dirpath,filepath);
	while(dirpath[len] != '/'){
		len--;
		filename[len1++] = dirpath[len];
	}
	dirpath[len] = '\0';
	len1--;
	filename[len1] = '\0';
	strcat(dirpath,"/");
	g_strreverse(filename);
}



/* Utils Function
 * Gets the hash for a given offset in the Tree file
 */
char * get_hash_from_offset(int offset, char  * file_tree_path)
{
	FILE  * f = fopen(file_tree_path,"r");
	int i;
	char ch;
	char * hash = (char * )malloc(50 * sizeof(char));
	fseek(f,offset,SEEK_SET);
	fscanf(f,"%d",&i);
	fscanf(f,"%d",&i);
	fscanf(f,"%d",&i);
	fscanf(f,"%c",&ch);
	fscanf(f,"%s",hash);
	return hash;
}

/* Utils Function
 * Gets the offset of the present HEAD from the heads file
 */
int get_present_head_offset(char * filepath)
{
	int offset;
	char * s = (char * )malloc(15 * sizeof(char));
	FILE * heads_filepath = fopen(filepath,"r");
	fscanf(heads_filepath,"%s",s);
	fscanf(heads_filepath,"%d",&offset);
	fclose(heads_filepath);
	printf("I am returning offset :%d\n",offset);
	return offset;
}


