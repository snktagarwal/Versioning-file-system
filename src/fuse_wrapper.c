#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>

int mkdir_ver (const char *dirpath) {

    char command [100];
    sprintf (command, "mkdir %s", dirpath);
    system (command);
	
	return 1;
}
