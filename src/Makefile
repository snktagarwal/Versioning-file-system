vfs : vfs.o log.o versioning.o vfs_utils.o versioning_utils.o fuse_wrapper.o\
	obj_md.o tree.o heads.o cleanup.o format.o
	gcc -g `pkg-config fuse glib-2.0 --libs` -o vfs vfs.o log.o versioning.o vfs_utils.o fuse_wrapper.o versioning_utils.o obj_md.o tree.o heads.o cleanup.o format.o

vfs.o : vfs.c log.h params.h vfs.h
	gcc -g -Wall `pkg-config fuse glib-2.0 --cflags` -c vfs.c

log.o : log.c log.h params.h
	gcc -g -Wall `pkg-config fuse glib-2.0 --cflags` -c log.c

vfs_utils.o: vfs_utils.c vfs.h
	gcc -g -Wall `pkg-config fuse glib-2.0 --cflags` -c vfs_utils.c

versioning.o : versioning.c vfs.h
	gcc -g -Wall `pkg-config fuse glib-2.0 --cflags` -c versioning.c
			
versioning_utils.o : versioning_utils.c vfs.h
	gcc -g -Wall `pkg-config fuse glib-2.0 --cflags` -c versioning_utils.c

fuse_wrapper.o : fuse_wrapper.c fuse_wrapper.h vfs.h
	gcc -g -Wall `pkg-config fuse glib-2.0 --cflags` -c fuse_wrapper.c
obj_md.o: obj_md.c vfs.h
	gcc -g -Wall `pkg-config fuse glib-2.0 --cflags` -c obj_md.c
heads.o: heads.c vfs.h
	gcc -g -Wall `pkg-config fuse glib-2.0 --cflags` -c heads.c
tree.o: tree.c vfs.h
	gcc -g -Wall `pkg-config fuse glib-2.0 --cflags` -c tree.c
cleanup.o: cleanup.c vfs.h
	gcc -g -Wall `pkg-config fuse glib-2.0 --cflags` -c cleanup.c
format.o: format.c vfs.h
	gcc -g -Wall `pkg-config fuse glib-2.0 --cflags` -c format.c	
clean:
	rm -f vfs *.o
