# Makefile to build the binaries( install manually for now )

all: checkout clean lsver revert tag __guidata __guiswitch

checkout: checkout.c
	gcc -g checkout.c -o checkout

clean: clean.c
	gcc -g clean.c -o clean

__guidata: __guidata.c
	gcc -g __guidata.c -o __guidata

lsver: lsver.c
	gcc -g lsver.c -o lsver

revert: revert.c
	gcc -g revert.c -o revert

tag: tag.c
	gcc -g tag.c -o tag

__guiswitch: __guiswitch.c
	gcc -g __guiswitch.c -o __guiswitch

cleanup:
	rm checkout clean lsver revert tag __guidata __guiswitch

install:
	sudo cp checkout clean lsver revert tag __guidata __guiswitch /usr/bin
