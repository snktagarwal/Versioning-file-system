#include <QtGui>
#include <iostream>

//#include "qxtspanslider.h"
#include "window.h"
#include "params.h"

GraphWindow::GraphWindow() {
	scene = new QGraphicsScene(0, 0, TIMELINE_DEFAULT_WIDTH, TIMELINE_DEFAULT_HEIGHT);
	
	setupModel();
	setupViews();
	
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

void GraphWindow::mousePressEvent(QMouseEvent *event) {
	
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
		else {
			editor->setText("");
		}
	}
}

void GraphWindow::readFromFile(const QString &path) {
	points = new QList<Point *>;
	
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
					QString tooltipText = pieces.value(2);
					QString parentString = pieces.value(3);
					int parentIndex = parentString.toInt();
					QString filename = pieces.value(4);
					
					if(x > (scene->width()-2*MARGIN)) {
						scene->setSceneRect(0, 0, (x+2*MARGIN), scene->height());
					}
					
					if(parentString.compare("-1") == 0) {
						
						if(TREE_WIDTH_DIRECTION == TOP_TO_BOTTOM) {
							y = 3*MARGIN + radius;
							if(y > (scene->height()-2*MARGIN)) {
								scene->setSceneRect(0, 0, scene->width(), (y+MARGIN));
							}
						}
						else if(TREE_WIDTH_DIRECTION == BOTTOM_TO_TOP) {
							y = scene->height() - (MARGIN + radius);
							if(y > (scene->height()-2*MARGIN)) {
								scene->setSceneRect(0, 0, scene->width(), (y+MARGIN));
							}
						}
						
						Point *p = new Point( MARGIN+x, y, radius, tooltipText );
						points->append(p);
						setRoot(p);
						
						p->setData(0, filename);
						
						if(counter == current) {
							p->setCurrent(true);
						}
					}
					else {
						Point *parent = points->at(parentIndex);
						Point *child = new Point( x, parent->getY(), radius, tooltipText, parent );
						parent->addChild(child);
						
						if(TREE_WIDTH_DIRECTION == TOP_TO_BOTTOM) {
							y = parent->getY() + BRANCH_SEPARATION * (parent->childCount()-1);
							if(y > (scene->height()-2*MARGIN)) {
								scene->setSceneRect(0, 0, scene->width(), (y+MARGIN));
							}
						}
						else if(TREE_WIDTH_DIRECTION == BOTTOM_TO_TOP) {
							y = parent->getY() - BRANCH_SEPARATION * (parent->childCount()-1);
						}
						
						child->setY(y);
						
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
