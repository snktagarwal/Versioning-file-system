#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <glib.h>

#include "vfs.h"
#include "fuse_wrapper.h"

#define HASH_PATH 1
#define HASH 0

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

/* Returns the type based on the following format 
 * 1 = PO + untagged
 * 2 = PO + tagged
 * 3 = LO + untagged
 * 4 = LO + tagged
 */

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

/* Iterator to print out a linked list consisting of node_mergedata struct */

void print_iterator(gpointer item) {

	log_msg("current offset %d child offset %d current tp %d child tp %d current_type %d parent_type %d mergedata %f nearest lo %d\n",((node_mergedata*)item)->current_offset,((node_mergedata*)item)->child_offset,((node_mergedata*)item)->current_timestamp,((node_mergedata*)item)->child_timestamp,((node_mergedata*)item)->current_type,((node_mergedata*)item)->parent_type,((node_mergedata*)item)->mergedata,((node_mergedata*)item)->nearest_lo_offset);
 
}

/* Function to compare node_mergedata based on timestamp (to check if equal) */

gint compare_node_mergedata(gconstpointer item1, gconstpointer item2) {
		
		if(((node_mergedata*)item1)->current_timestamp == ((node_mergedata*)item2)->current_timestamp && ((node_mergedata*)item1)->child_timestamp == ((node_mergedata*)item2)->child_timestamp)
			return 1;
		else 
			return 0;		 
}

gint sort_node_mergedata(gconstpointer item1, gconstpointer item2) {
		
		if(((node_mergedata*)item1)->mergedata < ((node_mergedata*)item2)->mergedata)
			return 0;
		else 
			return 1;		 
}

/* Function that generates the linked list of nodes that might be deleted/merged.
*	Should not return root as it should never be deleted
*/

GSList *generate_ll(file_data *file,GSList* node_list)
{
	FILE *fp_tree, *fp_head;
	fp_tree=fopen(file->tree_file_path,"r");
	fp_head=fopen(file->heads_file_path,"r");
	
	int current_timestamp, child_timestamp=-1;
	int parent_offset,child_offset,tmp_int,no_diff,current_type=-1,tagged=-1,current_newtype,nearest_lo_offset;
	
	int i;
	
	//list *head_list = newlist();
	//list *lo_list = newlist();
	
	guint n;
	
	GSList *head_list = NULL; 
	GSList *lo_list = NULL;
	GSList *iterator = NULL;
	GSList *iterator1 = NULL;


    if(fp_tree==NULL || fp_head==NULL) //do check if file exists.
	   {
	   	printf("ERROR: tree or head file does not exist");
	   }
	   
	//log_msg("\nfp_head and fp_tree are not null\n"); 
	
	int current_offset = 0;
	   
	char *tp = (char *)malloc(15*sizeof(char));
	   
	   //fseek(fp_head,0,SEEK_SET);
	   
	fscanf(fp_head,"%s %d",tp, &current_offset);
	   
	//log_msg("\nScanned first line of .head file\n");
	
	g_slist_foreach(head_list, (GFunc)print_head_iterator, NULL);
	
	//log_msg("end of head list\n");
	
	int c=0;
	
	while(fscanf(fp_head,"%s %d",tp,&current_offset) != EOF)
	{
		c++;
		//log_msg("%d: current_timestamp %s current_offset %d\n",c,tp,current_offset);
		head_list = g_slist_prepend(head_list, current_offset);
		//add(head_list,current_offset);
		//log_msg("vinay %d",*(&current_offset));
	}
	
	
	
	//log_msg("\nhead_list:\n");
	
/*	for (iterator = head_list; iterator; iterator = iterator->next) {*/
/*	  		log_msg("Current offset is '%d'\n", *((int*)iterator));*/
/*	  		*/
/* 		}*/

	//g_slist_foreach(head_list, (GFunc)print_head_iterator, NULL);
	
	//log_msg("vinayvinay\n");
	
	//printlist(head_list);
	
	//return NULL;
	
	//log_msg("\nhead_list:\n");
	//g_slist_foreach(head_list, (GFunc)print_head_iterator, NULL);
	//g_slist_foreach(head_list, (GFunc)move_to_lo_iterator,NULL);
	
	node_head_list *headnode = NULL;
	
	int v =0;
	
	while(head_list != NULL )
	{
		//log_msg("\nchecked if head_list == NULL\n");
		
		v++;
		
	  	//log_msg("Current offset is '%d'\n", *((int*)(iterator)));
	  		
	  	//return NULL;
	  		
	  	//node_list = move_to_lo_iterator(*((int*)(iterator)),fp_tree,node_list,lo_list);
	  	
	  	current_offset = *((int*)(head_list));
	  		
	  	nearest_lo_offset = current_offset;
	
		node_mergedata *node = NULL;
	
		char *tmp_char=(char *)malloc(255*sizeof(char));
		char *tag = (char *)malloc(256*sizeof(char));
	
		do{				
			// log_msg("Moving to tree file\n");
		
			fseek(fp_tree,current_offset,SEEK_SET);
		
			fscanf(fp_tree,"%d", &tmp_int);											// Valid,tmp_int
			fscanf(fp_tree,"%d", &current_timestamp);								// Epoch
			fscanf(fp_tree,"%d", &current_type);									// LO/PO
			fscanf(fp_tree,"%s", tmp_char);											// Hash
			fscanf(fp_tree,"%s", tag);												// Tag
			fscanf(fp_tree,"%d", &no_diff);											// #diff
			fscanf(fp_tree,"%d", &parent_offset);									// Parent offset
				
			if(strcmp(tag,"_")==0)
				tagged = 0;
			else tagged = 1;
			
			
				
			current_newtype = type(current_type,tagged);
			
			if(current_offset == 0)
				current_newtype =-1;
			
			if(node != NULL)
			{
				//log_msg("\n Node is not NULL, hence need to set parent_type as %d\n",current_newtype);
				
				node->parent_type = current_newtype;
				node = NULL;
			}
				
			if(current_type == LO && current_offset != nearest_lo_offset)
			{
				//log_msg("\nEncountered LO\n");
				headnode = (node_head_list*)malloc(sizeof(node_head_list));
				headnode -> offset = current_offset;
	
				if(g_slist_find(head_list, current_offset) == NULL )
				{
				
					head_list = g_slist_append((head_list),current_offset);
					//log_msg("Added item to headlist %d\n",current_offset);
					
				}
				else{	
					//log_msg("LO already present in list\n");
					}
				break;
				
			}
					
				
			
				
			if((child_timestamp!=-1) && current_newtype == 1 && current_offset != 0)
			{
				
				//log_msg("Creating a node to add to the linked list\n");
				
				node = (node_mergedata *)malloc(sizeof(node_mergedata));
					
				node->current_offset = current_offset;
				node->child_offset = child_offset;	
				node->current_timestamp = current_timestamp;
				node->child_timestamp = child_timestamp;		
				node->current_type = current_newtype;
				node->mergedata = (float)no_diff/(child_timestamp - current_timestamp);	
				node->nearest_lo_offset = nearest_lo_offset;
				
					
				//log_msg("\nAdding node to node list\n");
				node_list = g_slist_prepend(node_list, node);
				//print_iterator(node);
					
			}
			
			//log_msg("Now move from current %d -> parent  %d\n",current_offset,parent_offset);
			
			child_timestamp = current_timestamp;		// Store current timestamp before moving to parent	
			child_offset = current_offset;
			current_offset = parent_offset;		  			// move to the parent
				
			
					
				
		}while(parent_offset != -1);
			
		
		head_list = g_slist_remove_link (head_list,(g_slist_nth (head_list,0)));
		  		
	}
	   	
	fclose(fp_tree);
	fclose(fp_head);
	
	return node_list;
	
}

void print_head_iterator(gpointer item) {
	log_msg("%d\n", item);
}

/* Function that gains control from cleandisc() in vfs.c */

void cleanFile(char *filepath, double ratio)
{
	log_msg("Reached to clean file ratio = %f\n",ratio);
	
	file_data * file = construct_file_data(filepath);
	
	cleanupfile(file,ratio);
	
}

/* Function that does the actual cleaning up of the file specified */

void cleanupfile(file_data *file,float required_ratio)
{

	int act_file_size;
	float ratio;
	
	//log_msg("vinay %f\n",ratio);
	
	int md_size;
	GSList *node_list = NULL;
	
	//log_msg("%f\n",reqdratio);
	
	
	node_list = generate_ll(file,node_list);
	   	
	node_list = g_slist_sort(node_list, (GCompareFunc)sort_node_mergedata);
	
	g_slist_foreach(node_list, (GFunc)print_iterator, NULL);
		
	FILE *file_r;
	
	file_r=fopen(file->md_data_file_path,"r");
	
	int tmp; 
	while(fscanf(file_r,"%d",&tmp) != EOF)
	{
		fscanf(file_r,"%d",&tmp);
		fscanf(file_r,"%d",&tmp);
		fscanf(file_r,"%f",&ratio);
		log_msg("Ratio:::::%f",ratio);
	}
	fclose(file_r);
	
	//g_slist_foreach(node_list, (GFunc)g_free, NULL);
	log_msg("\nbefore while loop ratio=%f  rewuired ratio=%f",ratio,required_ratio);
	
	//while(node_list != NULL)//some criterea is satisfied.
	if(ratio<required_ratio)
	{
		while(ratio<required_ratio && node_list->next!=NULL)
		{
	
			//check if diff is to be merged.
			if(((node_mergedata*)(g_slist_nth_data (node_list,0)))->parent_type==1 || (((node_mergedata*)(g_slist_nth_data (node_list,0)))->parent_type==2 ))
			{ 
		
				log_msg("\n entering merge_diff	\n");	
				if(node_list == NULL)
				{
					log_msg("\n node_list is null \n");
					break;
				}
				node_list = merge_diff(((node_mergedata*)(g_slist_nth_data (node_list,0))),file,node_list);
				log_msg("\n exiting merge_diff \n");

				//calculae the size of the new diff 
			}
			else
			{	
				log_msg("\n entering delete_diff	\n");	
				//node_list=node_list->next;		
				node_list=delete_diff(((node_mergedata*)(g_slist_nth_data (node_list,0))),file,node_list);
				log_msg("\n exiting delete_diff \n");
				//simplly subtract the size of the diff

				//update the linked list	

			}
			node_list=NULL;
			
			node_list = generate_ll(file,node_list);
	   	
			node_list = g_slist_sort(node_list, (GCompareFunc)sort_node_mergedata);
	
			g_slist_foreach(node_list, (GFunc)print_iterator, NULL);
		
			if(node_list==NULL)
			{
			log_msg("\nfile cannot be cleaned further\n");
			break;
			}
		
	
	
	
			log_msg("\nentering the last\n ");
			md_size=calc_md_size(file);
			log_msg("\nentering the last 1\n ");
			//get the actual file size,read from heads file.
			//char *head=(char *)malloc(PATH_MAX*sizeof(char));
	
			//strcpy(file->heads_file_path,head);
			//FILE *head_file;
			//int offset;
			//head_file=fopen(file->heads_file_path,"r");
			//fscanf(head_file,"%s",head);
			//fscanf(head_file,"%d",&offset);
			//fclose(head_file);
			//log_msg("\nentering the last 2\n ");
	
	
			//getFileHashPath(offset,head,file); //head here will after this function call store the hash
	
			log_msg("\nentering the last 3\n ");
			act_file_size=calc_file_size(file->path); //calculating current heads size.
			//update ratio
			ratio=(float)act_file_size/(md_size+act_file_size);
	
			log_msg("\nentering the last 1 %f \n ",ratio);
	
		} 
		FILE *md_file;
		//strcpy(file->md_data_file_path,head);
		md_file=fopen(file->md_data_file_path,"a");
		int t=(int)time(NULL);
		fprintf(md_file,"%d ",t);
		fprintf(md_file,"%d ",act_file_size);
		fprintf(md_file,"%d ",md_size);
		fprintf(md_file,"%f\n",ratio);
		// Send list to fuddi
		//g_slist_foreach(node_list, (GFunc)g_free, NULL);
	
		fclose(md_file);
	
		if(node_list!=NULL)
		{

		g_slist_free(node_list);
		}
	}
}


//this makes full_hash_path refer to thehash apth of the correstponding offset and also returns the parent offset

int getFileHashPath(int offset, char *full_hash_path, file_data *f1,int return_mode)
{
	FILE *f;
	f=fopen(f1->tree_file_path,"r");
	fseek(f,offset,SEEK_SET);
	
	
	//get file hash
	int valid, tp, lp, diff_ct, p_off;
	char* hash=(char *)malloc(50*sizeof(char));
	char* tag=(char *)malloc(255*sizeof(char));
	fscanf(f,"%d",&valid);
	if(valid==0)
	{
		printf("ERROR: NOT IMPOSSIBLE\n");
		return -1;
	}
	fscanf(f,"%d %d %s %s %d %d",&tp, &lp, hash, tag, &diff_ct, &p_off);
	fclose(f);
	if(return_mode ==0)
	strcpy(full_hash_path,hash);
	//calculate absolute path of the hash file.
	else
	{	strcpy(full_hash_path, f1->objects_dir_path);
	
	//now append objects and the file name to full_hash_path
		log_msg("\n getr hash file parent offset %d %d %d %s %s %d %d \n",valid,tp,lp,hash,tag,diff_ct,p_off);
	strcat(full_hash_path,hash);
	
	}
	return p_off;
	
		
}




//function to merge two diffs.takes offset of the current and it's parent as parameter.
// f is the pointer to the tree file.
GSList *merge_diff(node_mergedata *node,file_data *f,GSList *node_list)
{
	if(node_list->next == NULL)
	{
	log_msg("\n\nfurther cleaning not possible\n\n");
	return NULL;
	}
	
	log_msg("\nentering merge\n");

	log_msg(" current offset %d",node->current_offset);

	//log_msg(" \n 121212 %d \n ",((node_mergedata *)node_list)->current_offset);

	//log_msg("current_offset %d",(node_mergedata(g_slist_nth_data (node_list,0)))->current_offset);

	log_msg("\n current offset %d child offset %d current tp %d child tp %d current_type %d parent_type %d mergedata %f nearest lo %d\n",((node_mergedata*)(g_slist_nth_data (node_list,0)))->current_offset,((node_mergedata*)(g_slist_nth_data (node_list,0)))->child_offset,((node_mergedata*)(g_slist_nth_data (node_list,0)))->current_timestamp,((node_mergedata*)(g_slist_nth_data (node_list,0)))->child_timestamp,((node_mergedata*)(g_slist_nth_data (node_list,0)))->current_type,((node_mergedata*)(g_slist_nth_data (node_list,0)))->parent_type,((node_mergedata*)(g_slist_nth_data (node_list,0)))->mergedata,((node_mergedata*)(g_slist_nth_data (node_list,0)))->nearest_lo_offset);


	int nearest_offset=node->nearest_lo_offset;
	int current_offset=node->current_offset;
	
	
	char *tree_path=(char *)malloc(PATH_MAX*sizeof(char));
	char *object_path=(char *)malloc(PATH_MAX*sizeof(char));
	strcpy(object_path, f->objects_dir_path);
	strcpy(tree_path, f->tree_file_path);
	
	
	log_msg("\n tree_file_path %s \n",f->tree_file_path);
	
	log_msg("\n object_path %s \n",f->objects_dir_path );
	
	log_msg("\n nearest_offset %d \n current_offset %d \n",nearest_offset,current_offset);

	int c_off = create_file_fromlo_cleanup(tree_path,current_offset,nearest_offset,object_path,1);
	
	log_msg("\n cheild %d\n ",c_off);
	
	char *tmp_filepath=(char *)malloc(PATH_MAX*sizeof(char));
	char *tmp_filepath2=(char *)malloc(PATH_MAX*sizeof(char));
	char *tmp_filepath3=(char *)malloc(PATH_MAX*sizeof(char));
	strcpy(tmp_filepath, "/tmp/rvfs/switchC");
	strcpy(tmp_filepath2, "/tmp/rvfs/switchP");
	strcpy(tmp_filepath3, "/tmp/rvfs/switchdiff");
	//now create the diff at the temp place
		
	
	log_msg("\nstage 1\n");
	//now calculate the diff and even edits the tree and OBJ_MD file
	diff(tmp_filepath,tmp_filepath2,tmp_filepath3);

	
	char * a = (char *)malloc(50*sizeof(char));
	find_SHA(tmp_filepath3, a);
	strcpy(tmp_filepath, object_path);

	
	strcat(tmp_filepath,a);
	//write a to the tree file.
	
	log_msg("\nstage 2\n");
	
	copy(tmp_filepath3,tmp_filepath);

	compress(tmp_filepath);
	
	char * rem_obj = (char *)malloc(PATH_MAX*sizeof(char));
	
	
	
	int parent_offset = getFileHashPath(current_offset, rem_obj, f,1);
	delete(rem_obj);
	getFileHashPath(current_offset, rem_obj, f,0);
	update_objmd_file(rem_obj,f->OBJ_MD_file_path,1);
	
	
	
	getFileHashPath(parent_offset,rem_obj,f,1);
	
	log_msg(" \n path of rem_onj \n %s",rem_obj);
	
	delete(rem_obj);
	getFileHashPath(parent_offset,rem_obj,f,0);
	update_objmd_file(rem_obj,f->OBJ_MD_file_path,1);
	
	log_msg("\n parent offset %d \n",parent_offset);
	FILE *f_new;
	f_new=fopen(f->tree_file_path,"r+");
	
	if(parent_offset==-1)
	{
		printf("error: you cannot go above that.\n");
		return NULL;
	}

	fseek(f_new,parent_offset,SEEK_SET);
	//now write to the tree file the name of the hash
	int valid, tp, lp, p_off;
	char* hash=(char *)malloc(50*sizeof(char));
	char* tag=(char *)malloc(255*sizeof(char));
	char* diff_ct=(char *)malloc(15*sizeof(char));

	log_msg("\nstage 3\n");
	fscanf(f_new,"%d",&valid);
	if(valid==0)
	{
		printf("ERROR: NOT IMPOSSIBLE\n");
		return NULL;
	}
	
	log_msg("\n new hash %s \n",a);
	
	fscanf(f_new,"%d %d",&tp, &lp);
	fprintf(f_new," %s",a);
	
	
	fseek(f_new, c_off, SEEK_SET);
	fscanf(f_new,"%d",&valid);
	if(valid==0)
	{
		printf("ERROR: NOT IMPOSSIBLE\n");
		return NULL;
	}
	fscanf(f_new,"%d %d %s %s %s",&tp, &lp, hash, tag, diff_ct);
	
	log_msg("\n %d %d %d %s %s %s \n",valid, tp, lp,hash, tag, diff_ct);
	
	fseek(f_new,10 - strlen(diff_ct),SEEK_CUR);
	char * p_offstr = (char *)malloc(12*sizeof(char));
	
	log_msg("\n %d parent offset \n",parent_offset);
	
	itoa(parent_offset, p_offstr);
	
	log_msg("\n %s parent offset \n",p_offstr);
	
	fprintf(f_new, " %s", addspaces(p_offstr, 10));
	
	
	fseek(f_new,current_offset,SEEK_SET);
	fprintf(f_new,"0");
	fclose(f_new);
	
	log_msg("invalidates the entry");
	

	char *tmp_char=(char *)malloc(PATH_MAX*sizeof(char));
		//if tagged then donot updata the merge data
		if(node->parent_type==1 && node->parent_type!=-1 )
		{
			GSList *tmp_list;
			tmp_list=node_list; //tmp points to the same place as node
			
		/*	log_msg("\n %d current offset \n",((node_mergedata*)(g_slist_nth_data (node_list,0)))->current_offset);
			
			//log_msg("\n   11111  forward \n tmp_list current_offset %d:\n",((node_mergedata *)node_list)->current_offset);
			node_list=node_list->next;
			
			log_msg("\n %d current offset \n",((node_mergedata*)(g_slist_nth_data (node_list,0)))->current_offset);
			
			//node_mergedata *tmp;
			*/
			
			
			log_msg("\n entered this function\n");
			//find node with offset=parent_offset;
			while(tmp_list!=NULL)
			{
				tmp_list=tmp_list->next;
				
				//tmp=(node_mergedata *)tmp_list;	
				
				//log_msg("\n moving forward \n tmp_list current_offset %d:\n",tmp->current_offset);
				
				//tmp=(node_mergedata *)tmp_list;
				if(tmp_list == NULL )
				{
				break;
				}
				else
				{
				log_msg("\n moving forward \n tmp_list current_offset %d:\n",((node_mergedata*)(g_slist_nth_data (tmp_list,0)))->current_offset);
				
				if(((node_mergedata*)(g_slist_nth_data (tmp_list,0)))->current_offset == parent_offset)
					{
					log_msg("\n entering \n \n");
					((node_mergedata*)(g_slist_nth_data (tmp_list,0)))->child_timestamp=node->child_timestamp;
					getFileHashPath(((node_mergedata*)(g_slist_nth_data (tmp_list,0)))->current_offset,tmp_char,f,1);
	
					//calculate size of the new metadeta and update mergedata.
					int time=-((((node_mergedata*)(g_slist_nth_data (tmp_list,0)))->current_timestamp)-(node->child_timestamp));
					((node_mergedata*)(g_slist_nth_data (tmp_list,0)))->mergedata=(float)calc_file_size(tmp_char)/time;
				
					int counter=0;
					log_msg("\n time = %d \n ", time );
					scanf("%d",&counter);
				
					log_msg("\n getting out from while loop\n");
					break;
					}
				}
			}
			
				
		}
		
		log_msg("\n deleting\n");
		
		node_list=g_slist_delete_link(node_list,(g_slist_nth (node_list,0)));
		log_msg("\n deleted\n");
		
		node_list=g_slist_sort(node_list,(GCompareFunc)sort_node_mergedata);	
		node=(node_mergedata *)node_list;
		
		log_msg("\n printing linked list \n");
		
		g_slist_foreach(node_list, (GFunc)print_iterator, NULL);
		return node_list;
		
}




// d;etes diff, takes offset of the current node as parameter.
GSList *delete_diff(node_mergedata *node,file_data *file,GSList *node_list)
{ 
	if(node_list->next == NULL)
		{
		log_msg("\n\nfurther cleaning not possible\n\n");
		return NULL;
		}	
	
	int tmp_offset=node->current_offset;
	char *hash_path=(char *)malloc(PATH_MAX*sizeof(char));
	int p_off=getFileHashPath(tmp_offset, hash_path, file,1);
	
	delete(hash_path);
    p_off=getFileHashPath(tmp_offset, hash_path, file,0);
	update_objmd_file(hash_path,file->OBJ_MD_file_path, 1);
	
	
	int valid, tp, lp, p1_off;
	
	int nearest_offset=node->nearest_lo_offset;
	int current_offset=node->current_offset;
	
	char* hash=(char *)malloc(50*sizeof(char));
	char* tag=(char *)malloc(255*sizeof(char));
	char* diff_ct=(char *)malloc(15*sizeof(char));
	
	
	
	char *tree_path=(char *)malloc(PATH_MAX*sizeof(char));
	char *diff_str=(char *)malloc(15*sizeof(char));
	char *object_path=(char *)malloc(PATH_MAX*sizeof(char));
	strcpy(object_path, file->objects_dir_path);
	strcpy( tree_path, file->tree_file_path);
	
	int c_off = create_file_fromlo_cleanup(tree_path,current_offset,nearest_offset,object_path,2);
	
	log_msg("\n child offset %d \n",c_off);
	log_msg("\nparent offset %d\n",p_off);
	
	
	FILE *f;
	f=fopen(tree_path,"r+");
	fseek(f,c_off,SEEK_SET);
	
	fscanf(f,"%d",&valid);
	if(valid==0)
	{
		printf("ERROR: NOT IMPOSSIBLE\n");
		return NULL;
	}
	fscanf(f,"%d %d %s %s %s",&tp, &lp, hash, tag, diff_ct);
	fseek(f,10 - strlen(diff_ct),SEEK_CUR);
	sprintf(diff_str,"%d",p_off);
	fprintf(f, " %s", addspaces(diff_str,10));
	
	
	
	fseek(f,tmp_offset,SEEK_SET);
	fprintf(f,"0");
	
//	fseek(f,c_off,SEEK_SET);
	
	fclose(f);
	
	
	node_list=g_slist_delete_link(node_list,node_list);
	
	
	if(node_list != NULL)
	{
	g_slist_foreach(node_list, (GFunc)print_iterator, NULL);
	}
	else
	{
	log_msg("\n node list is null \n");
	}
	//remove the hashfile

	//invalidate the entry in the tree file
	return node_list;
	//remove the corresponding entry in the OBJ_MD file
}

/*


int cleanupfile(file_data *file)
{

	GSList *node_list;
	node = generate_ll(file,node);
	
	
	
	

	while(node != NULL)//some criterea is satisfied.
	{
		//check if diff is to be merged.
		if(node->parent_type==1 || node->parent_type==2 )
		{
			merge_diff(node,file);

			//calculae the size of the new diff 
		}
		else
		{
			delete_diff(node,file);

			//simplly subtract the size of the diff

			//update the linked list	

		}
		
	
	int md_size=calc_md_size(file);
	//get the actual file size,read from heads file.
	char *head=(char *)malloc(PATH_MAX*sizeof(char));
	strcpy(file->heads_file_path,head);
	FILE *head_file;
	int offset;
	head_file=fopen(head,"r");
	fscanf(head_file,"%s",head);
	fscanf(head_file,"%d",&offset);
	fclose(head_file);
	
	
	getFileHashPath(offset,head,file); //head here will after this function call store the hash
	int act_file_size=calc_file_size(head); //calculating current heads size.
	//update ratio
	float ratio=(float)md_size/act_file_size;
	
	FILE *md_file;
	strcpy(file->md_data_file_path,head);
	md_file=fopen(head,"r+");
	int tmp;
	fscanf(md_file,"%d",&tmp);
	fprintf(md_file,"%d",act_file_size);
	fprintf(md_file,"%d",md_size);
	fprintf(md_file,"%f",ratio);

	} 
}
*/


int create_file_fromlo_cleanup(char * file_tree_path, int d_off, int lo_off, char * obj_dir_path, int mode)         // returns -1 for no success and offset of child for success
{
	FILE * fp = fopen(file_tree_path, "r");
	fseek(fp, lo_off, SEEK_SET);
	int valid, off = lo_off;
	fscanf(fp, "%d", &valid);
	if(valid==0)
	{
		printf("ERROR: NOT POSSIBLE.\n");
		return -1;
	}
	
	int tp, lp, diff_ct, p_off;
	char * hash = (char *)malloc(50*sizeof(char));
	char * tag = (char *)malloc(255*sizeof(char));
	
	fscanf(fp, "%d %d %s %s %d %d", &tp, &lp, hash, tag, &diff_ct, &p_off);
	
	char * temp_file = (char *)malloc(PATH_MAX*sizeof(char));
	mkdir("/tmp/rvfs", (mode_t)0755);
	strcpy(temp_file, "/tmp/rvfs/switchC");                                    // switchC stores the file of the child
	char * curr_file = (char *)malloc(PATH_MAX*sizeof(char));
	strcpy(curr_file, obj_dir_path);
	
	strcat(curr_file, hash);
	
	if(mode==1)
	{
		decompress(curr_file);
		copy(curr_file, temp_file);
		compress(curr_file);
	}
	
	while(p_off!=d_off)
	{
		off = p_off;
		fseek(fp, off, SEEK_SET);
		fscanf(fp, "%d", &valid);
		if(valid==0)
		{
			printf("ERROR: NOT POSSIBLE.\n");
			return -1;
		}
		
		fscanf(fp, "%d %d %s %s %d %d", &tp, &lp, hash, tag, &diff_ct, &p_off);
		int i = strlen(curr_file);
		while(curr_file[i]!='/')
			i--;
		curr_file[i+1] = '\0';
		strcat(curr_file, hash);
		
		if(mode==1)
		{
			decompress(curr_file);
			if(lp==0)
				copy(curr_file, temp_file);
			else
				patch(temp_file, curr_file);
			compress(curr_file);
		}
	}
	char * temp_file1 = (char *)malloc(PATH_MAX*sizeof(char));
	strcpy(temp_file1, "/tmp/rvfs/switchP");
	if(mode==1)
		copy(temp_file, temp_file1);
	
	int j;
	for(j=0; j<2; j++)
	{
		fseek(fp, p_off, SEEK_SET);
		fscanf(fp, "%d", &valid);
		if(valid==0)
		{
			printf("ERROR: NOT POSSIBLE.\n");
			return -1;
		}
		fscanf(fp, "%d %d %s %s %d %d", &tp, &lp, hash, tag, &diff_ct, &p_off);
		int i = strlen(curr_file);
		while(curr_file[i]!='/')
			i--;
		curr_file[i+1] = '\0';
		strcat(curr_file, hash);
		
		if(mode==1)
		{
			decompress(curr_file);
			if(lp==0)
				copy(curr_file, temp_file1);
			else
				patch(temp_file1, curr_file);
			compress(curr_file);
		}	
	}
	
	fclose(fp);
	return off;
}






