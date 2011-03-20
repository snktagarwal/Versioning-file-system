#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define PATH_MAX 255
int main(int argc, char *argv[])
{
	int i=1;
	if(argc != 4)
		return -1;   //There must be only two arguments
	char *filepath;
	filepath = (char*)malloc(PATH_MAX*sizeof(char));
	
	strcpy(filepath,argv[1]);
	/* argv[2] :Destination offset
	 * argv[3] :Nearest LO offset
	 */
	strcat(filepath,"|");
	strcat(filepath,argv[2]);
	strcat(filepath,"|");
	strcat(filepath,argv[3]);
	/* Dummy fetch the file */
	printf("filepath:::%s\n",filepath);
	FILE *f = fopen(filepath,"r");
	fclose(f);
	
	/*The call is from gui 
	 *The genearated file is automatically stored in /tmp/rvfs/switch
	 */
}
