#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

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
	strcat(filepath,"#");

	/* Dummy fetch the file */	
	FILE *f = fopen(filepath,"r");
	char *buf;
	fread(buf, 255, sizeof(char),f);
	/*the temporary files are created in the tmp folder for gui use*/
	fclose(f);
	
	/*The call is from command line
	 *Hence Open the temp file and show the info
	 */
	system("cat /tmp/rvfs/templist");
	system("rm /tmp/rvfs/templist");
}
