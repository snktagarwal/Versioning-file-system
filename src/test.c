#include<stdio.h>
#include<stdlib.h>
#include<glib.h>
#include "vfs.h"
#include<string.h>

int main(){
	
	int a = 1;
	GSList *l = g_slist_alloc();
	WriteInfo *w1 = (WriteInfo *)malloc(sizeof(WriteInfo));
	strcpy(w1->full_path,"/test/path/1");
	l = g_slist_prepend(l, (gpointer)w1);
	return 0;

}
