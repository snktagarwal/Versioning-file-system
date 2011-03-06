vfs : vfs.o log.o versioning.o vfs_utils.o versioning_utils.o fuse_wrapper.o
	gcc -g `pkg-config fuse glib-2.0 --libs` -o vfs vfs.o log.o versioning.o vfs_utils.o fuse_wrapper.o versioning_utils.o

vfs.o : vfs.c log.h params.h vfs.h
	gcc -g -Wall `pkg-config fuse glib-2.0 --cflags` -c vfs.c

log.o : log.c log.h params.h
	gcc -g -Wall `pkg-config fuse glib-2.0 --cflags` -c log.c

vfs_utils.o: vfs_utils.c vfs.h
	gcc -g -Wall `pkg-config fuse glib-2.0 --cflags` -c vfs_utils.c

versioning.o : versioning.c versioning.h
	gcc -g -Wall `pkg-config fuse glib-2.0 --cflags` -c versioning.c
			
versioning_utils.o : versioning_utils.c versioning_utils.h
	gcc -g -Wall `pkg-config fuse glib-2.0 --cflags` -c versioning_utils.c

fuse_wrapper.o : fuse_wrapper.c fuse_wrapper.h
	gcc -g -Wall `pkg-config fuse glib-2.0 --cflags` -c fuse_wrapper.c

clean:
	rm -f vfs *.o
