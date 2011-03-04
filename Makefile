all: test1

test1 : test/test1.o  testbench.o versioning.o
			gcc -Wall test/test1.o  testbench.o versioning.o -o test/test1
test1.o : test.c
			gcc -Wall -c -g test.c

testbench.o : testbench.c
			gcc -Wall -c -g testbench.c

versioning.o : versioning.c
			gcc -Wall -c -g versioning.c

clean:
			rm -rf *.o test/*.o test1
