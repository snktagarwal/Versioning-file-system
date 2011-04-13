# Makefile to build the binaries( install manually for now )

all: checkout cleanup lsver revert tag __guidata __guiswitch __guimd

checkout: checkout.c
	gcc -g checkout.c -o checkout

cleanup: cleanup.c
	gcc -g cleanup.c -o cleanup

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

__guimd: __guimd.c
	gcc -g __guimd.c -o __guimd

clean:
	rm checkout cleanup lsver revert tag __guidata __guiswitch __guimd

install:
	sudo cp checkout cleanup lsver revert tag __guidata __guiswitch __guimd /usr/bin/

uninstall:
	cd /usr/bin/; rm checkout cleanup lsver revert tag __guidata __guiswitch __guimd
