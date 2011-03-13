#include <iostream>
#include <cstdlib>

//#include "qxtspanslider.h"
#include "window.h"
#include "params.h"

GraphWindow::GraphWindow() {
	scene = new QGraphicsScene(0, 0, TIMELINE_DEFAULT_WIDTH, TIMELINE_DEFAULT_HEIGHT);
	
	setupModel();
	setupViews();
	animateTree();
	
	QPointF *p1 = new QPointF(LEFT_MARGIN, TOP_MARGIN);
	QPointF *p2 = new QPointF(maxX, TOP_MARGIN);
	axis = new Axis(p1, p2);
	axis->drawTicks(scene, rootX);
	scene->addItem(axis);
	
	QScrollBar *horizontalScrollBar = view->horizontalScrollBar();
	horizontalScrollBar->setValue(horizontalScrollBar->maximum());
	horizontalScrollBar->setValue(horizontalScrollBar->minimum());
	
	QList<int> sizes;
	sizes.append(scene->height());
	sizes.append(EDITOR_DEFAULT_HEIGHT);
	sizes.append(EDITOR_DEFAULT_HEIGHT);
	
	splitter->setSizes(sizes);
	
	connect(scene, SIGNAL(selectionChanged()), this, SLOT(showDocument()));
	
	if(MAXIMIZED)
		showMaximized();
	setWindowTitle("Timeline");
	setAnimated(true);
	setMaximumSize(QApplication::desktop()->screenGeometry().width(), QApplication::desktop()->screenGeometry().height());
}

Point *GraphWindow::getRoot() const { return root; }
void GraphWindow::setRoot(Point *p) { root = p; }

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
	
	//maxAncestorCount = points->at(0)->getAncestorCount();
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
		
		/* if(p->getAncestorCount() > maxAncestorCount) {
			maxAncestorCount = p->getAncestorCount();
		} */
	}
	
	/* qDebug() << "maxAncestorCount: " << maxAncestorCount;
	for(int i=0; i<points->size(); i++) {
		Point *p = points->at(i);
		QTimer *animationEndedTimer = new QTimer;
		animationEndedTimer->setSingleShot(true);
		connect( animationEndedTimer , SIGNAL(timeout()) , p , SLOT(animationEnded()) );
		animationEndedTimer->start((maxAncestorCount+10)*ANIMATION_TIME);
	} */
}

void GraphWindow::startTimeLine(Point *p) { p->startTimeLine(); }

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
	view->setMinimumSize(QApplication::desktop()->screenGeometry().width(), scene->height()+20);
	view->setBaseSize(scene->width(), scene->height()+20);
	
	// single editor for when a single point is clicked
	singleEditorLabel = new QLabel("Document");
	
	singleEditor = new QTextEdit(this);
	singleEditor->setReadOnly(true);
	singleEditor->setBaseSize(EDITOR_DEFAULT_WIDTH, EDITOR_DEFAULT_HEIGHT);
	
	QVBoxLayout *singleEditorLayout = new QVBoxLayout;
	singleEditorLayout->addWidget(singleEditorLabel);
	singleEditorLayout->addWidget(singleEditor);
	
	singleEditorWidget = new QWidget(this);
	singleEditorWidget->setLayout(singleEditorLayout);
	singleEditorWidget->hide();
	
	// double editor for when 2 points are clicked
	// Editor #1
	doubleEditorLeftLabel = new QLabel("Document");
	
	doubleEditorLeft = new QTextEdit(this);
	doubleEditorLeft->setReadOnly(true);
	doubleEditorLeft->setBaseSize(EDITOR_DEFAULT_WIDTH, EDITOR_DEFAULT_HEIGHT);
	doubleEditorLeft->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	doubleEditorLeft->setContentsMargins(0,0,0,0);
	
	QHBoxLayout *layout = new QHBoxLayout;
	layout->addWidget(doubleEditorLeft);
	
	QVBoxLayout *doubleEditorLeftLayout = new QVBoxLayout;
	doubleEditorLeftLayout->addWidget(doubleEditorLeftLabel);
	doubleEditorLeftLayout->addLayout(layout);
	
	// Editor #2
	doubleEditorRightLabel = new QLabel("Document");
	
	doubleEditorRight = new QTextEdit(this);
	doubleEditorRight->setReadOnly(true);
	doubleEditorRight->setBaseSize(EDITOR_DEFAULT_WIDTH, EDITOR_DEFAULT_HEIGHT);
	doubleEditorRight->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	doubleEditorRight->setContentsMargins(0,0,0,0);
	
	QVBoxLayout *doubleEditorRightLayout = new QVBoxLayout;
	doubleEditorRightLayout->addWidget(doubleEditorRightLabel);
	doubleEditorRightLayout->addWidget(doubleEditorRight);
	
	// Create and connect common scroll bar to editors
	QScrollBar *leftScrollBar = doubleEditorLeft->verticalScrollBar();
	QScrollBar *rightScrollBar = doubleEditorRight->verticalScrollBar();
	doubleEditorScrollBar = new QScrollBar(Qt::Vertical, this);
	connect( leftScrollBar , SIGNAL(valueChanged(int)) , rightScrollBar , SLOT(setValue(int)) );
	connect( rightScrollBar , SIGNAL(valueChanged(int)) , leftScrollBar , SLOT(setValue(int)) );
	connect( doubleEditorScrollBar , SIGNAL(valueChanged(int)) , leftScrollBar , SLOT(setValue(int)) );
	connect( doubleEditorScrollBar , SIGNAL(valueChanged(int)) , rightScrollBar , SLOT(setValue(int)) );
	connect( leftScrollBar , SIGNAL(valueChanged(int)) , doubleEditorScrollBar , SLOT(setValue(int)) );
	connect( rightScrollBar , SIGNAL(valueChanged(int)) , doubleEditorScrollBar , SLOT(setValue(int)) );
	
	connect( leftScrollBar , SIGNAL(rangeChanged(int,int)) , this , SLOT(updateRange(int,int)) ) ;
	connect( rightScrollBar , SIGNAL(rangeChanged(int,int)) , this , SLOT(updateRange(int,int)) ) ;
	
	layout->addWidget(doubleEditorScrollBar);
	
	// Double Editor Widget
	QHBoxLayout *doubleEditorLayout = new QHBoxLayout;
	doubleEditorLayout->addLayout(doubleEditorLeftLayout);
	//doubleEditorLayout->addLayout(doubleEditorScrollBarLayout);
	doubleEditorLayout->addLayout(doubleEditorRightLayout);
	
	doubleEditorWidget = new QWidget;
	doubleEditorWidget->setLayout(doubleEditorLayout);
	doubleEditorWidget->hide();
	
	splitter = new QSplitter(Qt::Vertical);
	splitter->addWidget(view);
	splitter->addWidget(singleEditorWidget);
	splitter->addWidget(doubleEditorWidget);
	
	setCentralWidget(splitter);
}

void GraphWindow::setupModel() {
	readFromFile(DATA_FILE);
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
			p->installSceneEventFilter(p->getParent());
		}
	}
}

void GraphWindow::showDocument() {
	QList<QGraphicsItem *> selectedPoints = scene->selectedItems();
	std::cout << "No. of selections: " << selectedPoints.size() << std::endl;
	
	QFile diff(DIFF_FILE);
	if(diff.exists())
		diff.remove();
	
	// 1 point selected
	if(selectedPoints.size() == 1) {
		singleEditor->setText("");
		doubleEditorLeft->setText("");
		doubleEditorRight->setText("");
		doubleEditorWidget->hide();
		singleEditorWidget->show();
		
		Point *p = dynamic_cast<Point *>(selectedPoints.at(0));
		QString filename = p->data(0).toString();
		QString path = OBJECTS_DIR+filename;
		
		if(path.compare(OBJECTS_DIR) != 0) {
			QFile file(path);
			
			if(file.open(QFile::ReadOnly | QFile::Text)) {
				QTextStream stream(&file);
				singleEditor->setText(stream.readAll());
			}
		}
	}
	// 2 points selected
	else if(selectedPoints.size() == 2) {
		singleEditor->setText("");
		doubleEditorLeft->setText("");
		doubleEditorRight->setText("");
		singleEditorWidget->hide();
		doubleEditorWidget->show();
		
		Point *p1 = dynamic_cast<Point *>(selectedPoints.at(0));
		QString filename1 = p1->data(0).toString();
		QString path1 = OBJECTS_DIR+filename1;
		
		QFile *file1, *file2;
		if(path1.compare(OBJECTS_DIR) != 0) {
			file1 = new QFile(path1);
			if(file1->open(QFile::ReadOnly | QFile::Text)) {
				QTextStream stream1(file1);
				doubleEditorLeft->setText(stream1.readAll());
			}
		}
		
		Point *p2 = dynamic_cast<Point *>(selectedPoints.at(1));
		QString filename2 = p2->data(0).toString();
		QString path2 = OBJECTS_DIR+filename2;
		
		if(path2.compare(OBJECTS_DIR) != 0) {
			file2 = new QFile(path2);
			if(file2->open(QFile::ReadOnly | QFile::Text)) {
				QTextStream stream2(file2);
				doubleEditorRight->setText(stream2.readAll());
			}
		}
		
		highlightDifferences(file1, file2);
		
		file1->close();
		file2->close();
	}
	// other selections
	else {
		scene->clearSelection();
		singleEditor->setText("");
		doubleEditorLeft->setText("");
		doubleEditorRight->setText("");
		singleEditorWidget->hide();
		doubleEditorWidget->hide();
	}
}

void GraphWindow::highlightDifferences(QFile *file1, QFile *file2) {
	// declare extra selections
	QList<QTextEdit::ExtraSelection> extraSelections1, extraSelections2;
	
	QString command = "diff "+file1->fileName()+" "+file2->fileName()+">"+DIFF_FILE;
	system(command.toLatin1().data());
	
	QFile *diff_file = new QFile(DIFF_FILE);
	QString thunkHead, line;
	if(diff_file->open(QFile::ReadOnly | QFile::Text)) {
		QTextStream stream(diff_file);
		
		thunkHead = stream.readLine();
		
		while(!thunkHead.isNull()) {
			qDebug() << "Thunk Head: " << thunkHead;
			if(thunkHead.contains('c')) {
				line = stream.readLine();
				
				QStringList changePieces = thunkHead.split("c", QString::SkipEmptyParts);
				QString file1Changes = changePieces.value(0);
				QString file2Changes = changePieces.value(1);
				qDebug() << file1Changes << "|" << file2Changes;
				
				if(!file1Changes.contains(",")) {
					QTextEdit::ExtraSelection selection1;
					selection1.cursor = doubleEditorLeft->textCursor();
					selection1.format.setBackground(EDITOR_COMMON_HIGHLIGHT_COLOR);
					
					int file1LineChanged = file1Changes.toInt();
					std::cout << "File 1 Changed: " << file1LineChanged << std::endl;
					
					selection1.cursor.movePosition(QTextCursor::Start);
					selection1.cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, file1LineChanged-1);
					selection1.format.setProperty(QTextFormat::FullWidthSelection, true);
					selection1.cursor.clearSelection();
					extraSelections1.append(selection1);
				}
				else {
					QStringList file1LinesChanged = file1Changes.split(",");
					int file1LinesChangedFrom = file1LinesChanged.value(0).toInt();
					int file1LinesChangedTo = file1LinesChanged.value(1).toInt();
					std::cout << "File 1 Changed [R]: From Line #" << file1LinesChangedFrom << " to Line #" << file1LinesChangedTo << std::endl; 
					
					for(int i=(file1LinesChangedFrom-1) ; i<=(file1LinesChangedTo-1) ; i++) {
						QTextEdit::ExtraSelection selection1;
						selection1.cursor = doubleEditorLeft->textCursor();
						selection1.format.setBackground(EDITOR_COMMON_HIGHLIGHT_COLOR);
						
						selection1.cursor.movePosition(QTextCursor::Start);
						selection1.cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, i);
						selection1.format.setProperty(QTextFormat::FullWidthSelection, true);
						selection1.cursor.clearSelection();
						extraSelections1.append(selection1);
					}
				}
				
				if(!file2Changes.contains(",")) {
					QTextEdit::ExtraSelection selection2;
					selection2.cursor = doubleEditorRight->textCursor();
					selection2.format.setBackground(EDITOR_COMMON_HIGHLIGHT_COLOR);
					
					int file2LineChanged = file2Changes.toInt();
					std::cout << "File 2 Changed: " << file2LineChanged << std::endl;
					
					selection2.cursor.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor);
					selection2.cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, file2LineChanged-1);
					selection2.format.setProperty(QTextFormat::FullWidthSelection, true);
					selection2.cursor.clearSelection();
					extraSelections2.append(selection2);
				}
				else {
					QStringList file2LinesChanged = file2Changes.split(",");
					int file2LinesChangedFrom = file2LinesChanged.value(0).toInt();
					int file2LinesChangedTo = file2LinesChanged.value(1).toInt();
					std::cout << "File 2 Changed [R]: From Line #" << file2LinesChangedFrom << " to Line #" << file2LinesChangedTo << std::endl; 
					
					for(int i=(file2LinesChangedFrom-1) ; i<=(file2LinesChangedTo-1) ; i++) {
						QTextEdit::ExtraSelection selection2;
						selection2.cursor = doubleEditorRight->textCursor();
						selection2.format.setBackground(EDITOR_COMMON_HIGHLIGHT_COLOR);
						
						selection2.cursor.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor);
						selection2.cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, i);
						selection2.format.setProperty(QTextFormat::FullWidthSelection, true);
						selection2.cursor.clearSelection();
						extraSelections2.append(selection2);
					}
				}
				
				while(line.startsWith("<") || line.startsWith(">") || line.startsWith("-")) {
					line = stream.readLine();
				}
			}
			else if(thunkHead.contains('a')) {
				line = stream.readLine();
				
				QStringList changePieces = thunkHead.split("a", QString::SkipEmptyParts);
				
				QString file1Changes = changePieces.value(0);
				QString file2Changes = changePieces.value(1);
				
				qDebug() << file1Changes << "|" << file2Changes;
				
				if(!file2Changes.contains(",")) {
					QTextEdit::ExtraSelection selection2;
					selection2.cursor = doubleEditorRight->textCursor();
					selection2.format.setBackground(EDITOR_EXCLUSIVE_HIGHLIGHT_COLOR);
					
					int file2LineChanged = file2Changes.toInt();
					std::cout << "File 2 Exclusive: " << file2LineChanged << std::endl;
					
					selection2.cursor.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor);
					selection2.cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, file2LineChanged-1);
					selection2.format.setProperty(QTextFormat::FullWidthSelection, true);
					selection2.cursor.clearSelection();
					extraSelections2.append(selection2);
				}
				else {
					QStringList file2LinesChanged = file2Changes.split(",");
					int file2LinesChangedFrom = file2LinesChanged.value(0).toInt();
					int file2LinesChangedTo = file2LinesChanged.value(1).toInt();
					std::cout << "File 2 Exclusive [R]: From Line #" << file2LinesChangedFrom << " to Line #" << file2LinesChangedTo << std::endl; 
					
					for(int i=(file2LinesChangedFrom-1) ; i<=(file2LinesChangedTo-1) ; i++) {
						QTextEdit::ExtraSelection *selection2 = new QTextEdit::ExtraSelection;
						selection2->cursor = doubleEditorRight->textCursor();
						selection2->format.setBackground(EDITOR_EXCLUSIVE_HIGHLIGHT_COLOR);
						
						selection2->cursor.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor);
						selection2->cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, i);
						selection2->format.setProperty(QTextFormat::FullWidthSelection, true);
						selection2->cursor.clearSelection();
						extraSelections2.append(*selection2);
					}
				}
				
				while(line.startsWith("<") || line.startsWith(">") || line.startsWith("-")) {
					line = stream.readLine();
				}
			}
			else if(thunkHead.contains('d')) {
				line = stream.readLine();
				
				QStringList changePieces = thunkHead.split("d", QString::SkipEmptyParts);
				
				QString file1Changes = changePieces.value(0);
				QString file2Changes = changePieces.value(1);
				
				qDebug() << file1Changes << "|" << file2Changes;
				
				if(!file1Changes.contains(",")) {
					QTextEdit::ExtraSelection selection1;
					selection1.cursor = doubleEditorLeft->textCursor();
					selection1.format.setBackground(EDITOR_EXCLUSIVE_HIGHLIGHT_COLOR);
					
					int file1LineChanged = file1Changes.toInt();
					std::cout << "File 1 Exclusive: " << file1LineChanged << std::endl;
					
					selection1.cursor.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor);
					selection1.cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, file1LineChanged-1);
					selection1.format.setProperty(QTextFormat::FullWidthSelection, true);
					selection1.cursor.clearSelection();
					extraSelections1.append(selection1);
				}
				else {
					QStringList file1LinesChanged = file1Changes.split(",");
					int file1LinesChangedFrom = file1LinesChanged.value(0).toInt();
					int file1LinesChangedTo = file1LinesChanged.value(1).toInt();
					std::cout << "File 1 Exclusive [R]: From Line #" << file1LinesChangedFrom << " to Line #" << file1LinesChangedTo << std::endl; 
					
					for(int i=(file1LinesChangedFrom-1) ; i<=(file1LinesChangedTo-1) ; i++) {
						QTextEdit::ExtraSelection *selection1 = new QTextEdit::ExtraSelection;
						selection1->cursor = doubleEditorLeft->textCursor();
						selection1->format.setBackground(EDITOR_EXCLUSIVE_HIGHLIGHT_COLOR);
						
						selection1->cursor.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor);
						selection1->cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, i);
						selection1->format.setProperty(QTextFormat::FullWidthSelection, true);
						selection1->cursor.clearSelection();
						extraSelections1.append(*selection1);
					}
				}
				
				while(line.startsWith("<") || line.startsWith(">") || line.startsWith("-")) {
					line = stream.readLine();
				}
			}
			thunkHead = line;
		}
	}
	diff_file->close();
	
	doubleEditorLeft->setExtraSelections(extraSelections1);
	doubleEditorRight->setExtraSelections(extraSelections2);
}

void GraphWindow::updateRange(int min, int max) {
	QScrollBar *leftScrollBar = doubleEditorLeft->verticalScrollBar();
	QScrollBar *rightScrollBar = doubleEditorRight->verticalScrollBar();
	int leftScrollBarRange = leftScrollBar->maximum() - leftScrollBar->minimum();
	int rightScrollBarRange = rightScrollBar->maximum() - rightScrollBar->minimum();
	/* std::cout << "Left: " << leftScrollBar->minimum() << "," << leftScrollBar->maximum() << std::endl;
	std::cout << "Right: " << rightScrollBar->minimum() << "," << rightScrollBar->maximum() << std::endl;
	std::cout << "Received: " << min << "," << max << std::endl; */
	int scrollBarRangeMin, scrollBarRangeMax; //singleStep;
	if(leftScrollBarRange > rightScrollBarRange) {
		scrollBarRangeMax = leftScrollBar->maximum();
		scrollBarRangeMin = leftScrollBar->minimum();
		//singleStep = leftScrollBar->singleStep();
	}
	else {
		scrollBarRangeMax = rightScrollBar->maximum();
		scrollBarRangeMin = rightScrollBar->minimum();
		//singleStep = rightScrollBar->singleStep();
	}
	doubleEditorScrollBar->setRange( scrollBarRangeMin , scrollBarRangeMax );
	//doubleEditorScrollBar->setSingleStep(singleStep);
}

void GraphWindow::readFromFile(const QString &path) {
	points = new QList<Point *>;
	maxX = 0;
	int branchCount = 1;
	
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
					
					qreal x = pieces.value(0).toDouble() * SCALING_FACTOR;
					qreal y;
					qreal radius = pieces.value(1).toDouble();
					//QString tooltipText = pieces.value(2);
					QString parentString = pieces.value(2);
					int parentIndex = parentString.toInt();
					QString filename = pieces.value(3);
					QString tagText = pieces.value(4);
					QString tooltipText = tagText;
					
					if(parentString.compare("-1") == 0) {
						rootX = x;
						x = 0;
						y = TOP_MARGIN + BOTTOM_MARGIN + AXIS_BOTTOM_MARGIN + radius;
						
						// resize the scene as required
						if(x > (scene->width() - rootX - RIGHT_MARGIN))
							scene->setSceneRect(0, 0, x+RIGHT_MARGIN, scene->height());
						if(y > (scene->height() - BOTTOM_MARGIN))
							scene->setSceneRect(0, 0, scene->width(), y+BOTTOM_MARGIN);
						
						Point *p = new Point( scene, LEFT_MARGIN+x, y, radius, tagText, tooltipText );
						points->append(p);
						setRoot(p);
						
						if(LEFT_MARGIN+x > maxX)
							maxX = LEFT_MARGIN+x;
						
						p->setData(0, filename);
						
						if(counter == current) {
							p->setCurrent(true);
						}
					}
					else {
						x -= rootX;
						
						Point *parent = points->at(parentIndex);
						Point *child = new Point( scene, LEFT_MARGIN+x, parent->getY(), radius, tagText, tooltipText, parent );
						parent->addChild(child);
						
						if(LEFT_MARGIN+x > maxX) {
							maxX = LEFT_MARGIN+x;
						}
						
						if(parent->childCount() > 1) {
							branchCount++;
							y = root->getY() + BRANCH_SEPARATION * (branchCount-1);
						}
						else {
							y = parent->getY();
						}
						
						// resize the scene as required
						if(x > (scene->width() - rootX - RIGHT_MARGIN))
							scene->setSceneRect(0, 0, x+RIGHT_MARGIN, scene->height());
						if(y > (scene->height() - BOTTOM_MARGIN))
							scene->setSceneRect(0, 0, scene->width(), y+BOTTOM_MARGIN);
						
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
			
			qreal length = maxX - LEFT_MARGIN;
			int segmentCount = ((int)(length/AXIS_DEFAULT_TICK_SEPARATION)) + 1;
			qreal axisMaxX = LEFT_MARGIN + segmentCount * AXIS_DEFAULT_TICK_SEPARATION;
			scene->setSceneRect( 0, 0, axisMaxX + RIGHT_MARGIN, scene->height() );
			
			file.close();
		}
	}
}
