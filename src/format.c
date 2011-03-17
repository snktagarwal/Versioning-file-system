#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "vfs.h"
#include "log.h"


void add_normal_time(int tp, char * pr)
{
    struct tm  ts;
    char buf[80];

    time_t now = tp;

    // Format time, "ddd yyyy-mm-dd hh:mm:ss zzz"
    ts = *localtime(&now);
    strftime(buf, sizeof(buf), "%a %Y-%m-%d %H:%M:%S %Z", &ts);
    strcat(pr, buf);
    strcat(pr, " ");
    return ;
}



void print_full_branch(char * tree_file_path, int off)
{
	FILE * fpt = fopen(tree_file_path, "r");
	FILE *fout = fopen("/tmp/rvfs/templist","a");
	log_msg("\n New branch starts here:\n");
	fseek(fpt, off, SEEK_SET);
	int valid, tp, lp, diff_ct, p_off;
	char * hash = (char *)malloc(50*sizeof(char));
	char * tag = (char *)malloc(255*sizeof(char));
	
	char * pr = (char *)malloc(10000*sizeof(char));
	fscanf(fpt, "%d", &valid);
	if(valid==0)
	{
		log_msg("ERROR: no record of parent head in tree.");
		return ;
	}
	fscanf(fpt, "%d %d %s %s %d %d", &tp, &lp, hash, tag, &diff_ct, &p_off);
	int flag=0;
	while(off!=-1)
	{
		char * dummy = (char *)malloc(20*sizeof(char));
		itoa(tp,dummy);
		if(flag==0)
			strcpy(pr, dummy);
		else
			strcat(pr, dummy);
		flag++;
		strcat(pr, " ");
		add_normal_time(tp, pr);
		if(tag[0]=='_')
			strcat(pr, "\n");
		else
		{
			tag[5] = '\0';
			strcat(pr, tag);
			strcat(pr, "\n");
		}
		off=p_off;
		fseek(fpt, off, SEEK_SET);
		fscanf(fpt, "%d", &valid);
		if(valid==0)
		{
			log_msg("ERROR: no record of parent head in tree.");
			return ;
		}
		fscanf(fpt, "%d %d %s %s %d %d", &tp, &lp, hash, tag, &diff_ct, &p_off);
	}
	log_msg("%s\n",pr);
	fprintf(fout,"New Branch starts here\n");
	fprintf(fout,"%s\n", pr);
	
	
	fclose(fout);
	return;
}


void print_all_versions(char *heads_file_path, char *tree_file_path)
{
	FILE * fph = fopen(heads_file_path, "r");
	
	int off;
	char * bname = (char *)malloc(15*sizeof(char));
	fscanf(fph, "%s %d", bname, &off);
	
	while(fscanf(fph, "%s %d", bname, &off)!=EOF)
	{
		print_full_branch(tree_file_path, off);
	}
	fclose(fph);
}


/*
int main()
{
	int a =5;
	printf("tg%5dtg", a);
	return 0;
}*/
