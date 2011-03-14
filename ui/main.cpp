#include <QApplication>

#include "window.h"

int main(int argc, char *argv[]) {
	QApplication app(argc, argv);
	
	GraphWindow *window = new GraphWindow(argv[1]);
	window->show();
	
	return app.exec();
}
