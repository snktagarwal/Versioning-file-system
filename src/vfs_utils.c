#include<glib.h>
#include<string.h>
#include<stdio.h>
#include<stdlib.h>
#include<fuse.h>
#include<unistd.h>

#include "vfs.h"
#include "log.h"
#include "versioning.h"
//EDIT 
gint GCompareWI(gconstpointer a, gconstpointer b)
{
	WriteInfo *w1 = (WriteInfo *)a;
	WriteInfo *w2 = (WriteInfo *)b;
	
	log_msg("in GCompare WI %s %s \n", w1->full_path, w2->full_path);
	if(w1->full_path == NULL || w2->full_path == NULL)
		return 0;
	return strcmp((const char*)w1->full_path, (const char*)w2->full_path);	
}

//EDIT 
void add_write_info(VerInfo ver_info, const char* path,size_t size,off_t offset)
{
    WriteInfo *w_tmp = (WriteInfo *)malloc(sizeof(WriteInfo));
    strcpy(w_tmp->full_path, path);
    w_tmp->b_info = g_slist_alloc();
    BlockInfo *b_tmp = (BlockInfo*)malloc(sizeof(BlockInfo));
    b_tmp->offset = offset;
    b_tmp->size = size;    	
	
    WriteInfo *w_found = (WriteInfo*)(g_slist_find_custom(ver_info.ll_wi, (gconstpointer)w_tmp, GCompareWI)->data);
       
    if(w_found == NULL){
    	log_msg("Starting to write\n");
    	w_tmp->b_info = g_slist_insert(w_tmp->b_info,(gpointer)b_tmp,0);
    	ver_info.ll_wi = g_slist_insert(ver_info.ll_wi,(gpointer)w_tmp,0);
    	//WriteInfo *w_tmp2 = (WriteInfo*)(g_slist_find_custom(ver_info.ll_wi, (gconstpointer)w_tmp, GCompareWI)->data);
    }  
    else
    {
        log_msg("Writing blocks\n");
	w_found->b_info = g_slist_append(w_found->b_info,(gpointer)b_tmp);
    }
}

//EDIT 
/**
  to remove the write info of a file
  */
void remove_write_info(VerInfo ver_info, const char *path)
{
    WriteInfo *w_tmp = (WriteInfo *)malloc(sizeof(WriteInfo));
    strcpy(w_tmp->full_path, path);
    w_tmp->b_info = (GSList*)g_slist_alloc();
    WriteInfo *w_found = (g_slist_find_custom(ver_info.ll_wi, (gconstpointer)w_tmp, GCompareWI)->data);
    free(w_tmp);
    
    if(w_found != NULL)
    {
    	log_msg("removing write info\n");
    	g_slist_free(w_found->b_info);
    	ver_info.ll_wi = g_slist_remove(ver_info.ll_wi,(gconstpointer)w_found);
    }
}


//EDIT
//Versioning
void version_file(VerInfo ver_info, const char* path)
{
    //TODO
    //Call the versioning function here
    log_msg("\n Calling Report Release\n");
    report_release(path);
    //remove_write_info(ver_info, path);
}

char *get_log_file_name(char *filepath)
{
	int len,len1=0;
	char filename[255];
	
	len = strlen(filepath);
	
	while(filepath[len] != '/'){
		len--;
		filename[len1++] = filepath[len];
	}
	filepath[len] = '\0';
	len1--;
	filename[len1] = '\0';
	
	g_strreverse(filename);
	
	/* Create log file name */
	strcat(filepath,"/.ver/");
	strcat(filepath, filename);
	//strcat(filepath,".log");
	
	return filepath;
	
}

/* reverse:  reverse string s in place */
 void reverse(char s[])
 {
     int i, j;
     char c;
 
     for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
         c = s[i];
         s[i] = s[j];
         s[j] = c;
     }
 }

/* itoa:  convert n to characters in s */
 void itoa(int n, char s[])
 {
     int i, sign;
 
     if ((sign = n) < 0)  /* record sign */
         n = -n;          /* make n positive */
     i = 0;
     do {       /* generate digits in reverse order */
         s[i++] = n % 10 + '0';   /* get next digit */
     } while ((n /= 10) > 0);     /* delete it */
     if (sign < 0)
         s[i++] = '-';
     s[i] = '\0';
     reverse(s);
 }

 

    	
