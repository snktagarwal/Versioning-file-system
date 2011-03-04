#define DEBUG

#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include "string.h"

#include "../src/versioning.h"
#include "testbench.h"

int mkdir_ (const char *dirpath) {
	#ifdef DEBUG	 
		printf ("[Testbench] recieved 'mkdir %s'\n", dirpath);
	#endif
	
	char command[100];
	sprintf(command,"mkdir %s",dirpath);
	system(command);

	report_write (dirpath);		
	
	return 0;
}

int writefile (const char *filepath, const char *content) {
	#ifdef DEBUG	 
		printf ("[Testbench] recieved 'writefile %s'\n", filepath);
	#endif

	FILE *fp = fopen (filepath, "w");
	fputs (content, fp);
	fclose (fp);

	report_write (filepath);

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

