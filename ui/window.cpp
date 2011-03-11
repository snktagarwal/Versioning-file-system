#include <iostream>

//#include "qxtspanslider.h"
#include "window.h"
#include "params.h"

GraphWindow::GraphWindow() {
	scene = new QGraphicsScene(0, 0, TIMELINE_DEFAULT_WIDTH, TIMELINE_DEFAULT_HEIGHT);
	
	setupModel();
	setupViews();
	animateTree();
	
	QPointF *p1 = new QPointF(MARGIN, 1.5*MARGIN);
	QPointF *p2 = new QPointF((view->width())-MARGIN, 1.5*MARGIN);
	axis = new Axis(p1, p2);
	axis->drawTicks(scene);
	scene->addItem(axis);
	
	QList<int> sizes;
	sizes.append(scene->height());
	sizes.append(20);
	sizes.append(EDITOR_DEFAULT_HEIGHT);
	
	splitter->setSizes(sizes);
	
	connect(scene, SIGNAL(selectionChanged()), this, SLOT(showDocument()));
	
	if(MAXIMIZED)
		showMaximized();
	setWindowTitle("Timeline");
	setAnimated(true);
}

Point *GraphWindow::getRoot() const {
	return root;
}

void GraphWindow::setRoot(Point *p) {
	root = p;
}

void GraphWindow::animateTree() {
	QList<Point *> *pointsCopy = new QList<Point *>;
	
	Point *root = points->at(0);
	
	/*for(int i=0; i<points->size(); i++) {
		Point *p = points->at(i);
		pointsCopy->insert(i, new Point(p->getX(), p->getY()));
	}*/
	
	for(int i=0; i<points->size(); i++) {
		Point *p = points->at(i);
		if(p->getParent() == 0) {
			pointsCopy->insert(i, new Point(scene, p->getX()-root->getX(), p->getY()-root->getY()));
		}
		else {
			Point *parent;
			parent = p->getParent();
			pointsCopy->insert(i, new Point(scene, p->getX()-parent->getX(), p->getY()-parent->getY()));
		}
	}
	
	for(int i=0; i<points->size(); i++) {
		Point *p = points->at(i);
		Point *q = pointsCopy->at(i);
		
		p->setX(root->getX());
		p->setY(root->getY());
		
		QTimeLine *timeline = new QTimeLine(ANIMATION_TIME);
		p->setTimeLine( timeline );
		timeline->setFrameRange(0, 100);
		timeline->setCurveShape(ANIMATION_EASING_CURVE);
		
		QGraphicsItemAnimation *animation = new QGraphicsItemAnimation;
		p->setAnimation(animation);
		animation->setItem(p);
		animation->setTimeLine(timeline);
		
		for(int j=0; j<ANIMATION_TIME; j++) {
			qreal ratio = ((float)j)/ANIMATION_TIME;
			animation->setPosAt( ratio, QPointF( ratio*(q->getX()), ratio*(q->getY()) ) );
		}
		
		p->startTimer((p->getAncestorCount()+1)*ANIMATION_TIME);
	}
}

void GraphWindow::startTimeLine(Point *p) {
	p->startTimeLine();
}

void GraphWindow::setAncestorCount() {
	Point *p, *p1, *p2;
	for(int i=0; i<points->size(); i++) {
		p = points->at(i);
		p1 = p;
		p2 = p->getParent();
		while(p2 != 0) {
			p1 = p2;
			p2 = p1->getParent();
			p->incrementAncestorCount();
		}
		//std::cout << "Ancestors: " << p->getAncestorCount() << std::endl;
	}
}

void GraphWindow::setupViews() {
	view = new QGraphicsView;
	view->setScene(scene);
	view->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
	view->setAlignment(Qt::AlignLeft | Qt::AlignTop);
	view->setMinimumSize(scene->width(), scene->height()+2);
	
	editor = new QTextEdit();
	editor->setReadOnly(true);
	editor->setBaseSize(EDITOR_DEFAULT_WIDTH, EDITOR_DEFAULT_HEIGHT);
	
	label = new QLabel("Document");
	label->setMinimumSize(20, 20);
	label->setMaximumSize(2000, 20);
	
	splitter = new QSplitter;
	splitter->addWidget(view);
	splitter->addWidget(label);
	splitter->addWidget(editor);
	splitter->setOrientation(Qt::Vertical);
	
	setCentralWidget(splitter);
}

void GraphWindow::setupModel() {
	readFromFile("data");
	setAncestorCount();
	
	for(int i=0; i<points->size(); i++) {
		Point *p = points->at(i);
		
		scene->addItem(p);
		
		if(p->childCount() == 0) {
			p->setOutlineColor(POINT_HEAD_OUTLINE_COLOR);
			p->setOutlineWidth(POINT_HEAD_OUTLINE_WIDTH);
		}
		
		if(p->getParent() != 0) {
			Line *line = new Line(p->getParent(), p, 5.0);
			scene->addItem(line);
			
			p->setParentItem(p->getParent());
		}
	}
}

void GraphWindow::showDocument() {
	QList<QGraphicsItem *> selectedPoints = scene->selectedItems();
	std::cout << "No. of selections: " << selectedPoints.size() << std::endl;
	
	QList<QTextEdit::ExtraSelection> extras;
	editor->setExtraSelections(extras);
	
	if(selectedPoints.size() == 1) {
		Point *p = dynamic_cast<Point *>(selectedPoints.at(0));
		QString filename = p->data(0).toString();
		QString path = OBJECTS_DIR+filename;
		
		if(path.compare(OBJECTS_DIR) != 0) {
			QFile file(path);
			
			if(file.open(QFile::ReadOnly | QFile::Text)) {
				QTextStream stream(&file);
				editor->setText(stream.readAll());
				
				QTextCursor cursor = editor->textCursor();
				cursor.movePosition(QTextCursor::Down);
				cursor.movePosition(QTextCursor::Down);
				
				QTextEdit::ExtraSelection highlight, highlight1;
				highlight.cursor = editor->textCursor();
				highlight.cursor.movePosition(QTextCursor::Down);
				highlight.cursor.movePosition(QTextCursor::Down);
				highlight.format.setBackground(QColor(0, 255, 0, 100));
				highlight.format.setProperty(QTextFormat::FullWidthSelection, true);
				
				highlight1.cursor = editor->textCursor();
				highlight1.cursor.movePosition(QTextCursor::Down);
				highlight1.cursor.movePosition(QTextCursor::Down);
				highlight1.cursor.movePosition(QTextCursor::Down);
				highlight1.format.setBackground(QColor(255, 0, 0, 100));
				highlight1.format.setProperty(QTextFormat::FullWidthSelection, true);
				
				QList<QTextEdit::ExtraSelection> extras;
				extras << highlight;
				extras << highlight1;
				//editor->setExtraSelections(extras);
				
				/*QPalette palette;
				palette.setColor(QPalette::Highlight, QColor(255, 0, 0, 100));
				editor->setPalette(palette);
				
				QTextCursor cursor = editor->textCursor();
				cursor.movePosition(QTextCursor::Down);
				cursor.movePosition(QTextCursor::Down);
				cursor.select(QTextCursor::LineUnderCursor);
				editor->setTextCursor(cursor);
				
				palette.setColor(QPalette::Highlight, QColor(0, 255, 0, 100));
				editor->setPalette(palette);
				
				cursor = editor->textCursor();
				cursor.movePosition(QTextCursor::Down);
				cursor.movePosition(QTextCursor::Down);
				cursor.select(QTextCursor::LineUnderCursor);
				editor->setTextCursor(cursor);*/
			}
		}
	}
	else if(selectedPoints.size() == 2) {
	}
	else {
			scene->clearSelection();
			editor->setText("");
	}
}

void GraphWindow::readFromFile(const QString &path) {
	points = new QList<Point *>;
	qreal rootX;
	
	if(!path.isNull()) {
		QFile file(path);
		
		if(file.open(QFile::ReadOnly | QFile::Text)) {
			QTextStream stream(&file);
			QString line;
			
			current = stream.readLine().toInt();
			int counter = 0;
			
			do {
				line = stream.readLine();
				
				if(!line.isEmpty()) {
					QStringList pieces = line.split(",", QString::KeepEmptyParts);
					
					qreal x = pieces.value(0).toDouble();
					qreal y;
					qreal radius = pieces.value(1).toDouble();
					//QString tooltipText = pieces.value(2);
					QString parentString = pieces.value(2);
					int parentIndex = parentString.toInt();
					QString filename = pieces.value(3);
					QString tagText = pieces.value(4);
					QString tooltipText = tagText;
					
					if(x > (scene->width()-2*MARGIN)) {
						scene->setSceneRect(0, 0, (x+2*MARGIN), scene->height());
					}
					
					if(parentString.compare("-1") == 0) {
						rootX = x;
						x = 0;
						y = 3*MARGIN + radius;
						if(y > (scene->height()-2*MARGIN)) {
							scene->setSceneRect(0, 0, scene->width(), (y+MARGIN));
						}
						
						Point *p = new Point( scene, MARGIN+x, y, radius, tagText, tooltipText );
						points->append(p);
						setRoot(p);
						
						p->setData(0, filename);
						
						if(counter == current) {
							p->setCurrent(true);
						}
					}
					else {
						x -= rootX;
						
						Point *parent = points->at(parentIndex);
						Point *child = new Point( scene, MARGIN+x, parent->getY(), radius, tagText, tooltipText, parent );
						parent->addChild(child);
						
						y = parent->getY() + BRANCH_SEPARATION * (parent->childCount()-1);
						if(y > (scene->height()-2*MARGIN)) {
							scene->setSceneRect(0, 0, scene->width(), (y+MARGIN));
						}
						
						child->setY(y);
						child->updateTagPosition();
						
						points->append(child);
						
						child->setData(0, filename);
						
						if(counter == current) {
							child->setCurrent(true);
						}
					}
				}
				
				counter++;
			} while(!line.isEmpty());
			
			file.close();
		}
	}
}
