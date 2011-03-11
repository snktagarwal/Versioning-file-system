#include <QApplication>

#include "window.h"

int main(int argc, char *argv[]) {
	QApplication app(argc, argv);
	
	GraphWindow *window = new GraphWindow;
	window->show();
	
	return app.exec();
}
