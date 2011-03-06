all: 	test1

test1 : test1.o  testbench.o versioning.o fuse_wrapper.o versioning_utils.o
			gcc -Wall test1.o  testbench.o versioning.o fuse_wrapper.o versioning_utils.o -o test1

test1.o : test1.c
			gcc -Wall -c -g test1.c

testbench.o : testbench.c
			gcc -Wall -c -g testbench.c

versioning.o : ../src/versioning.c
			gcc -Wall -c -g ../src/versioning.c
			
versioning_utils.o : ../src/versioning_utils.c
			gcc -Wall -c -g ../src/versioning_utils.c

fuse_wrapper.o : ../src/fuse_wrapper.c
			gcc -Wall -c -g ../src/fuse_wrapper.c

clean:
			rm -rf *.o ../src/versioning.o ../src/fuse_wraper.o test1
