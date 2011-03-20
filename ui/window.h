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
		QList<Point *> *getPoints() const;
		QGraphicsScene *getScene() const;
		QString getMountDirPath() const;
		
		void setRoot(Point *p);
		void setCurrent(Point *current);
	
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
		QScrollBar *doubleEditorVerticalScrollBar;
		//QSplitter *doubleEditorSplitter;
		
		QSplitter *splitter;
		//QWidget *mainWidget;
		
		Axis *axis;
		QList<Point *> *points;
		Point *root;
		qreal rootX;
		qreal maxX;
		Point *current;
		float scalingFactor;
		//int maxAncestorCount;
		
		QString filepath;
		QString filename;
		QString mountdir_path;
		
		void loadFileData(QString filepath);
		void setupModel();
		void setupViews();
		void readFromFile(const QString &path);
		void animateTree();
		void setAncestorCount();
		void highlightDifferences();
};

#endif
