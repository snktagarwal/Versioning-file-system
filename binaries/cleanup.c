#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define PATH_MAX 255
int main(int argc, char *argv[])
{
	int i=1;
	if(argc != 3)
	{
		printf("Usage:  clean <filepath> <ratio>\n");
		return -1;   
	}
	char *filepath;
	char * ratio = (char*)malloc(100*sizeof(char));;
	filepath = (char*)malloc(PATH_MAX*sizeof(char));
	
	strcpy(filepath,argv[1]);
	FILE *f = fopen(filepath,"r");
	if(f == NULL)
	{
		printf("File does not exist\n");
		return -1;
	}	
	fclose(f);
	strcat(filepath,"^");
	strcat(filepath,argv[2]);
	
	/* Dummy fetch the file */
	printf("filepath:::%s\n",filepath);
	f = fopen(filepath,"r");
	fclose(f);
	
}
