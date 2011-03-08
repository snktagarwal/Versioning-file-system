#ifndef GRAPH_WINDOW_H
#define GRAPH_WINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QPair>

#include "line.h"
#include "point.h"
//#include "label.h"

class GraphWindow : public QMainWindow {
	Q_OBJECT
	
	public:
		GraphWindow();
		
		Point *getRoot() const;
		void setRoot(Point *p);
	
	protected:
		void mousePressEvent(QMouseEvent *event);
	
	private:
		typedef QPair<Point *, Point *> PointPair;
		
		QGraphicsScene *scene;
		QGraphicsView *view;
		
		QList<Point *> *points;
		Point *root;
		//Label *label;
		
		void setupModel();
		void readFromFile(const QString &path);
};

#endif
