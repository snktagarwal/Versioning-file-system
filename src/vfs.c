/*
 * A Versioning File System built ovet the top of an
 * existing ext3, fat32 or any other linux complaint
 * file system.
 * Function Format: vfs_<function_name>
*/

#include "params.h"

#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <fuse.h>
#include <libgen.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/xattr.h>
#include <time.h>

#include "vfs.h"
#include "log.h"
#define MAXTAG 255
#define NORMAL 0
#define ABNORMAL 1
VerInfo ver_info;
int last_epoch = 0;

// Report errors to logfile and give -errno to caller
static int vfs_error(char *str)
{
    int ret = -errno;
    
    log_msg("    ERROR %s: %s\n", str, strerror(errno));
    
    return ret;
}

// Check whether the given user is permitted to perform the given operation on the given 

//  All the paths I see are relative to the root of the mounted
//  filesystem.  In order to get to the underlying filesystem, I need to
//  have the mountpoint.  I'll save it away early on in main(), and then
//  whenever I need a path for something I'll call this to construct
//  it.
static void vfs_fullpath(char fpath[PATH_MAX], const char *path)
{
    strcpy(fpath, BB_DATA->rootdir);
    strncat(fpath, path, PATH_MAX); // ridiculously long paths will
				    // break here

    log_msg("    vfs_fullpath:  rootdir = \"%s\", path = \"%s\", fpath = \"%s\"\n",
	    BB_DATA->rootdir, path, fpath);
}

///////////////////////////////////////////////////////////
 //Edit 
 char *get_actual_path(char* filepath)
 {
     	int len,len1=0;
	char filename[255];
	char act_filename[255];
	len = strlen(filepath);
	
	while(filepath[len] != '/'){
		len--;
		filename[len1++] = filepath[len];
	}
	filepath[len] = '\0';
	len1--;
	filename[len1] = '\0';
	
	g_strreverse(filename);
	len = 0;
	while(filename[len]!='@')
	{
		act_filename[len] = filename[len];
		len++;	
	}
	act_filename[len] = '\0';
	strcat(filepath,"/");
	strcat(filepath,act_filename);
	return filepath;
}
/*	Sets the tag of the file in the metadata stored in the .tree file. 
	Takes filepath, timestamp of the version to be tagged and the tag as params */	

int report_file_tag(char *filepath, int timestamp, char *tag)
{
	char *file = (char*)malloc(PATH_MAX*sizeof(char));
	FILE *fp;
	char *newtag = (char *)malloc(256*sizeof(char));
	
	log_msg("assigned \n\n");
	
	newtag = addspaces(tag,255);

	int len,len1=0;
	len = strlen(filepath);
	
	while(filepath[len] != '/'){
		
		len--;
		file[len1++] = filepath[len];
	}
	
	filepath[len] = '\0';
	len1--;
	file[len1] = '\0';
	
	g_strreverse(file);
	
	strcat(filepath,"/.ver");
	//log_msg("filename = %s   ------------------   \n%s \n",filename,dirpath);
	
	log_msg("\n filepath = %s\n filename = %s\n",  filepath,file); 
	
	strcat(filepath,"/trees/");
	strcat(filepath,file);
	strcat(filepath,".tree");
	
	fp = fopen(filepath,"r+");
	
	char *str = (char *)malloc(256*sizeof(char));
	int tp;
	int tmp;
	while(fscanf(fp,"%s",str) != EOF)
	{
		fscanf(fp,"%d",&tp);
		fscanf(fp,"%d",&tmp);
		fscanf(fp,"%s",str);
		if(tp == timestamp)
		{
			//Write tag
			//fseek(45);			// Move the file cursor to the starting char of tag
			fprintf(fp," %s",newtag);	// Overwrite the tag with the new tag
			break;
		}
		else
			fscanf(fp,"%s",str);
		
		fscanf(fp,"%s",str);		//	 strings in the line
		fscanf(fp,"%s",str);		//	not useful 
	}
	
	fclose(fp); 
	
	return 0;
}
 char *set_tags(char *fpath, int mode)
 {
 	int len = strlen(fpath);
 	int len1 = 0,len2 = 0;
 	char *tag = (char*)malloc(PATH_MAX*sizeof(char));
 	char *ver = (char*)malloc(PATH_MAX*sizeof(char));
 	char *ret_file_path = (char*)malloc(PATH_MAX*sizeof(char));
 	len--;
 	while(fpath[len] != '%')
 	{
 		tag[len1] = fpath[len];
 		log_msg("%c\n",tag[len1]);
 		len1++;
 		len--;
 	}
 	tag[len1] = '\0';
 	log_msg("Entering into report tag %c\n",tag[2]);
 	g_strreverse(tag); 
 	log_msg("Entering into report tag %c\n",tag[2]);
 	len--;
 	while(fpath[len] != '%')
 	{
 		ver[len2++] = fpath[len];
 		len--;
 	}
 	ver[len2] = '\0';
 	g_strreverse(ver); 
 	fpath[len] = '\0';
 	log_msg("Entering into report tag %s\n",ver);
 	int ver_no = atoi(ver);
 	strcpy(ret_file_path,fpath);
 	if(mode == 0)
 	{
 		log_msg("Entering into report tag %s\n",tag);
 		int ret = report_file_tag(fpath,ver_no,tag);
 		log_msg("Return Status : %d \n",ret); 
 	}
 	return ret_file_path;
 }
 
 char *checkout_to(char *fpath, int mode)
 {
 	int len = strlen(fpath);
 	int len2 = 0;
 	
 	char *ver = (char*)malloc(PATH_MAX*sizeof(char));
 	char *ret_file_path = (char*)malloc(PATH_MAX*sizeof(char));
 	len--;
 	while(fpath[len] != '@')
 	{
 		ver[len2++] = fpath[len];
 		len--;
 	}
 	ver[len2] = '\0';
 	g_strreverse(ver); 
 	fpath[len] = '\0';
 	log_msg("Entering into report checkout %s\n",ver);
 	int tp = atoi(ver);
 	strcpy(ret_file_path,fpath);
 	if(mode == 0)
 	{
 		log_msg("Entering into report checkout %s\n",ver);
 		report_checkout(fpath,tp);
 	}
 	return ret_file_path;
 }
 
 char *revert_to(char *fpath, int mode)
 {
 	int len = strlen(fpath);
 	int len2 = 0;
 	
 	char *ver = (char*)malloc(PATH_MAX*sizeof(char));
 	char *ret_file_path = (char*)malloc(PATH_MAX*sizeof(char));
 	len--;
 	while(fpath[len] != '&')
 	{
 		ver[len2++] = fpath[len];
 		len--;
 	}
 	ver[len2] = '\0';
 	g_strreverse(ver); 
 	fpath[len] = '\0';
 	log_msg("Entering into report revert %s\n",ver);
 	int tp = atoi(ver);
 	strcpy(ret_file_path,fpath);
 	if(mode == 0)
 	{
 		revert_to_version(fpath,tp);
 	}
 	return ret_file_path;
 }
 char *list_versions(char *fpath, int mode)
 {
 	int len = strlen(fpath);
 	int len2 = 0;
 	
 	char *ret_file_path = (char*)malloc(PATH_MAX*sizeof(char));
 	len--;
 	while(fpath[len] != '#')
 		len--;
 	
 	fpath[len] = '\0';
 	strcpy(ret_file_path,fpath);
 	if(mode == 0)
 	{
 		//IF ANYTHING REQD
 	}
 	return ret_file_path;
 }
 char *cleanDisc(char *fpath, int mode)
 {
 	int len = strlen(fpath);
 	int len2 = 0;
 	
 	char *ret_file_path = (char*)malloc(PATH_MAX*sizeof(char));
 	len--;
 	while(fpath[len] != '^')
 		len--;
 	
 	fpath[len] = '\0';
 	strcpy(ret_file_path,fpath);
 	if(mode == 0)
 	{
 		cleanFile(fpath);
 	}
 	return ret_file_path;
 }
 
//
// Prototypes for all these functions, and the C-style comments,
// come indirectly from /usr/include/fuse.h
//
/** Get file attributes.
 *
 * Similar to stat().  The 'st_dev' and 'st_blksize' fields are
 * ignored.  The 'st_ino' field is ignored except if the 'use_ino'
 * mount option is given.
 */

int vfs_getattr(const char *path, struct stat *statbuf)
{
    //if(primary_rootver_exists() != 0)
    	//create_rootver();
    
    int retstat = 0;
    char fpath[PATH_MAX];	
    log_msg("\nvfs_getattr(path=\"%s\", statbuf=0x%08x)\n",
	  path, statbuf);
    if(strstr(path,".ver")!=NULL)   // Check to make sure ".ver" directories are not found
		return -1;
    vfs_fullpath(fpath, path);
    /*if(strstr(path,"@")!=NULL)
    	get_actual_path(fpath);    
    */
    if(strstr(path,"#")!=NULL)
    	retstat = lstat(list_versions(fpath,NORMAL),statbuf);
    else if(strstr(path,"%")!=NULL)
    	retstat = lstat(set_tags(fpath,NORMAL),statbuf);
    else if(strstr(path,"@")!=NULL)
    	retstat = lstat(checkout_to(fpath,NORMAL),statbuf);
    else if(strstr(path,"&")!=NULL)
    	retstat = lstat(revert_to(fpath,NORMAL),statbuf);
    else if(strstr(path,"^")!=NULL)
	retstat = lstat(cleanDisc(fpath,NORMAL),statbuf);    
    else
    	retstat = lstat(fpath, statbuf);
    if (retstat != 0)
	retstat = vfs_error("vfs_getattr lstat");
    log_stat(statbuf);
    return retstat;
}

/** Read the target of a symbolic link
 *
 * The buffer should be filled with a null terminated string.  The
 * buffer size argument includes the space for the terminating
 * null character.  If the linkname is too long to fit in the
 * buffer, it should be truncated.  The return value should be 0
 * for success.
 */
// Note the system readlink() will truncate and lose the terminating
// null.  So, the size passed to to the system readlink() must be one
// less than the size passed to vfs_readlink()
// vfs_readlink() code by Bernardo F Costa (thanks!)
int vfs_readlink(const char *path, char *link, size_t size)
{
    int retstat = 0;
    char fpath[PATH_MAX];
    
    log_msg("vfs_readlink(path=\"%s\", link=\"%s\", size=%d)\n",
	  path, link, size);
    vfs_fullpath(fpath, path);
    
    retstat = readlink(fpath, link, size - 1);
    if (retstat < 0)
	retstat = vfs_error("vfs_readlink readlink");
    else  {
	link[retstat] = '\0';
	retstat = 0;
    }
    
    return retstat;
}

/* Create a file node
 *
 * There is no create() operation, mknod() will be called for
 * creation of all non-directory, non-symlink nodes.
 */
int vfs_mknod(const char *path, mode_t mode, dev_t dev)
{
    int retstat = 0;
    char fpath[PATH_MAX];
    
    log_msg("\nvfs_mknod(path=\"%s\", mode=0%3o, dev=%lld)\n",
	  path, mode, dev);
    vfs_fullpath(fpath, path);
    
    // On Linux this could just be 'mknod(path, mode, rdev)' but this
    //  is more portable
    if (S_ISREG(mode)) {
        retstat = open(fpath, O_CREAT | O_EXCL | O_WRONLY, mode);
	if (retstat < 0)
	    retstat = vfs_error("vfs_mknod open");
        else {
            retstat = close(retstat);
	    if (retstat < 0)
		retstat = vfs_error("vfs_mknod close");
	}
    } else
	if (S_ISFIFO(mode)) {
	    retstat = mkfifo(fpath, mode);
	    if (retstat < 0)
		retstat = vfs_error("vfs_mknod mkfifo");
	} else {
	    retstat = mknod(fpath, mode, dev);
	    if (retstat < 0)
		retstat = vfs_error("vfs_mknod mknod");
	}
    
    return retstat;
}

/** EDIT */

/** Create a log file in .ver directory **/

/*void vfs_createlog(const char *path, mode_t mode)
{
	char logpath[PATH_MAX];
	
	vfs_fullpath(logpath, path);
	strcat(logpath,"/.ver/ver.log");
	FILE *f;
	f = fopen(logpath,"a");
	 if(!f)
	{
		vfs_error("vfs_createlog ver.log");
		}
	fclose(f);
	
	}*/

/** Create a .ver directory and inside it create objects,trees,heads directory and also a file called OBJ_MD for every directory made */

void vfs_mkverdir(const char *path, mode_t mode)
{
	int retstat = 0;
	char verpath[PATH_MAX];
	char verpath_object[PATH_MAX];
	char verpath_tree[PATH_MAX];
	char verpath_head[PATH_MAX];
	char verpath_md_data[PATH_MAX];	
	
	vfs_fullpath(verpath, path);
	vfs_fullpath(verpath_object, path);
	vfs_fullpath(verpath_tree, path);
	vfs_fullpath(verpath_head, path);
	vfs_fullpath(verpath_md_data, path);

	strcat(verpath,"/.ver");
    	strcat(verpath_object,"/.ver/objects");
	strcat(verpath_tree,"/.ver/trees");
	strcat(verpath_head,"/.ver/heads");
	strcat(verpath_md_data, "/.ver/md_data");
	
	retstat = mkdir(verpath, mode);
        
	if(retstat < 0)
	{
		retstat = vfs_error("vfs_mkverdir verdir");
	}
        
	retstat = mkdir(verpath_object, mode);
    	if(retstat < 0)
	{
		retstat = vfs_error("vfs_mkverdir verdir");
	}

	retstat = mkdir(verpath_tree, mode);
        if(retstat < 0)
	{
		retstat = vfs_error("vfs_mkverdir verdir");
	}
	
	retstat = mkdir(verpath_head, mode);
    	if(retstat < 0)
	{
		retstat = vfs_error("vfs_mkverdir verdir");
	}
    	
    	retstat = mkdir(verpath_md_data, mode);
    	if(retstat < 0)
	{
		retstat = vfs_error("vfs_mkverdir verdir");
	}
    	
    	strcat(verpath,"/OBJ_MD");
    	FILE *f= fopen(verpath,"w");
    	if(f==NULL)
		log_msg("\nError opening file \n ");   
		fclose(f);
    	log_msg("\nvfs_mkdir(path=\"%s\", mode=0%3o)\n",verpath,mode);
}

/** Edited to add feature to create .ver inside each dir */

/** Create a directory */
int vfs_mkdir(const char *path, mode_t mode)
{
    int retstat = 0;
    char fpath[PATH_MAX];
    
    log_msg("\nvfs_mkdir(path=\"%s\", mode=0%3o)\n",
	    path, mode);
    vfs_fullpath(fpath, path);
    
    retstat = mkdir(fpath, mode);
    if (retstat < 0)
	retstat = vfs_error("vfs_mkdir mkdir");
	
	vfs_mkverdir(path,mode);
    
    return retstat;
}

char *get_file_name(char *filepath, char filename[PATH_MAX])
{
	int len,len1=0;
	log_msg("file naem ---------------------------- \n");
	len = strlen(filepath);
	
	while(filepath[len] != '/'){
		len--;
		filename[len1++] = filepath[len];
	}
	log_msg("file naem ---------------------------- \n");
	filepath[len] = '\0';
	len1--;
	filename[len1] = '\0';
	
	g_strreverse(filename);
	log_msg("file naem ---------------------------- \n");	
	
	strcat(filepath,"/.ver");
	
	log_msg("filename = %s\n",filename);
	
	return filename;
	
}

void get_file_name_new(char *dirpath, char* filename, const char *path)
{

	vfs_fullpath(dirpath, path);	
	//log_msg("----------------------------------------------------\n");
	
	int len,len1=0;
	len = strlen(dirpath);
	char *filen = (char*)malloc(PATH_MAX*sizeof(char));
	while(dirpath[len] != '/'){
		
		len--;
		filen[len1++] = dirpath[len];
		//log_msg("----------------------------------------------------\n");
	}
	
	dirpath[len] = '\0';
	len1--;
	filen[len1] = '\0';
	
	g_strreverse(filen);
	
	strcat(dirpath,"/.ver");
	
	//log_msg("filename = %s\n",filen);
	strcpy(filename, filen);
	//log_msg("filename = %s   ------------------   \n%s \n",filename,dirpath);
}

void iterator(gpointer key,gpointer val,gpointer f)
{
	char *keystr = (char*)key;
	int *keyval = (int*)val;
	fprintf((FILE*)f,"%s %d\n",keystr,*keyval);
} 
//Edit
void remove_versions(const char *fpath)
{
	char *file_heads_path,*file_objects_path,*file_trees_path,*file_objmd_path;
	file_heads_path = (char*)malloc(PATH_MAX*sizeof(char));
	file_trees_path = (char*)malloc(PATH_MAX*sizeof(char));
	file_objects_path = (char*)malloc(PATH_MAX*sizeof(char));
	file_objmd_path = (char*)malloc(PATH_MAX*sizeof(char));
	char filename[PATH_MAX]; 
	get_file_name(fpath,filename);
	
	log_msg("fpath = %s\n fname = %s",fpath,filename);
	
	strcpy(file_heads_path,fpath);	
	strcpy(file_trees_path,fpath);	
	strcpy(file_objects_path,fpath);	
	strcpy(file_objmd_path,fpath);
	
	strcat(file_heads_path,"/heads/");
	strcat(file_trees_path,"/trees/");
	strcat(file_objects_path,"/objects/");
	strcat(file_objmd_path,"/OBJ_MD");
	FILE *f;
	/*
	f = fopen(file_objmd_path,"r");	
	GHashTable *gHashTable = g_hash_table_new(g_str_hash,g_str_equal);
	char *file_hash=(char*)malloc(41*sizeof(char));
	int ref_count;
	while(fscanf(f,"%s",file_hash) != EOF)
	{
		fscanf(f,"%d",&ref_count);
		g_hash_table_insert(gHashTable,file_hash,&ref_count);
	}
	fclose(f); */
	strcat(file_heads_path,filename);
	strcat(file_heads_path,".head");
	strcat(file_trees_path,filename);
	strcat(file_trees_path,".tree");
	char *str = (char*)malloc(PATH_MAX*sizeof(char));
	
	char *file_obj_path = (char*)malloc(PATH_MAX*sizeof(char));
	f = fopen(file_trees_path,"r");
	while(fscanf(f,"%s",str) != EOF)
	{
		fscanf(f,"%s",str);
		fscanf(f,"%s",str);
		fscanf(f,"%s",str);
		strcpy(file_obj_path,file_objects_path);
		strcat(file_obj_path,"/");
		strcat(file_obj_path,str);
		unlink(file_obj_path);
		/*
		ref_count = *((int*)g_hash_table_lookup(gHashTable,str));
		ref_count = ref_count - 1; 
		if(ref_count == 0)
			g_hash_table_remove(gHashTable,str);
		else
			g_hash_table_replace(gHashTable,str,&ref_count);
			*/
		fscanf(f,"%s",str);
		fscanf(f,"%s",str);
		fscanf(f,"%s",str);		
	}
	fclose(f);
	unlink(file_heads_path);
	unlink(file_trees_path);
	//f = fopen(file_objmd_path,"w");
	//g_hash_table_foreach(gHashTable,(GHFunc)iterator,f);
	//fclose(f);
}

/** Remove a file */
int vfs_unlink(const char *path)
{
    int retstat = 0;
    char fpath[PATH_MAX];
    
    
    log_msg("vfs_unlink(path=\"%s\")\n",
	    path);
    vfs_fullpath(fpath, path);
    
    retstat = unlink(fpath);
    if (retstat < 0)
	retstat = vfs_error("vfs_unlink unlink");
    remove_versions(fpath);
    return retstat;
}

//EDIT
//Removing Version Directory
int remove_ver_dir(const char *fpath)
{
     char *ver_dir_path = (char*)malloc(PATH_MAX*sizeof(char)),
     	  *heads_path = (char*)malloc(PATH_MAX*sizeof(char)),
     	  *trees_path = (char*)malloc(PATH_MAX*sizeof(char)),
     	  *objects_path = (char*)malloc(PATH_MAX*sizeof(char)),
     	  *objmd_path = (char*)malloc(PATH_MAX*sizeof(char));
     strcpy(ver_dir_path,fpath);
     strcat(ver_dir_path,"/.ver");
     
     strcpy(heads_path,ver_dir_path);
     strcat(heads_path,"/heads");
     int retstat = 0;
     retstat = rmdir(heads_path);
     if(retstat<0)
     {
     	log_msg("Error removing heads folder\n");
     	return -1;
     }
     
     strcpy(trees_path,ver_dir_path);
     strcat(trees_path,"/trees");
     retstat = 0;
     retstat = rmdir(trees_path);
     if(retstat<0)
     {
     	log_msg("Error removing trees folder\n");
     	return -1;
     }
     
     strcpy(objects_path,ver_dir_path);
     strcat(objects_path,"/heads");
     retstat = 0;
     retstat = rmdir(objects_path);
     if(retstat<0)
     {
     	log_msg("Error removing objects folder\n");
     	return -1;
     }
     
     strcpy(objmd_path,ver_dir_path);
     strcat(objmd_path,"/heads");
     retstat = 0;
     retstat = unlink(objmd_path);
     if(retstat<0)
     {
     	log_msg("Error removing objmd folder\n");
     	return -1;
     }
     
     retstat = rmdir(ver_dir_path);
     if(retstat<0)
     {
     	log_msg("Error removing ver dir\n");
     	return -1;
     }
     return 0;
     
}

/** Remove a directory */
int vfs_rmdir(const char *path)
{
    int retstat = 0;
    char fpath[PATH_MAX];
    
    log_msg("vfs_rmdir(path=\"%s\")\n",
	    path);
    vfs_fullpath(fpath, path);
    
    retstat = rmdir(fpath);
    if (retstat < 0)
    {
    	retstat = remove_ver_dir(fpath);
    	if(retstat<0)
	    return vfs_error("vfs_rmdir rmdir version dir");
	retstat = rmdir(fpath);
	if(retstat<0)
	    return vfs_error("vfs_rmdir rmdir main dir");
    }
    return retstat;
}

/** Create a symbolic link */
// The parameters here are a little bit confusing, but do correspond
// to the symlink() system call.  The 'path' is where the link points,
// while the 'link' is the link itself.  So we need to leave the path
// unaltered, but insert the link into the mounted directory.
int vfs_symlink(const char *path, const char *link)
{
    int retstat = 0;
    char flink[PATH_MAX];
    
    log_msg("\nvfs_symlink(path=\"%s\", link=\"%s\")\n",
	    path, link);
    vfs_fullpath(flink, link);
    
    retstat = symlink(path, flink);
    if (retstat < 0)
	retstat = vfs_error("vfs_symlink symlink");
    
    return retstat;
}


/** Manage the versions of the file/directory upon renaming */

/*int vfs_ver_rename(const char *path,const char *newpath)
{
    char f_ver_path[PATH_MAX];
    char f_ver_newpath[PATH_MAX];
    vfs_fullpath(f_ver_path,path);
    vfs_fullpath(f_ver_newpath,newpath);
    get_log_file_name(f_ver_path);
    get_log_file_name(f_ver_newpath);
    log_msg("rename %s -> %s",f_ver_path,f_ver_newpath);
    //strcat(fver_path,"/.ver");
    //strcat(fnew_ver_path,"/.ver");
    //strcat(fver_path,path);
    //strcat(fnew_ver_path,newpath);
    int restat=0;
    restat=rename(f_ver_path,f_ver_newpath);
    if(restat<0)
        restat = vfs_error("vfs_ver_rename ver_rename");

    return restat;
}
*/
/* Rename all the versioning data of the file.
 *Takes as params the relative original and new path.
 *Renames oldpath.tree -> newpath.tree and oldpath.head -> newpath.head 
 */ 

int vfs_version_rename(const char *path, const char *newpath)
{
	char *filename = (char*)malloc(PATH_MAX*sizeof(char)),
             *newfilename = (char*)malloc(PATH_MAX*sizeof(char));
	char fpath[PATH_MAX];
	char fnewpath[PATH_MAX];
	char treepath[PATH_MAX],headpath[PATH_MAX], newtreepath[PATH_MAX], newheadpath[PATH_MAX], dirpath[PATH_MAX];
	char objpath[PATH_MAX],newobjpath[PATH_MAX];
	
	FILE *fp;
	
	log_msg("Rea---------------------------------------------------\n");
	
	get_file_name_new(dirpath,filename, path);
	
	log_msg("%s %s %s",dirpath,filename,path);
	
	strcpy(fpath,dirpath);
	get_file_name_new(dirpath,newfilename, newpath);
	strcpy(fnewpath, dirpath);
	
	log_msg("\nfpath = %s\nnew f path = %s\nfname = %s\nnew fname = %s\n",fpath,fnewpath,filename,newfilename);

	strcpy(treepath,fpath);
	strcpy(newtreepath,fnewpath);
	strcpy(headpath,fpath);
	strcpy(newheadpath,fnewpath);
	strcpy(objpath,fpath);
	strcpy(newobjpath,fnewpath);
	
	log_msg("\ntreepath = %s\nnew tree path = %s\nhead path = %s\nnew head path = %s\n",treepath,newtreepath,headpath,newheadpath);
	
	strcat(treepath,"/trees/");
	strcat(newtreepath,"/trees/");
	strcat(headpath,"/heads/");
	strcat(newheadpath,"/heads/");
	strcat(objpath,"/objects/");
	strcat(newobjpath,"/objects/");
	
	log_msg("\ntreepath = %s\nnew tree path = %s\nhead path = %s\nnew head path = %s\n",treepath,newtreepath,headpath,newheadpath);
	
	strcat(treepath,filename);
	strcat(newtreepath,newfilename);
	strcat(headpath,filename);
	strcat(newheadpath,newfilename);
	
	log_msg("\ntreepath = %s\nnew tree path = %s\nhead path = %s\nnew head path = %s\n",treepath,newtreepath,headpath,newheadpath);
	
	strcat(treepath,".tree");
	strcat(newtreepath,".tree");
	strcat(headpath,".head");
	strcat(newheadpath,".head");
	
	// Move the objects to the corresponding path
	
	fp = fopen(treepath, "r");
	
	char *str = (char *)malloc(256*sizeof(char));
	char *hash = (char *)malloc(41*sizeof(char));
	char hashobjpath[PATH_MAX],newhashobjpath[PATH_MAX];
	int tp;
	int tmp;
	while(fscanf(fp,"%s",str) != EOF)
	{
		fscanf(fp,"%d",&tp);
		fscanf(fp,"%d",&tmp);
		
		fscanf(fp,"%s",hash);
		
		// Move the corresponding file to the correct folder
		
		strcpy(hashobjpath, objpath);
		strcpy(newhashobjpath, newobjpath);
		strcat(hashobjpath, hash);
		strcat(newhashobjpath, hash);
		
		rename(hashobjpath,newhashobjpath);
		
		fscanf(fp,"%s",str);
		fscanf(fp,"%s",str);		
		fscanf(fp,"%s",str);		//	not useful 
	}
	
	fclose(fp); 
	
	/* Still need to update OBJ_MD files in both folders   */
	
	log_msg("\ntreepath = %s\nnew tree path = %s\nhead path = %s\nnew head path = %s\n",treepath,newtreepath,headpath,newheadpath);
	
	rename(treepath,newtreepath);
	rename(headpath,newheadpath);
		
	log_msg("\ntreepath = %s\nnew tree path = %s\nhead path = %s\nnew head path = %s\n",treepath,newtreepath,headpath,newheadpath);
	
	return 0;
	
}
/** Rename a file */
// both path and newpath are fs-relative
int vfs_rename(const char *path, const char *newpath)
{
    int retstat = 0;
    int retstat1 = 0;
    char fpath[PATH_MAX];
    char fnewpath[PATH_MAX];
    
    log_msg("\nvfs_rename(fpath=\"%s\", newpath=\"%s\")\n",
        path, newpath);
    vfs_fullpath(fpath, path);
    vfs_fullpath(fnewpath, newpath);
    
    retstat = rename(fpath, fnewpath);
    if (retstat < 0)
    retstat = vfs_error("vfs_rename rename");
    
    retstat1 = vfs_version_rename(path,newpath);
    
    if(retstat1 < 0)
    log_msg("\nRenaming a dir, not a file. Hence ../.ver/filename will not exist\n");
    
    return retstat;
}


/** Create a hard link to a file */
int vfs_link(const char *path, const char *newpath)
{
    int retstat = 0;
    char fpath[PATH_MAX], fnewpath[PATH_MAX];
    
    log_msg("\nvfs_link(path=\"%s\", newpath=\"%s\")\n",
	    path, newpath);
    vfs_fullpath(fpath, path);
    vfs_fullpath(fnewpath, newpath);
    
    retstat = link(fpath, fnewpath);
    if (retstat < 0)
	retstat = vfs_error("vfs_link link");
    
    return retstat;
}

/** Change the permission bits of a file */
int vfs_chmod(const char *path, mode_t mode)
{
    int retstat = 0;
    char fpath[PATH_MAX];
    
    log_msg("\nvfs_chmod(fpath=\"%s\", mode=0%03o)\n",
	    path, mode);
    vfs_fullpath(fpath, path);
    
    retstat = chmod(fpath, mode);
    if (retstat < 0)
	retstat = vfs_error("vfs_chmod chmod");
    
    return retstat;
}

/** Change the owner and group of a file */
int vfs_chown(const char *path, uid_t uid, gid_t gid)
  
{
    int retstat = 0;
    char fpath[PATH_MAX];
    
    log_msg("\nvfs_chown(path=\"%s\", uid=%d, gid=%d)\n",
	    path, uid, gid);
    vfs_fullpath(fpath, path);
    
    retstat = chown(fpath, uid, gid);
    if (retstat < 0)
	retstat = vfs_error("vfs_chown chown");
    
    return retstat;
}

/** Change the size of a file */
int vfs_truncate(const char *path, off_t newsize)
{
    int retstat = 0;
    char fpath[PATH_MAX];
    
    log_msg("\nvfs_truncate(path=\"%s\", newsize=%lld)\n",
	    path, newsize);
    vfs_fullpath(fpath, path);
    
    retstat = truncate(fpath, newsize);
    if (retstat < 0)
	vfs_error("vfs_truncate truncate");
    
    return retstat;
}

/** Change the access and/or modification times of a file */
/* note -- I'll want to change this as soon as 2.6 is in debian testing */
int vfs_utime(const char *path, struct utimbuf *ubuf)
{
    int retstat = 0;
    char fpath[PATH_MAX];
    
    log_msg("\nvfs_utime(path=\"%s\", ubuf=0x%08x)\n",
	    path, ubuf);
    vfs_fullpath(fpath, path);
    if(strstr(path,"%")!=NULL || strstr(path,"@")!=NULL || strstr(path,"&")!=NULL || strstr(path,"^")!=NULL)
    	set_tags(fpath,ABNORMAL);
    retstat = utime(fpath, ubuf);
    if (retstat < 0)
	retstat = vfs_error("vfs_utime utime");
    
    return retstat;
}

/** File open operation
 *
 * No creation, or truncation flags (O_CREAT, O_EXCL, O_TRUNC)
 * will be passed to open().  Open should check if the operation
 * is permitted for the given flags.  Optionally open may also
 * return an arbitrary filehandle in the fuse_file_info structure,
 * which will be passed to all file operations.
 *
 * Changed in version 2.2
 */
int vfs_open(const char *path, struct fuse_file_info *fi)
{
    int retstat = 0;
    int fd;
    char fpath[PATH_MAX];
    
    log_msg("\nvfs_open(path\"%s\", fi=0x%08x)\n",
	    path, fi);
    vfs_fullpath(fpath, path);
    if(strstr(path,"%")!=NULL || strstr(path,"@")!=NULL || strstr(path,"&")!=NULL || strstr(path,"^")!=NULL)
    {
    	//set_tags(fpath,ABNORMAL);
    	return 0;
    }
    if(strstr(path,"#")!=NULL)
    {
    	list_versions(fpath,ABNORMAL);
    }
    
    fd = open(fpath, fi->flags);
    if (fd < 0)
	retstat = vfs_error("vfs_open open");
    
    fi->fh = fd;
    log_fi(fi);
    
    return retstat;
}

/** Read data from an open file
 *
 * Read should return exactly the number of bytes requested except
 * on EOF or error, otherwise the rest of the data will be
 * substituted with zeroes.  An exception to this is when the
 * 'direct_io' mount option is specified, in which case the return
 * value of the read system call will reflect the return value of
 * this operation.
 *
 * Changed in version 2.2
 */
// I don't fully understand the documentation above -- it doesn't
// match the documentation for the read() system call which says it
// can return with anything up to the amount of data requested. nor
// with the fusexmp code which returns the amount of data also
// returned by read.
int vfs_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
    int retstat = 0, ind;
    int gui = -1; // 1 for gui call
    		 // 0 for command line call    
    		 // -1 for normal call(not through lsver/__guidata
    
    log_msg("\nvfs_read(path=\"%s\", buf=0x%08x, size=%d, offset=%lld, fi=0x%08x)\n",
	    path, buf, size, offset, fi);
    if(strstr(path,"#~"))
    	gui = 1;
    else if(strstr(path,"#"))
    	gui = 0;
    
    /*Check whether the call is from lsver/__guidata or a normal call*/
    if(strstr(path,"#"))
    {
    	log_msg("Reached to list versions command line\n");
    	/* Open the associated tree file */

    	
    	char *filename = (char*)malloc(PATH_MAX*sizeof(char));
    	char *dirpath = (char*)malloc(PATH_MAX*sizeof(char));
    	char *fpath_tree = (char*)malloc(PATH_MAX*sizeof(char));
    	char *fpath_head = (char*)malloc(PATH_MAX*sizeof(char));
    	char *temp = (char*)malloc(PATH_MAX*sizeof(char));
    	
    	list_versions(path,ABNORMAL);
    	get_file_name_new(dirpath,filename,path);

    	strcpy(fpath_tree,dirpath);
    	strcat(fpath_tree,"/trees/");
    	strcat(fpath_tree,filename);	
    	strcat(fpath_tree,".tree");
    	
    	strcpy(fpath_head,dirpath);
    	strcat(fpath_head,"/heads/");
    	strcat(fpath_head,filename);	
    	strcat(fpath_head,".head");
    	
    	log_msg("Tree Path:%s\n",fpath_tree);
   	log_msg("Head Path:%s\n",fpath_head);
   	
   	/*Create the tmp/rvfs folder*/
   	strcpy(temp,"/tmp/rvfs/");
	mkdir(temp,(mode_t)0755);
	
    	/*Check whether the call is from gui*/
    	if(gui)
    	{
	    	/* Read the data into temp file */
		
		log_msg("************************************* %s", temp);
		strcat(temp,filename);
		strcat(temp,".head");
		copy(fpath_head, temp);
			
		strcpy(temp,"/tmp/rvfs/");
		strcat(temp,filename);
		strcat(temp,".tree");
		copy(fpath_tree, temp);
	}
	/*or from command line*/
	else
	{
	
		strcpy(temp,"/tmp/rvfs/templist");
		copy(fpath_tree, temp);
		/*TODO present the tree file nicely*/
	}		
	
    	return 0;
    }
    
    // no need to get fpath on this one, since I work from fi->fh not the path
    log_fi(fi);
    
    retstat = pread(fi->fh, buf, size, offset);
    if (retstat < 0)
	retstat = vfs_error("vfs_read read");
    
    return retstat;
}



/** Write data to an open file
 *
 * Write should return exactly the number of bytes requested
 * except on error.  An exception to this is when the 'direct_io'
 * mount option is specified (see read operation).
 *
 * Changed in version 2.2
 */
// As  with read(), the documentation above is inconsistent with the
// documentation for the write() system call.
int vfs_write(const char *path, const char *buf, size_t size, off_t offset,
	     struct fuse_file_info *fi)
{
    int retstat = 0;
    
    log_msg("\nvfs_write(path=\"%s\", buf=0x%08x, size=%d, offset=%lld, fi=0x%08x)\n",
	    path, buf, size, offset, fi
	    );
    // no need to get fpath on this one, since I work from fi->fh not the path
    log_fi(fi);
	
    retstat = pwrite(fi->fh, buf, size, offset);
    if (retstat < 0)
	retstat = vfs_error("vfs_write pwrite");
    log_msg("Goiing..........................\n");
    add_write_info(ver_info, path,size,offset);
    return retstat;
}


/** Get file system statistics
 *
 * The 'f_frsize', 'f_favail', 'f_fsid' and 'f_flag' fields are ignored
 *
 * Replaced 'struct statfs' parameter with 'struct statvfs' in
 * version 2.5
 */
int vfs_statfs(const char *path, struct statvfs *statv)
{
    int retstat = 0;
    char fpath[PATH_MAX];
    
    log_msg("\nvfs_statfs(path=\"%s\", statv=0x%08x)\n",
	    path, statv);
    vfs_fullpath(fpath, path);
    
    // get stats for underlying filesystem
    retstat = statvfs(fpath, statv);
    if (retstat < 0)
	retstat = vfs_error("vfs_statfs statvfs");
    
    log_statvfs(statv);
    
    return retstat;
}

/** Possibly flush cached data
 *
 * BIG NOTE: This is not equivalent to fsync().  It's not a
 * request to sync dirty data.
 *
 * Flush is called on each close() of a file descriptor.  So if a
 * filesystem wants to return write errors in close() and the file
 * has cached dirty data, this is a good place to write back data
 * and return any errors.  Since many applications ignore close()
 * errors this is not always useful.
 *
 * NOTE: The flush() method may be called more than once for each
 * open().  This happens if more than one file descriptor refers
 * to an opened file due to dup(), dup2() or fork() calls.  It is
 * not possible to determine if a flush is final, so each flush
 * should be treated equally.  Multiple write-flush sequences are
 * relatively rare, so this shouldn't be a problem.
 *
 * Filesystems shouldn't assume that flush will always be called
 * after some writes, or that if will be called at all.
 *
 * Changed in version 2.2
 */
int vfs_flush(const char *path, struct fuse_file_info *fi)
{
    int retstat = 0;
    
    log_msg("\nvfs_flush(path=\"%s\", fi=0x%08x)\n", path, fi);
    // no need to get fpath on this one, since I work from fi->fh not the path
    log_fi(fi);
	
    return retstat;
}


/** Release an open file
 *
 * Release is called when there are no more references to an open
 * file: all file descriptors are closed and all memory mappings
 * are unmapped.
 *
 * For every open() call there will be exactly one release() call
 * with the same flags and file descriptor.  It is possible to
 * have a file opened more than once, in which case only the last
 * release will mean, that no more reads/writes will happen on the
 * file.  The return value of release is ignored.
 *
 * Changed in version 2.2
 */
int vfs_release(const char *path, struct fuse_file_info *fi)
{
		//int current_epoch  = (int)time(NULL);
		//if(current_epoch - last_epoch < 10)
		//return 0;
		//else
		//last_epoch = current_epoch;
    int retstat = 0;
    if(strstr(path,"%")!=NULL || strstr(path,"@")!=NULL || strstr(path,"&")!=NULL || strstr(path,"#")!=NULL || strstr(path,"^")!=NULL)
    	return 0;
    
    log_msg("\nvfs_release(path=\"%s\", fi=0x%08x)\n",
	  path, fi);
    log_fi(fi);
				
    // We need to close the file.  Had we allocated any resources
    // (buffers etc) we'd need to free them here as well.
    retstat = close(fi->fh);
    
    //EDIT
    //Versioning
    char fpath[PATH_MAX];
    vfs_fullpath(fpath,path);

    version_file(ver_info, fpath);    
    return retstat;
}

/** Synchronize file contents
 *
 * If the datasync parameter is non-zero, then only the user data
 * should be flushed, not the meta data.
 *
 * Changed in version 2.2
 */
int vfs_fsync(const char *path, int datasync, struct fuse_file_info *fi)
{
    int retstat = 0;
    
    log_msg("\nvfs_fsync(path=\"%s\", datasync=%d, fi=0x%08x)\n",
	    path, datasync, fi);
    log_fi(fi);
    
    if (datasync)
	retstat = fdatasync(fi->fh);
    else
	retstat = fsync(fi->fh);
    
    if (retstat < 0)
	vfs_error("vfs_fsync fsync");
    
    return retstat;
}

/** Set extended attributes */
int vfs_setxattr(const char *path, const char *name, const char *value, size_t size, int flags)
{
    int retstat = 0;
    char fpath[PATH_MAX];
    
    log_msg("\nvfs_setxattr(path=\"%s\", name=\"%s\", value=\"%s\", size=%d, flags=0x%08x)\n",
	    path, name, value, size, flags);
    vfs_fullpath(fpath, path);
    
    retstat = lsetxattr(fpath, name, value, size, flags);
    if (retstat < 0)
	retstat = vfs_error("vfs_setxattr lsetxattr");
    
    return retstat;
}

/** Get extended attributes */
int vfs_getxattr(const char *path, const char *name, char *value, size_t size)
{
    int retstat = 0;
    char fpath[PATH_MAX];
    
    log_msg("\nvfs_getxattr(path = \"%s\", name = \"%s\", value = 0x%08x, size = %d)\n",
	    path, name, value, size);
    vfs_fullpath(fpath, path);
    
    retstat = lgetxattr(fpath, name, value, size);
    if (retstat < 0)
	retstat = vfs_error("vfs_getxattr lgetxattr");
    else
	log_msg("    value = \"%s\"\n", value);
    
    return retstat;
}

/** List extended attributes */
int vfs_listxattr(const char *path, char *list, size_t size)
{
    int retstat = 0;
    char fpath[PATH_MAX];
    char *ptr;
    
    log_msg("vfs_listxattr(path=\"%s\", list=0x%08x, size=%d)\n",
	    path, list, size
	    );
    vfs_fullpath(fpath, path);
    
    retstat = llistxattr(fpath, list, size);
    if (retstat < 0)
	retstat = vfs_error("vfs_listxattr llistxattr");
    
    log_msg("    returned attributes (length %d):\n", retstat);
    for (ptr = list; ptr < list + retstat; ptr += strlen(ptr)+1)
	log_msg("    \"%s\"\n", ptr);
    
    return retstat;
}

/** Remove extended attributes */
int vfs_removexattr(const char *path, const char *name)
{
    int retstat = 0;
    char fpath[PATH_MAX];
    
    log_msg("\nvfs_removexattr(path=\"%s\", name=\"%s\")\n",
	    path, name);
    vfs_fullpath(fpath, path);
    
    retstat = lremovexattr(fpath, name);
    if (retstat < 0)
	retstat = vfs_error("vfs_removexattr lrmovexattr");
    
    return retstat;
}

/** Open directory
 *
 * This method should check if the open operation is permitted for
 * this  directory
 *
 * Introduced in version 2.3
 */
int vfs_opendir(const char *path, struct fuse_file_info *fi)
{
    DIR *dp;
    int retstat = 0;
    char fpath[PATH_MAX];
    
    log_msg("\nvfs_opendir(path=\"%s\", fi=0x%08x)\n",
	  path, fi);
    vfs_fullpath(fpath, path);
    
    dp = opendir(fpath);
    if (dp == NULL)
	retstat = vfs_error("vfs_opendir opendir");
    
    fi->fh = (intptr_t) dp;
    
    log_fi(fi);
    
    return retstat;
}

/** Read directory
 *
 * This supersedes the old getdir() interface.  New applications
 * should use this.
 *
 * The filesystem may choose between two modes of operation:
 *
 * 1) The readdir implementation ignores the offset parameter, and
 * passes zero to the filler function's offset.  The filler
 * function will not return '1' (unless an error happens), so the
 * whole directory is read in a single readdir operation.  This
 * works just like the old getdir() method.
 *
 * 2) The readdir implementation keeps track of the offsets of the
 * directory entries.  It uses the offset parameter and always
 * passes non-zero offset to the filler function.  When the buffer
 * is full (or an error happens) the filler function will return
 * '1'.
 *
 * Introduced in version 2.3
 */
int vfs_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset,
	       struct fuse_file_info *fi)
{
    int retstat = 0;
    DIR *dp;
    struct dirent *de;
    
    log_msg("\nvfs_readdir(path=\"%s\", buf=0x%08x, filler=0x%08x, offset=%lld, fi=0x%08x)\n",
	    path, buf, filler, offset, fi);
    // once again, no need for fullpath -- but note that I need to cast fi->fh
    dp = (DIR *) (uintptr_t) fi->fh;

    // Every directory contains at least two entries: . and ..  If my
    // first call to the system readdir() returns NULL I've got an
    // error; near as I can tell, that's the only condition under
    // which I can get an error from readdir()
    de = readdir(dp);
    if (de == 0) {
	retstat = vfs_error("vfs_readdir readdir");
	return retstat;
    }

    // This will copy the entire directory into the buffer.  The loop exits
    // when either the system readdir() returns NULL, or filler()
    // returns something non-zero.  The first case just means I've
    // read the whole directory; the second means the buffer is full.
    do {
	log_msg("calling filler with name %s\n", de->d_name);
	
	if(strcmp(de->d_name,".ver")==0) continue;  // Check to make sure ".ver" directories are not found
	
	if (filler(buf, de->d_name, NULL, 0) != 0) {
	    log_msg("    ERROR vfs_readdir filler:  buffer full");
	    return -ENOMEM;
	}
    } while ((de = readdir(dp)) != NULL);
    
    log_fi(fi);
    
    return retstat;
}

/** Release directory
 *
 * Introduced in version 2.3
 */
int vfs_releasedir(const char *path, struct fuse_file_info *fi)
{
    int retstat = 0;
    
    log_msg("\nvfs_releasedir(path=\"%s\", fi=0x%08x)\n",
	    path, fi);
    log_fi(fi);
    
    closedir((DIR *) (uintptr_t) fi->fh);
    
    return retstat;
}

/** Synchronize directory contents
 *
 * If the datasync parameter is non-zero, then only the user data
 * should be flushed, not the meta data
 *
 * Introduced in version 2.3
 */
// when exactly is this called?  when a user calls fsync and it
// happens to be a directory? ???
int vfs_fsyncdir(const char *path, int datasync, struct fuse_file_info *fi)
{
    int retstat = 0;
    
    log_msg("\nvfs_fsyncdir(path=\"%s\", datasync=%d, fi=0x%08x)\n",
	    path, datasync, fi);
    log_fi(fi);
    
    return retstat;
}

//EDIT
/** Initialising the Versioning Info for a file
 */
void init_ver_info()
{
    ver_info.ll_wi = g_slist_alloc();
    log_msg("in init ver info\n");
}


/**
 * Initialize filesystem
 *
 * The return value will passed in the private_data field of
 * fuse_context to all file operations and as a parameter to the
 * destroy() method.
 *
 * Introduced in version 2.3
 * Changed in version 2.6
 */
// Undocumented but extraordinarily useful fact:  the fuse_context is
// set up before this function is called, and
// fuse_get_context()->private_data returns the user_data passed to
// fuse_main().  Really seems like either it should be a third
// parameter coming in here, or else the fact should be documented
// (and this might as well return void, as it did in older versions of
// FUSE).
void *vfs_init(struct fuse_conn_info *conn)
{    
    log_msg("\nvfs_init()\n");
    vfs_mkverdir("",(mode_t)0755);
    init_ver_info();
    return BB_DATA;
}

/**
 * Clean up filesystem
 *
 * Called on filesystem exit.
 *
 * Introduced in version 2.3
 */
void vfs_destroy(void *userdata)
{
    log_msg("\nvfs_destroy(userdata=0x%08x)\n", userdata);
}

/**
 * Check file access permissions
 *
 * This will be called for the access() system call.  If the
 * 'default_permissions' mount option is given, this method is not
 * called.
 *
 * This method is not called under Linux kernel versions 2.4.x
 *
 * Introduced in version 2.5
 */
int vfs_access(const char *path, int mask)
{
    int retstat = 0;
    char fpath[PATH_MAX];
   
    log_msg("\nvfs_access(path=\"%s\", mask=0%o)\n",
	    path, mask);
    vfs_fullpath(fpath, path);
    if(strstr(path,"@")!=NULL)
    	get_actual_path(fpath);
    
    retstat = access(fpath, mask);
    
    if (retstat < 0)
	retstat = vfs_error("vfs_access access");
    
    return retstat;
}

/**
 * Create and open a file
 *
 * If the file does not exist, first create it with the specified
 * mode, and then open it.
 *
 * If this method is not implemented or under Linux kernel
 * versions earlier than 2.6.15, the mknod() and open() methods
 * will be called instead.
 *
 * Introduced in version 2.5
 */
int add_file_to_objects(const char* path){

	char fpath[PATH_MAX];
	char *file_objects_path = (char*)malloc(PATH_MAX*sizeof(char));

	vfs_fullpath(fpath,path);

	guchar data[1024*1024]; 
	gsize size = 0; 
	FILE *input,*f; 
	gchar *sum; 
	log_msg("REACHED HRE\n");
	
	input = fopen( fpath, "rb" ); 
	size = fread( (void *)data, sizeof( guchar ), 1024*1024, input ); 
	fclose( input ); 

	sum = g_compute_checksum_for_data( G_CHECKSUM_SHA1, data, size ); 
	log_msg( "SHA1 Checksum for >> %s <<: %s\n", fpath, sum ); 


	get_log_file_name_new(fpath);
	strcpy(file_objects_path,fpath);
	strcat(file_objects_path,"/objects/");
	strcat(file_objects_path,sum);

	log_msg("Create file with filename as %s",file_objects_path);

	
	f = fopen(file_objects_path,"a");
	if(!f)
	{vfs_error("add file to objects ERROR\n");}

	fclose(f);
	
	char *file_objmd_path = (char*)malloc(PATH_MAX*sizeof(char));
	strcpy(file_objmd_path,fpath);
	strcat(file_objmd_path,"/OBJ_MD");
	f = fopen(file_objmd_path,"r");
	
	/*GHashTable *gHashTable = g_hash_table_new(g_str_hash,g_str_equal);
	char *file_hash=(char*)malloc(255*sizeof(char));
	int ref_count;
	int flag =0;
	
	while(fscanf(f,"%s",file_hash) != EOF)
	{
		fscanf(f,"%d",&ref_count);
		log_msg("I M HERE \n");
		if(strcmp(file_hash,sum) == 0)
		{
			flag = 1;
			ref_count++;
		}
		log_msg("%s %d \n",file_hash,ref_count);
		g_hash_table_insert(gHashTable,file_hash,&ref_count);
	}
	log_msg("reached_here\n");
	int z = 1;
	if(!flag)
	{
		g_hash_table_insert(gHashTable,sum,&z);
	}
	fclose(f);
	f = fopen(file_objmd_path,"w");
	g_hash_table_foreach(gHashTable,(GHFunc)iterator,f);
	*/
	fclose(f);
	g_free( sum ); 
	log_msg("Entered Out of add file to object\n");
	return 0;
}
/*DEPRECATED*/
int vfs_add_newfile_ver_info(const char *path) 
{
    char file_ver_path[PATH_MAX], file_ver_head[PATH_MAX], file_ver_tree[PATH_MAX], file_ver_object[PATH_MAX], file_ver_md_data[PATH_MAX];
    int retstat = 0;
    FILE *f;char filen[PATH_MAX];
    log_msg("new file ver info\n");
    vfs_fullpath(file_ver_path,path); // file_ver_path indicates the absolute path of the version directory of each file
    
    get_file_name(file_ver_path,filen);
	//strip only the filename
    /*log_msg("new file ver info\n");
    int len,len1=0;
    char filen[255]; //filen stores the file name

    len = strlen(filename);

    while(filename[len] != '/'){
            len--;
            filen[len1++] = filename[len];
    }
   	// filename[len] = '\0';
   	// len1--;
  	//  filename[len1] = '\0';

    g_strreverse(filen);
    */


    //mkdir(file_ver_path,(mode_t)0777); // Setting the mode such that we have permissions to create files in the directory
    
    strcpy(file_ver_head, file_ver_path);
    strcpy(file_ver_tree, file_ver_path);
    strcpy(file_ver_md_data, file_ver_path);
    
    strcat(file_ver_head,"/heads/");
    strcat(file_ver_tree,"/trees/");
    strcat(file_ver_md_data,"/md_data/");
    
    strcat(file_ver_head,filen);
    strcat(file_ver_tree,filen);
    strcat(file_ver_md_data,filen);
    strcat(file_ver_head,".head");
    strcat(file_ver_tree,".tree");
    strcat(file_ver_md_data,".md");
 
   	// log_msg("\n%s\n",file_log_path);
    
    f = fopen(file_ver_head,"w");
    if(!f)
    {
    	log_msg("%s",file_ver_head);
    	vfs_error("vfs_create log");
    }
    
    fclose(f);
        
    f = fopen(file_ver_tree,"w");
    if(!f)
    {
	log_msg("%s",file_ver_tree);    
    	vfs_error("vfs_create log");
    }
    
    fclose(f);    
        
  	
    add_file_to_objects(path);	    
    return retstat;

}
/*DEPRECATED*/
 
int vfs_create(const char *path, mode_t mode, struct fuse_file_info *fi)
{
    int retstat = 0;
    char fpath[PATH_MAX];
    int fd;
    
    log_msg("\nvfs_create(path=\"%s\", mode=0%03o, fi=0x%08x)\n",
	    path, mode, fi);
    vfs_fullpath(fpath, path);
    
    fd = creat(fpath, mode);
    if (fd < 0)
	retstat = vfs_error("vfs_create creat");
    
    fi->fh = fd;
    
    log_fi(fi);
    
    //vfs_add_newfile_ver_info(path);
    
    return retstat;
}

/**
 * Change the size of an open file
 *
 * This method is called instead of the truncate() method if the
 * truncation was invoked from an ftruncate() system call.
 *
 * If this method is not implemented or under Linux kernel
 * versions earlier than 2.6.15, the truncate() method will be
 * called instead.
 *
 * Introduced in version 2.5
 */
int vfs_ftruncate(const char *path, off_t offset, struct fuse_file_info *fi)
{
    int retstat = 0;
    
    log_msg("\nvfs_ftruncate(path=\"%s\", offset=%lld, fi=0x%08x)\n",
	    path, offset, fi);
    log_fi(fi);
    
    retstat = ftruncate(fi->fh, offset);
    if (retstat < 0)
	retstat = vfs_error("vfs_ftruncate ftruncate");
    
    return retstat;
}

/**
 * Get attributes from an open file
 *
 * This method is called instead of the getattr() method if the
 * file information is available.
 *
 * Currently this is only called after the create() method if that
 * is implemented (see above).  Later it may be called for
 * invocations of fstat() too.
 *
 * Introduced in version 2.5
 */
// Since it's currently only called after vfs_create(), and vfs_create()
// opens the file, I ought to be able to just use the fd and ignore
// the path...
int vfs_fgetattr(const char *path, struct stat *statbuf, struct fuse_file_info *fi)
{
    int retstat = 0;
    
    log_msg("\nvfs_fgetattr(path=\"%s\", statbuf=0x%08x, fi=0x%08x)\n",
	    path, statbuf, fi);
    log_fi(fi);
    
    retstat = fstat(fi->fh, statbuf);
    if (retstat < 0)
	retstat = vfs_error("vfs_fgetattr fstat");
    
    log_stat(statbuf);
    
    return retstat;
}

struct fuse_operations vfs_oper = {
  .getattr = vfs_getattr,
  .readlink = vfs_readlink,
  // no .getdir -- that's deprecated
  .getdir = NULL,
  .mknod = vfs_mknod,
  .mkdir = vfs_mkdir,
  .unlink = vfs_unlink,
  .rmdir = vfs_rmdir,
  .symlink = vfs_symlink,
  .rename = vfs_rename,
  .link = vfs_link,
  .chmod = vfs_chmod,
  .chown = vfs_chown,
  .truncate = vfs_truncate,
  .utime = vfs_utime,
  .open = vfs_open,
  .read = vfs_read,
  .write = vfs_write,
  /** Just a placeholder, don't set */ // huh???
  .statfs = vfs_statfs,
  .flush = vfs_flush,
  .release = vfs_release,
  .fsync = vfs_fsync,
  .setxattr = vfs_setxattr,
  .getxattr = vfs_getxattr,
  .listxattr = vfs_listxattr,
  .removexattr = vfs_removexattr,
  .opendir = vfs_opendir,
  .readdir = vfs_readdir,
  .releasedir = vfs_releasedir,
  .fsyncdir = vfs_fsyncdir,
  .init = vfs_init,
  .destroy = vfs_destroy,
  .access = vfs_access,
  .create = vfs_create,
  .ftruncate = vfs_ftruncate,
  .fgetattr = vfs_fgetattr
};

void vfs_usage()
{
    fprintf(stderr, "usage:  vfsfs rootDir mountPoint\n");
    abort();
}

int main(int argc, char *argv[])
{
    
    int i;
    int fuse_stat;
    struct vfs_state *vfs_data;

    // vfsfs doesn't do any access checking on its own (the comment
    // blocks in fuse.h mention some of the functions that need
    // accesses checked -- but note there are other functions, like
    // chown(), that also need checking!).  Since running vfsfs as root
    // will therefore open Metrodome-sized holes in the system
    // security, we'll check if root is trying to mount the filesystem
    // and refuse if it is.  The somewhat smaller hole of an ordinary
    // user doing it with the allow_other flag is still there because
    // I don't want to parse the options string.
    if ((getuid() == 0) || (geteuid() == 0)) {
	fprintf(stderr, "Running BBFS as root opens unnacceptable security holes\n");
	return 1;
    }

    vfs_data = calloc(sizeof(struct vfs_state), 1);
    if (vfs_data == NULL) {
	perror("main calloc");
	abort();
    }
    
    vfs_data->logfile = log_open();
    
    // libfuse is able to do most of the command line parsing; all I
    // need to do is to extract the rootdir; this will be the first
    // non-option passed in.  I'm using the GNU non-standard extension
    // and having realpath malloc the space for the path
    // the string.
    for (i = 1; (i < argc) && (argv[i][0] == '-'); i++)
	if (argv[i][1] == 'o') i++; // -o takes a parameter; need to
				    // skip it too.  This doesn't
				    // handle "squashed" parameters
    
    if ((argc - i) != 2) vfs_usage();
    
    vfs_data->rootdir = realpath(argv[i], NULL);

    argv[i] = argv[i+1];
    argc--;
    
	 
    fprintf(stderr, "about to call fuse_main\n");
    fuse_stat = fuse_main(argc, argv, &vfs_oper, vfs_data);
    fprintf(stderr, "fuse_main returned %d\n", fuse_stat);
    
    return fuse_stat;
    
}
