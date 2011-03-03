void report_write(const char * filepath);;
void report_filedelete(const char * filepath);
void report_dirdelete(const char * dirpath); // recursively deletes all files under dirpath

struct version * get_versionlist(const char * filepath);

changeto_version(const char * filepath, struct version v);


struct version
{
int vnumber;
char id[100]; // id is required inspite of vnumber in the case when a a deleted directory has to be reverted back. In this case only those files are revived which have same version_id as that of dir to be revived. Simply reviving latest versions wont work, because in the case when a file in the dir is deleted before dir was deleted that file will be wrongfully revived on reviving dir if reviving based on latest versions.
struct time timestamp;
int diff_lc; // line count of diff with previous version
}
