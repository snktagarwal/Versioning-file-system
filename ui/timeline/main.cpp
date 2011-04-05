#include <QApplication>
#include <iostream>

#include "window.h"

int main(int argc, char *argv[]) {
	if(argc != 2) {
		std::cerr << "Usage: timeline </path/to/text/file/in/mountdir>" << std::endl;
		return -1;
	}
	
	QApplication app(argc, argv);
	
	new GraphWindow(argv[1]);
	
	return app.exec();
}
