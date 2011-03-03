vfs : vfs.o log.o
	gcc -g `pkg-config fuse --libs` -o vfs vfs.o log.o

vfs.o : vfs.c log.h params.h
	gcc -g -Wall `pkg-config fuse --cflags` -c vfs.c

log.o : log.c log.h params.h
	gcc -g -Wall `pkg-config fuse --cflags` -c log.c

clean:
	rm -f vfs *.o
