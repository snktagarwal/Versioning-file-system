
void fuse_mkdir(const char * dirpath);
FILE * fuse_fopen(const char * filepath, char mode); // Note : write events for files opened through fuse_fopen should not be reported for versioning
