#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define PATH_MAX 255
int main(int argc, char *argv[])
{
	int i=1;
	if(argc != 2)
		return -1;   //There must be only two arguments
	char *filepath;
	filepath = (char*)malloc(PATH_MAX*sizeof(char));
	
	strcpy(filepath,argv[1]);
	strcat(filepath,"+");

	/* Dummy fetch the file */
	printf("filepath:::%s\n",filepath);
	FILE *f = fopen(filepath,"r");
	if(f!=NULL)
		fclose(f);
	
	/*The call is from gui 
	 *Therefore there is no need to show the file
	 *Instead the head and tree files are automatically stored in /tmp/rvfs/
	 */
}
