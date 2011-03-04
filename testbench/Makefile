all: test1

test1 : test1.o  testbench.o
			gcc -Wall test1.o  testbench.o versioning.o -o test1
test1.o : test1.c
			gcc -Wall -c -g test1.c

testbench.o : testbench.c
			gcc -Wall -c -g testbench.c

clean:
			rm -rf *.o test1
