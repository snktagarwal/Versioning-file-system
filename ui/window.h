#ifndef GRAPH_WINDOW_H
#define GRAPH_WINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QPair>
class QSplitter;
class QTextEdit;
class QLabel;

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
	
	protected:
		void mousePressEvent(QMouseEvent *event);
	
	private slots:
		void showDocument();
	
	private:
		typedef QPair<Point *, Point *> PointPair;
		
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
};

#endif
