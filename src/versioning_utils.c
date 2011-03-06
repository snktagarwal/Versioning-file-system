#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "versioning_utils.h"

int getlastslashpos(const char * filepath) {
  int i; 
	for(i = strlen(filepath)-1; i>=0; i--)
		if(filepath[i]=='/')
			break;
	
	return i;
}

char * getdirpath (const char * filepath) {
	int i = getlastslashpos(filepath);
	
	char * dirpath = (char *)malloc((i+2)*sizeof(char));
	int j;
	for(j=0; j<=i; j++) {
		dirpath[j] = filepath[j];
	}

	return dirpath;
}

char * getfilename(const char * filepath) {
	int i = getlastslashpos(filepath);
	
	char * filename = (char *) malloc( ( strlen(filepath) - i ) * sizeof(char) );
	int j;
	for(j=i+1; j<=strlen(filepath)-1 ;j++)
		filename[j-i-1] = filepath[j];
	
	return filename;
}
