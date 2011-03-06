#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>

#include "testbench.h"

int main(){
	system ("rm -r root_test1");
	system ("mkdir root_test1");

	/* Test calls */
	mkdir_ ("root_test1/dir1");
	mkdir_ ("root_test1/dir1/.ver");

	writefile ("root_test1/dir1/file1.txt", "a\nb\nc\n",0);
	
	writefile ("root_test1/dir1/file1.txt", "a\nb\nd\nc\n",1);

	writefile ("root_test1/dir1/file1.txt", "a\nb\nd\nc\ne\n",1);
	
	writefile ("root_test1/dir1/file1.txt", "a\nb\nd\nc\ne\nf\n",1);

	writefile ("root_test1/dir1/file1.txt", "a\nb\nd\nc\ne\nf\ng\n",1);

	/*writefile ("root_test1/dir1/file2.txt", "x\ny\nz\n");

	writefile ("root_test1/file3.txt", "1\n2\n3\n");
	deletefile ("root_test1/file3.txt");*/

	return 0;
}
