#include <QMainWindow>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <string>
#include <iostream>

#include "graph_window.h"
#include "timeline_params.h"

GraphWindow::GraphWindow() {
	scene = new QGraphicsScene(0, 0, DEFAULT_WIDTH, DEFAULT_HEIGHT);
	
	/*label = new Label("None");
	label->setPos(10,10);
	scene->addItem(label);*/
	
	setupModel();
	
	view = new QGraphicsView;
	view->setScene(scene);
	view->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
	view->setAlignment(Qt::AlignLeft | Qt::AlignTop);
	
//	setFixedSize(DEFAULT_WIDTH, DEFAULT_HEIGHT);
//	showFullScreen();
	setCentralWidget(view);
	
	setWindowTitle("Timeline");
	setAnimated(true);
}

Point *GraphWindow::getRoot() const {
	return root;
}

void GraphWindow::setRoot(Point *p) {
	root = p;
}

void GraphWindow::mousePressEvent(QMouseEvent *event) {
	
}

void GraphWindow::setupModel() {
	/*Point *p = new Point(50, 150, 5, "1");
	setRoot(p);
	
	Point *q = new Point(100, 150, 5, "2", root);
	Point *r = new Point(120, 75, 5, "3", root);
	
	QList<Point *> *points = new QList<Point *>;
	points->append(root);
	points->append(q);
	points->append(r);*/
	
	//label->installSceneEventFilter(root);
	
	//Line *pq = new Line(p, q, 5.0);
	//Line *pr = new Line(p, r, 5.0);
	
	readFromFile("data");
	
	for(int i=0; i<points->size(); i++) { 
		scene->addItem(points->at(i));
		
		if(points->at(i)->getParent() != 0) {
			Line *line = new Line(points->at(i)->getParent(), points->at(i), 5.0);
			scene->addItem(line);
			
			points->at(i)->setParentItem(points->at(i)->getParent());
		}
	}
	
	/*scene->addItem(p);
	scene->addItem(q);
	scene->addItem(r);
	scene->addItem(pq);
	scene->addItem(pr);
	
	q->setParentItem(root);
	r->setParentItem(root);*/
}

void GraphWindow::readFromFile(const QString &path) {
	points = new QList<Point *>;
	
	if(!path.isNull()) {
		QFile file(path);
		
		if(file.open(QFile::ReadOnly | QFile::Text)) {
			QTextStream stream(&file);
			QString line;
			
			do {
				line = stream.readLine();
				
				if(!line.isEmpty()) {
					QStringList pieces = line.split(",", QString::KeepEmptyParts);
					
					qreal x = pieces.value(0).toDouble();
					qreal y;
					qreal radius = pieces.value(1).toDouble();
					QString tooltipText = pieces.value(2);
					int parentIndex = pieces.value(3).toInt();
					
					if(x > (scene->width()-2*MARGIN)) {
						scene->setSceneRect(0, 0, (x+2*MARGIN), DEFAULT_HEIGHT);
					}
					
					if(pieces.value(3).compare("") == 0) {
						if(TREE_WIDTH_DIRECTION == TOP_TO_BOTTOM)
							y = MARGIN + radius;
						else if(TREE_WIDTH_DIRECTION == BOTTOM_TO_TOP)
							y = scene->height() - (MARGIN + radius);
						
						Point *p = new Point( MARGIN+radius+x, y, radius, tooltipText );
						points->append(p);
						setRoot(p);
					}
					else {
						Point *parent = points->at(parentIndex);
						Point *child = new Point( x, parent->getY(), radius, tooltipText, parent );
						parent->addChild(child);
						
						if(TREE_WIDTH_DIRECTION == TOP_TO_BOTTOM) 
							y = parent->getY() + BRANCH_SEPARATION * (parent->childCount()-1);
						else if(TREE_WIDTH_DIRECTION == BOTTOM_TO_TOP) 
							y = parent->getY() - BRANCH_SEPARATION * (parent->childCount()-1);
						
						child->setY(y);
						
						points->append(child);
					}
				}
			} while(!line.isEmpty());
			
			file.close();
		}
	}
}
