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
		GraphWindow(char *filepath);
		
		Point *getRoot() const;
		Point *getCurrent() const;
		void setRoot(Point *p);
	
	private slots:
		void showDocument();
		void startTimeLine(Point *p);
		void updateRange(int min, int max);
		//void loadFile();
		//void setScale(int scale);
			
	private:
		//QLineEdit *fileLineEdit;
		//QPushButton *fileButton;
		//QSlider *zoomSlider;
		
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
		qreal maxX;
		int current;
		float scalingFactor;
		//int maxAncestorCount;
		
		QString filepath;
		QString filename;
		
		void loadFileData(QString filepath);
		void setupModel();
		void setupViews();
		void readFromFile(const QString &path);
		void animateTree();
		void setAncestorCount();
		void highlightDifferences(QFile *file1, QFile *file2);
};

#endif
