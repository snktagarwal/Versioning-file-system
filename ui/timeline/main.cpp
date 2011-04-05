#include <QApplication>

#include "window.h"

int main(int argc, char *argv[]) {
	QApplication app(argc, argv);
	
	GraphWindow *window = new GraphWindow(argv[1]);
	
	return app.exec();
}
