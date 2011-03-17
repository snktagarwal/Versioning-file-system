#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <glib.h>

#include "vfs.h"


/*    file_data *test = (file_data *)malloc(sizeof(file_data));*/
/*	*/
/*	strcpy(test->path,"/home/vinay/rvfs/testing/rootdir/l");*/
/*	strcpy(test->name,"l");*/
/*	strcpy(test->dir_path,"/home/vinay/rvfs/testing/rootdir/");*/
/*	strcpy(test->ver_dir_path,"/home/vinay/rvfs/testing/rootdir/.ver/");*/
/*	strcpy(test->objects_dir_path,"/home/vinay/rvfs/testing/rootdir/.ver/objects/");*/
/*	strcpy(test->tree_file_path,"/home/vinay/rvfs/testing/rootdir/.ver/trees/l.tree");*/
/*	strcpy(test->heads_file_path,"/home/vinay/rvfs/testing/rootdir/.ver/heads/l.head");*/
/*	*/
/*	print_file_data(test);*/

	

// calculates size of given file and returns as int

int calc_file_size(char *fpath)
{
	int size;
	struct stat st;
	stat(fpath, &st);
	size = st.st_size;
	
	//log_msg("\nfile size of %s = %d\n",fpath,size);
	return size;

}

//Given a tree file, calculates the total size of all objects in that tree

int calc_obj_size(file_data * file)
{
	
	FILE *fp;
	int tmp,size=0;
	char *str = (char *)malloc(300*sizeof(char));
	char *hash = (char *)malloc(50*sizeof(char));
	char fpath[PATH_MAX];
	
	fp = fopen(file->tree_file_path,"r");
	
	while(fscanf(fp,"%s",str) != EOF)
	{
		fscanf(fp,"%d",&tmp);
		fscanf(fp,"%d",&tmp);
		fscanf(fp,"%s",hash);
		strcpy(fpath,file->objects_dir_path);
		size += calc_file_size(strcat(fpath,hash));
 		fscanf(fp,"%s",str);		
		fscanf(fp,"%s",str);		
		fscanf(fp,"%s",str);		
		
	}	
	
	fclose(fp);
	return size;
}

// calculates size of version metadata and returns as int

int calc_md_size(file_data * file)
{

	int tree_file_size, head_file_size, obj_file_size,total;
	
	tree_file_size = calc_file_size(file->tree_file_path);
	head_file_size = calc_file_size(file->heads_file_path);
	obj_file_size = calc_obj_size(file);
	
	log_msg("\ntree = %d\n head = %d obj = %d\n", tree_file_size,head_file_size,obj_file_size);
	
	total = (tree_file_size+head_file_size+obj_file_size);
	
	log_msg("\nmetadata size of %s = %d", file->path,total) ;

	return total;
}

void compare_diff_vs_time(file_data *file)
{	

	GSList* node_list = NULL;
	
	node_list = generate_ll(file,node_list);
	   	
	node_list = g_slist_sort(node_list, (GCompareFunc)compare_node_mergedata);
	
	log_msg("\nblah\n");
	
	g_slist_foreach(node_list, (GFunc)print_iterator, NULL);
	// Send list to fuddi
	//g_slist_foreach(node_list, (GFunc)g_free, NULL);
	
	g_slist_free(node_list);
	
	log_msg("\n\nDone\n\n");
	
	
}

int type(int lopo, int tagged)
{
	if( (lopo) && (!tagged))
		return 1;
	else if( (lopo) && (tagged))
		return 2;
	else if( (!lopo) && (!tagged))
		return 3;
	else return 4;
} 


void print_iterator(gpointer item) {

	log_msg("current offset %d child offset %d current tp %d child tp %d current_type %d parent_type %d mergedata %f nearest lo %d\n",((node_mergedata*)item)->current_offset,((node_mergedata*)item)->child_offset,((node_mergedata*)item)->current_timestamp,((node_mergedata*)item)->child_timestamp,((node_mergedata*)item)->current_type,((node_mergedata*)item)->parent_type,((node_mergedata*)item)->mergedata,((node_mergedata*)item)->nearest_lo_offset);
 
}

gint compare_node_mergedata(gconstpointer item1, gconstpointer item2) {
		/*
		if(((node_mergedata*)item1->data)->current_timestamp == ((node_mergedata*)item2->data)->current_timestamp && ((node_mergedata*)item1->data)->child_timestamp == ((node_mergedata*)item2->data)->child_timestamp)
			return 1;
		else 
			return 0;		 */
			
		//log_msg("current_timestamp1 = %d current_timestamp2 = %d child_timestamp1 = %d child_timestamp2 = %d\n",((node_mergedata*)item1)->current_timestamp,((node_mergedata*)item2)->current_timestamp,((node_mergedata*)item1)->child_timestamp,((node_mergedata*)item2)->child_timestamp);
		
		
		if(((node_mergedata*)item1)->current_timestamp == ((node_mergedata*)item2)->current_timestamp && ((node_mergedata*)item1)->child_timestamp == ((node_mergedata*)item2)->child_timestamp)
			return 1;
		else 
			return 0;		 
}



GSList* generate_ll(file_data *file,GSList* node_list)
{
	FILE *fp_tree, *fp_head;
	fp_tree=fopen(file->tree_file_path,"r");
	fp_head=fopen(file->heads_file_path,"r");
	
	GSList *head_list = NULL;
	GSList *lo_list = NULL;
	GSList *iterator = NULL;


    if(fp_tree==NULL || fp_head==NULL) //do check if file exists.
	   {
	   	printf("ERROR: tree or head file does not exist");
	   }
	   
	log_msg("\nfp_head and fp_tree are not null\n"); 
	
	int current_offset = 0;
	   
	char *tp = (char *)malloc(15*sizeof(char));
	   
	fscanf(fp_head,"%s %d",tp, &current_offset);
	
	
	   
	log_msg("\nScanned first line of .head file\n");
	
	while(fscanf(fp_head,"%s %d\n",tp,&current_offset) != EOF)
	{
		//log_msg("%d: current_timestamp %s current_offset %d\n",c,tp,current_offset);
		head_list = g_slist_prepend(head_list, &current_offset);
	}
	
	//log_msg("\nhead_list:\n");
	//g_slist_foreach(head_list, (GFunc)print_head_iterator, NULL);
	//g_slist_foreach(head_list, (GFunc)move_to_lo_iterator,NULL);
	
	while(head_list != NULL)
	{
		log_msg("\nhead_list:\n");
		for (iterator = head_list; iterator; iterator = iterator->next) {
	  		
	  		log_msg("Current offset is '%d'\n", iterator->data);
	  		
	  		node_list = move_to_lo_iterator(*((int*)(iterator->data)),fp_tree,node_list,lo_list);
	  		
 		}
 		
 		log_msg("Finished moving all heads to corresponding LO.\n");
 		
 		//g_slist_foreach(head_list, (GFunc)g_free, NULL);
 		
 		log_msg("Freed each item of head_list\n");
	
		g_slist_free(head_list);
		
		log_msg("g_free head_list\n");
		
		head_list = lo_list;
		
		log_msg("head_list == lo_list\n");
		
		log_msg(" Freed heads list and equated heads list to lo_list\n");
		
		lo_list = NULL;
 		
/* 		log_msg("vinay\n");*/
/*	  	g_slist_foreach(head_list, (GFunc)print_head_iterator, NULL);*/
	}
	   	
	fclose(fp_tree);
	fclose(fp_head);
	
	//g_slist_foreach(head_list, (GFunc)g_free, NULL);
	g_slist_free(head_list);
	
	head_list = NULL;
	
	//g_slist_foreach(lo_list, (GFunc)g_free, NULL);
	g_slist_free(lo_list);
	
	lo_list = NULL;
	
	return node_list;
	
}

void print_head_iterator(gpointer item) {
 log_msg("%d\n",item);
}

GSList *move_to_lo_iterator(int current_offset, FILE *fp_tree, GSList *node_list, GSList *lo_list)
{	
	int current_timestamp, child_timestamp=-1;
	int parent_offset,child_offset,tmp_int,no_diff,current_type=-1,tagged=-1,current_newtype,nearest_lo_offset;
	
	nearest_lo_offset = current_offset;
	
	node_mergedata *node = NULL;
	
	char *tmp_char=(char *)malloc(255*sizeof(char));
	char *tag = (char *)malloc(256*sizeof(char));
	
		do{	
			
				log_msg("Moving to tree file\n");
			
				fseek(fp_tree,current_offset,SEEK_SET);
		
				fscanf(fp_tree,"%d", &tmp_int);											// Valid,tmp_int
				//log_msg("\n%d\n",tmp_int);
				fscanf(fp_tree,"%d", &current_timestamp);								// Epoch
				//log_msg("\n%d\n",current_timestamp);
				fscanf(fp_tree,"%d", &current_type);									// LO/PO
				//log_msg("\n%d\n",current_type);
				fscanf(fp_tree,"%s", tmp_char);											// Hash
				//log_msg("\n%s\n",tmp_char);
				fscanf(fp_tree,"%s", tag);												// Tag
				//log_msg("\n%s\n",tag);
				fscanf(fp_tree,"%d", &no_diff);											// #diff
				//log_msg("\n%d\n",no_diff);
				fscanf(fp_tree,"%d", &parent_offset);									// Parent offset
				//log_msg("\n%d\n",parent_offset);
				
				if(strcmp(tag,"_")==0)
					tagged = 0;
				else tagged = 1;
				
				//tagged =0 ; /* IMPORTANT : to be removed later, checking currently for untagged */
				
				current_newtype = type(current_type,tagged);
				
				if(current_type == LO && current_offset != nearest_lo_offset)
				{
					log_msg("\nEncountered LO\n");
					
					if(g_slist_find(lo_list, &current_offset) != NULL)
					{
						log_msg("LO already present in list\n");
					}
					else{
						lo_list = g_slist_prepend(lo_list,&current_offset);
						log_msg("Added item to list\n");
					}
					break;
				}
				
				log_msg("%d %d %d %s %s %d %d\n",tmp_int,current_timestamp,current_type,tmp_char,tag,no_diff,parent_offset);
				
				if(node != NULL)
				{
					//log_msg("\n Node is not NULL, hence need to set parent_type as %d\n",current_newtype);
				
					node->parent_type = current_newtype;
					node = NULL;
				}
				
				if((child_timestamp!=-1) && current_newtype == 1)
				{
				
					log_msg("Creating a node to add to the linked list\n");
				
					node = (node_mergedata *)malloc(sizeof(node_mergedata));
					
					node->current_offset = current_offset;
					node->child_offset = child_offset;	
					node->current_timestamp = current_timestamp;
					node->child_timestamp = child_timestamp;		
					node->current_type = current_newtype;
					node->mergedata = (float)no_diff/(child_timestamp - current_timestamp);	
					node->nearest_lo_offset = nearest_lo_offset;
					
					log_msg("\nAdding node\n");
					node_list = g_slist_prepend(node_list, node);
					//print_iterator(node);
					
				}
				child_timestamp = current_timestamp;		// Store current timestamp before moving to parent	
				child_offset = current_offset;
				current_offset = parent_offset;		  			// move to the parent
				
				log_msg("Now move to parent -> parent_offset = %d\n",parent_offset);
				
			}while(parent_offset != -1);
			
			log_msg("\n Out of while loop\n\n");
			
		return node_list;
}

GSList* generate_ll_old(file_data *file, GSList *node_list)
{
	FILE *fp_tree, *fp_head;
	fp_tree=fopen(file->tree_file_path,"r");
	fp_head=fopen(file->heads_file_path,"r");

    if(fp_tree==NULL || fp_head==NULL) //do check if file exists.
	   {
	   	printf("ERROR: tree or head file does not exist");
	   }
	   
	   log_msg("\nfp_head and fp_tree are not null\n"); 
	   
	   int current_timestamp, child_timestamp=-1;
	   
	   int parent_offset,current_offset=0,child_offset,tmp_int,no_diff,current_type=-1,tagged=-1,current_newtype;
	   
	   fscanf(fp_head,"B_%d %d",&current_timestamp, &current_offset);
	   
	   char *tmp_char=(char *)malloc(255*sizeof(char));
	   char *tag = (char *)malloc(256*sizeof(char));
	   
	   log_msg("\nScanned first line of .head file\n");
	   
		 node_mergedata *node = NULL;
		      
		while (fscanf(fp_head,"B_%d %d",&current_timestamp, &current_offset) != EOF && current_offset != -1)
		{
		//log_msg("current_timestamp %d\n",current_timestamp);
		//log_msg("Scanned a line of .head file\nparent offset %d\ncurrent offset %d\ncurrent tp %d\n",parent_offset,current_offset,current_timestamp);
		    	//move into the tree file
		//log_msg("current_timestamp = %d current_offset = %d\n",current_timestamp,current_offset);	
			do{	
			
				//log_msg("Moving to tree file\n");
			
				fseek(fp_tree,current_offset,SEEK_SET);
		
				fscanf(fp_tree,"%d", &tmp_int);											// Valid,tmp_int
				//log_msg("\n%d\n",tmp_int);
				fscanf(fp_tree,"%d", &current_timestamp);								// Epoch
				//log_msg("\n%d\n",current_timestamp);
				fscanf(fp_tree,"%d", &current_type);									// LO/PO
				//log_msg("\n%d\n",current_type);
				fscanf(fp_tree,"%s", tmp_char);											// Hash
				//log_msg("\n%s\n",tmp_char);
				fscanf(fp_tree,"%s", tag);												// Tag
				//log_msg("\n%s\n",tag);
				fscanf(fp_tree,"%d", &no_diff);											// #diff
				//log_msg("\n%d\n",no_diff);
				fscanf(fp_tree,"%d", &parent_offset);									// Parent offset
				//log_msg("\n%d\n",parent_offset);
				
				log_msg("%d %d %d %s %s %d %d\n",tmp_int,current_timestamp,current_type,tmp_char,tag,no_diff,parent_offset);
				
				if(node != NULL)
				{
					//log_msg("\n Node is not NULL, hence need to set parent_type as %d\n",current_newtype);
				
					node->parent_type = current_newtype;
					node = NULL;
				}
				
				if(strcmp(tag,"_")==0)
					tagged = 0;
				else tagged = 1;
				
				current_newtype = type(current_type,tagged);
				
				if((child_timestamp!=-1) && current_newtype == 1)
				{
				
					log_msg("Creating a node to add to the linked list\n");
				
					node = (node_mergedata *)malloc(sizeof(node_mergedata));
					
					node->current_offset = current_offset;
					node->child_offset = child_offset;	
					node->current_timestamp = current_timestamp;
					node->child_timestamp = child_timestamp;		
					node->current_type = current_newtype;
					node->mergedata = (float)no_diff/(child_timestamp - current_timestamp);	
					
					if(g_slist_find(node_list, node) != NULL)
					{
						log_msg("Item already present in list\n");
					}
					else{
						node_list = g_slist_append(node_list, node);
						log_msg("Added item to list\n");
					}
					
				}
				child_timestamp = current_timestamp;		// Store current timestamp before moving to parent	
				child_offset = current_offset;
				current_offset = parent_offset;		  			// move to the parent
				
				//log_msg("Now move to parent -> parent_offset = %d\n",parent_offset);
				
			}while(parent_offset != -1);
			
			//log_msg("Finished one branch\n");
			
	   	}
	   	
	log_msg("\n Out of while loop\n\n");
	   	
	fclose(fp_tree);
	fclose(fp_head);
	
	return node_list;
	
}
void cleanFile(char *filepath)
{
	log_msg("Reached to clean file\n");
}




