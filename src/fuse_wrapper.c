#define DEBUG

#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#include "fuse_wrapper.h"

int makedir (const char *dirpath) {
	char * command = strcat("mkdir ",dirpath);
  //char * command = (char *) malloc(strlen("mkdir ")+strlen(dirpath)+1);
  //sprintf (command, "mkdir %s", dirpath);
  int status = system (command);
  
  if(status == -1)
  	return 0;
	
	return 1;
}

int does_exist(const char * filepath) {
	if(access(filepath,F_OK) != -1)
		return 1;
	else
		return 0;
}

int copy(const char * source, const char * dest) {
	char *command = (char *) malloc(1000*sizeof(char));
	strcpy(command,"cp ");
	strcat(command,source);
	strcat(command," ");
	strcat(command,dest);
	printf("\n\ncopy: %s\n\n",command);
	
	int status = system(command);
	
	if(status == -1)
		return 0;
	
	return 1;
}

int move(const char * source, const char * dest) {
	char * command = (char *) malloc(1000*sizeof(char));
	strcpy(command,"mv ");
	strcat(command,source);
	strcat(command," ");
	strcat(command,dest);
	printf("\n\nmove: %s\n\n",command);
	
	int status = system(command);
	
	if(status == -1)
		return 0;
	
	return 1;
}

void diff(const char * filepath1, const char * filepath2, const char * diff_filepath ) {
	char * command = (char *) malloc(1000*sizeof(char));
	strcpy(command,"diff -u ");
	strcat(command,filepath1);
	strcat(command," ");
	strcat(command,filepath2);
	strcat(command," >");
	strcat(command,diff_filepath);
	printf("\n\ndiff: %s\n\n",command);
	
	system(command);
}

void rem(const char * filepath) {
	char * command = (char *) malloc(1000*sizeof(char));
	strcpy(command,"rm ");
	strcat(command,filepath);
	printf("\n\nremove: %s\n\n",command);
	
	system(command);
}
