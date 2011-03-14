#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MOUNTDIR "mountdir"
#define PATH_MAX 255
int main(int argc, char *argv[])
{
	int i=1;
	if(argc != 2)
		printf("Usage:  lsver <filepath>");
		
	
	char *filepath;
	filepath = (char*)malloc(PATH_MAX*sizeof(char));
	
	strcpy(filepath,argv[1]);
	strcat(filepath,"--");
	
	FILE *f = fopen(filepath,"r");
	fclose(f);
}
