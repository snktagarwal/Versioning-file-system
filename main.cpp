#include <QApplication>

#include "graph_window.h"

int main(int argc, char *argv[]) {
	QApplication app(argc, argv);
	
	GraphWindow *window = new GraphWindow;
	window->show();
	
	return app.exec();
}
