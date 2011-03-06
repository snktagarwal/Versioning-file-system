#define DEBUG

#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include "string.h"

#include "../src/versioning.h"
#include "testbench.h"

int mkdir_ (const char *dirpath) {
	#ifdef DEBUG	 
		printf ("[Testbench] recieved 'mkdir_ %s'\n", dirpath);
	#endif
	
	char command[1000];
	sprintf(command,"mkdir %s",dirpath);
	system(command);

	//report_release (dirpath);		
	
	return 0;
}

int writefile (const char *filepath, const char *content,int mode) {
	#ifdef DEBUG	 
		printf ("[Testbench] recieved 'writefile %s with mode %d'\n", filepath, mode);
	#endif

	FILE *fp = fopen (filepath, "w");
	if(fp != NULL) {
		#ifdef DEBUG
			printf("[Testbench] file %s opened in write mode\n",filepath);
		#endif
		fprintf (fp,"%s",content);
		fclose (fp);
	}

	if(mode == 0) {
		mkdir_ ("root_test1/dir1/.ver/file1.txt/");
		writefile ("root_test1/dir1/.ver/file1.txt/log","",2);
		writefile ("root_test1/dir1/.ver/file1.txt/current_ver_number","0",2);
		char * path_from_root = (char *) malloc(512*sizeof(char));
		strcat(path_from_root,"/home/vicky/Documents/Opensoft/rvfs/testbench/");
		strcat(path_from_root,filepath);
		report_release (path_from_root);
	}
	else if(mode == 1) {
		char * path_from_root = (char *) malloc(512*sizeof(char));
		strcat(path_from_root,"/home/vicky/Documents/Opensoft/rvfs/testbench/");
		strcat(path_from_root,filepath);
		report_release (path_from_root);
	}

	return 0;
}

int deletefile (const char *filepath) {
	#ifdef DEBUG	 
		printf ("[Testbench] recieved 'deletefile %s'\n", filepath);
	#endif

	if( remove (filepath) == -1)
		return -1;

	report_filedelete (filepath);
	
	return 0;
}

