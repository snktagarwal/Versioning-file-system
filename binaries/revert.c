#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MOUNTDIR "mountdir"
#define PATH_MAX 255
int main(int argc, char *argv[])
{
	int i=1;
	if(argc != 3)
		printf("Usage:  revert <filepath> <version no.>");
		
	
	char *filepath;
	//char * revert;
	filepath = (char*)malloc(PATH_MAX*sizeof(char));
	//revert = (char*)malloc(PATH_MAX*sizeof(char));
	int ver_no = atoi(argv[2]);

	strcpy(filepath,argv[1]);
	//strcpy(revert,argv[3]);
	strcat(filepath,"&");
	strcat(filepath,argv[2]);
	//strcat(filepath,"&");
	//strcat(filepath,argv[3]);

	FILE *f = fopen(filepath,"r");
	fclose(f);
}
