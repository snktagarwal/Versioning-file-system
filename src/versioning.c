#define DEBUG

#include <stdio.h>

#include "versioning.h"
#include "versioning_utils.h"

int report_write(const char * filepath) {
	#ifdef DEBUG
		printf ("[versioning] received 'report_write for %s'\n", filepath);
	#endif

	char * dir;
	dirpath = getdir (filepath);
	
	if does_exist( dirpath + ".ver/"  ) == 0
		mkdir_ver( dirpath + ".ver/"  );
	
	// TODO : Handle versioning

	return 0;
}

int report_filedelete(const char * filepath) {
	#ifdef DEBUG
		printf ("[versioning] received 'report_filedelete for %s'\n", filepath);
	#endif

	// TODO : delete all files with names 'filepath*'

	return 0;
}

