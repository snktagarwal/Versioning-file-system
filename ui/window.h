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
		void updateRange(int min, int max);
			
	private:
		QGraphicsScene *scene;
		QGraphicsView *view;
		
		QWidget *singleEditorWidget;
		QLabel *singleEditorLabel;
		QTextEdit *singleEditor;
		
		QWidget *doubleEditorWidget;
		QLabel *doubleEditorLeftLabel;
		QLabel *doubleEditorRightLabel;
		QTextEdit *doubleEditorLeft;
		QTextEdit *doubleEditorRight;
		QScrollBar *doubleEditorScrollBar;
		//QSplitter *doubleEditorSplitter;
		
		QSplitter *splitter;
		//QWidget *mainWidget;
		
		Axis *axis;
		QList<Point *> *points;
		Point *root;
		qreal rootX;
		int current;
		
		void setupModel();
		void setupViews();
		void readFromFile(const QString &path);
		void animateTree();
		void setAncestorCount();
		void highlightDifferences(QFile *file1, QFile *file2);
};

#endif
