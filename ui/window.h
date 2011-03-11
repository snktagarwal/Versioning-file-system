#ifndef GRAPH_WINDOW_H
#define GRAPH_WINDOW_H

#include <QtGui>

#include "line.h"
#include "point.h"
#include "axis.h"
//#include "label.h"

class GraphWindow : public QMainWindow {
	Q_OBJECT
	
	public:
		GraphWindow();
		
		Point *getRoot() const;
		void setRoot(Point *p);
	
	private slots:
		void showDocument();
		void startTimeLine(Point *p);
			
	private:
		QGraphicsScene *scene;
		QGraphicsView *view;
		QTextEdit *editor;
		QLabel *label;
		QSplitter *splitter;
		
		Axis *axis;
		QList<Point *> *points;
		Point *root;
		int current;
		
		void setupModel();
		void setupViews();
		void readFromFile(const QString &path);
		void animateTree();
		void setAncestorCount();
};

#endif
