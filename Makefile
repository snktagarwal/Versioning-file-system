all:
	cd src/; make
	cd ui/timeline/; qmake-qt4 -project; qmake-qt4 timeline.pro; make
	cd ui/piechart/; qmake-qt4 -project; qmake-qt4 piechart.pro; make
	cd binaries/; make

install:
	cd src/; make install
	cd ui/timeline/; cp timeline /usr/bin/
	cd ui/piechart/; cp piechart /usr/bin/
	cd binaries/; make install
	cp rvfs_submenu.py ~/.nautilus/python-extensions/

clean:
	cd src/; make clean
	cd ui/timeline/; make clean; rm Makefile timeline.pro timeline
	cd ui/piechart/; make clean; rm Makefile piechart.pro piechart
	cd binaries/; make clean

uninstall:
	cd src/; make uninstall
	rm /usr/bin/timeline
	rm /usr/bin/piechart
	cd binaries/; make uninstall
	rm ~/.nautilus/python-extensions/rvfs_submenu.py
