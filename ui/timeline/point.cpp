#include <iostream>

#include "point.h"
#include "line.h"
#include "window.h"

Point::Point(QString filepath, QGraphicsScene *scene, qreal x, qreal y, qreal r, QString tagText, QString tooltipText, Point *parent, GraphWindow *window) {
	
	if(tagText.length() > TAG_MAX_LENGTH)
		tagText = tagText.left(TAG_MAX_LENGTH).append("...");
	tagText.prepend("[").append("]");
	
	QStringList filePathPieces = filepath.split("/");
	for(int i=0; i<filePathPieces.size(); i++) {
		QString piece = filePathPieces.value(i);
		if(piece.compare("mountdir", Qt::CaseSensitive) == 0) {
			for(int j=i+1; j<filePathPieces.size(); j++) {
				QString usefulPiece = filePathPieces.value(j);
				relativeFilePath += "/"+usefulPiece;
			}
		}
	}
	relativeFilePath = relativeFilePath.remove(0,1);
	
	this->x = x;
	this->y = y;
	this->r = r;
	this->tagText = tagText;
	this->tooltipText = tooltipText;
	this->parent = parent;
	this->ancestorCount = 0;
	this->invalidCount = 0;
	this->current = false;
	this->window = window;
	
	textColor = TEXT_COLOR;
	backgroundColor = POINT_DEFAULT_BG_COLOR;
	outlineColor = POINT_DEFAULT_OUTLINE_COLOR;
	outlineWidth = POINT_DEFAULT_OUTLINE_WIDTH;
	
	setToolTip(tooltipText);
	
	if(tagText.compare("[]") != 0) {
		tag = new QGraphicsSimpleTextItem(tagText);
		updateTagPosition();
		scene->addItem(tag);
	}
	else {
		tag = new QGraphicsSimpleTextItem("");
		updateTagPosition();
		scene->addItem(tag);
	}
	
	setFlags(ItemIsSelectable);
	setCacheMode(QGraphicsItem::ItemCoordinateCache);
	setAcceptHoverEvents(true);
}

Point::~Point() {
	foreach(Line *line, lines)
		delete line;
	foreach(Point *p, children)
		delete p;
}

qreal Point::getX() const { return x; }
qreal Point::getY() const { return y; }
qreal Point::getXAtScale(float scale) const { return x*scale; }
qreal Point::getRadius() const { return r; }
Point *Point::getParent() const { return parent; }
QSet<Point *> Point::getChildren() const { return children; }
int Point::childCount() const { return children.size(); }
int Point::getAncestorCount() const { return ancestorCount; }
QColor Point::getBackgroundColor() const { return backgroundColor; }
qreal Point::getOutlineWidth() const { return outlineWidth; }
QGraphicsSimpleTextItem *Point::getTag() const { return tag; }
int Point::getInvalidCount() const { return invalidCount; }
bool Point::isValid() const { return valid; }
bool Point::isHead() const { if(this->children.size() == 0) { return true; } return false; }
QString Point::getRelativeFilePath() const { return relativeFilePath; }
QAction *Point::getCheckoutAction() const { return checkoutAction; }
QAction *Point::getRevertAction() const { return revertAction; }

void Point::setX(qreal x) { this->x = x; }
void Point::setY(qreal y) { this->y = y; }
void Point::setRadius(qreal r) { this->r = r; }
void Point::setCurrent(bool isCurrent) { this->current = isCurrent; }
void Point::setBackgroundColor(const QColor &color) { backgroundColor = color; }
void Point::setOutlineColor(const QColor &color) { outlineColor = color; }
void Point::setOutlineWidth(qreal width) { outlineWidth = width; }
void Point::setInvalidCount(int invalidCount) { this->invalidCount = invalidCount; }
void Point::setValidity(bool valid) { this->valid = valid; }
void Point::setCheckoutEnabled(bool enabled) { checkoutAction->setEnabled(enabled); }
void Point::setRevertEnabled(bool enabled) { revertAction->setEnabled(enabled); }
void Point::setTagText(QString tagText) {
	this->tagText = tagText;
	this->tag->setText(tagText);
	updateTagPosition();
}

void Point::updateTagPosition() {
	if(tagText.compare("[]") != 0) {
		QFontMetricsF metrics = qApp->font();
		QRectF rect = metrics.boundingRect(tagText);
		qreal tagTextX = x - (rect.width()/2);
		qreal tagTextY = y + r + (rect.height()/2) - 5;
		tag->setPos(tagTextX, tagTextY);
	}
}

void Point::checkoutVersion() {
	QString command = "cd "+window->getMountDirPath()+" && checkout "+relativeFilePath+" "+this->data(POINT_TIMESTAMP_INDEX).toString();
	int status = system(command.toLatin1().data());
	
	if(status == 0) {
		window->setCurrent(this);
		updateContextMenus();
	}
}
void Point::revertToVersion() {
	QString command = "cd "+window->getMountDirPath()+" && revert "+relativeFilePath+" "+this->data(POINT_TIMESTAMP_INDEX).toString();
	int status = system(command.toLatin1().data());
	
	if(status == 0) {
		Point *oldCurrent = window->getCurrent();
		window->setCurrent(this);
		// OPTIONAL TODO: roll in reverted points
		removeInvalidVersions(oldCurrent, this);
		updateContextMenus();
	}
}
void Point::tagVersion() {
	bool ok;
	QString tagTextCopy = tagText;
	QString text = QInputDialog::getText(window, "Edit Tag",
																				"Tag:", QLineEdit::Normal,
																				tagTextCopy.mid(1, tagTextCopy.length()-2), &ok);
	if (ok && !text.isEmpty()) {
		QString command = "cd "+window->getMountDirPath()+" && tag "+relativeFilePath+" "+this->data(POINT_TIMESTAMP_INDEX).toString()+" "+text;
		int status = system(command.toLatin1().data());
		if(status != -1) {
			text.prepend("[").append("]");
			setTagText(text);
		}
	}
}

void Point::removeInvalidVersions(Point *oldCurrent, Point *newCurrent) {
	Point *p = oldCurrent;
	Point *q;
	QList<Point *> *points = window->getPoints();
	QList<int> *markedPoints = new QList<int>;
	
	while(p->childCount() <=1 && newCurrent->isAncestorOf(p)) {
		q = p->getParent();
		
		for(int i=points->size()-1; i>=0; i--) {
			if(points->at(i)->data(POINT_OFFSET_INDEX) == p->data(POINT_OFFSET_INDEX)) {
				markedPoints->append(i);
				break;
			}
		}
		
		p = q;
	}
	
	for(int i=0; i<markedPoints->size(); i++) {
		Point *p = window->getPoints()->at(markedPoints->at(i));
		p->deleteAllLines();
		p->getParent()->removeChild(p);
		window->getPoints()->removeAt(markedPoints->at(i));
		p->deleteTag();
		window->getScene()->removeItem(p);
	}
	
}

void Point::addLine(Line *line) { lines.insert(line); }
void Point::removeLine(Line *line) { lines.remove(line); }
void Point::deleteAllLines() {
	foreach(Line *line, lines)
		delete line;
}
void Point::deleteTag() {
	delete tag;
}

void Point::addChild(Point *child) { children.insert(child); }
void Point::removeChild(Point *child) { children.remove(child); }
void Point::incrementAncestorCount() { ancestorCount++; }

void Point::updateContextMenu() {
}
void Point::updateContextMenus() {
	bool destinationIsAncestorOfCurrent = this->isAncestorOf(window->getCurrent());
	bool destinationIsHead = this->isHead();
	bool currentIsHead = window->getCurrent()->isHead();

	checkoutAction->setEnabled(destinationIsAncestorOfCurrent || destinationIsHead);
	revertAction->setEnabled(destinationIsAncestorOfCurrent && currentIsHead);
}

QRectF Point::boundingRect() const {
    QRectF rect(getX()-r, getY()-r, 2*r, 2*r);
    return rect;
}

QPainterPath Point::shape() const {
	QPainterPath path;
	path.addEllipse(boundingRect());
	return path;
}

QVariant Point::itemChange(GraphicsItemChange change, const QVariant &value)
{
  if (change == ItemPositionHasChanged)
		foreach(Line *line, lines)
			line->trackPoints();
	
	return QGraphicsItem::itemChange(change, value);
}

void Point::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget * /* widget */) {
	QPen pen(QBrush(outlineColor), outlineWidth);
	if(!current) {
		if((option->state & QStyle::State_Selected) && (option->state & QStyle::State_MouseOver)) {
			setBackgroundColor(POINT_HOVER_SELECTED_BG_COLOR);
			setCursor(Qt::PointingHandCursor);
		}
		else if(option->state & QStyle::State_MouseOver) {
			setBackgroundColor(POINT_HOVER_BG_COLOR);
			setCursor(Qt::PointingHandCursor);
		}
		else if(option->state & QStyle::State_Selected) {
			setBackgroundColor(POINT_SELECTED_BG_COLOR);
		}
		else {
			setBackgroundColor(POINT_DEFAULT_BG_COLOR);
		}
	}
	else {
		if((option->state & QStyle::State_Selected) && (option->state & QStyle::State_MouseOver)) {
			setBackgroundColor(POINT_CURRENT_HOVER_SELECTED_BG_COLOR);
			setCursor(Qt::PointingHandCursor);
		}
		else if(option->state & QStyle::State_MouseOver) {
			setBackgroundColor(POINT_CURRENT_HOVER_BG_COLOR);
			setCursor(Qt::PointingHandCursor);
		}
		else if(option->state & QStyle::State_Selected) {
			setBackgroundColor(POINT_CURRENT_SELECTED_BG_COLOR);			
		}
		else {
			setBackgroundColor(POINT_CURRENT_DEFAULT_BG_COLOR);
		}
	}
	
	painter->setRenderHint(QPainter::Antialiasing,true);
	painter->setPen(pen);
	painter->setBrush(backgroundColor);
	
	painter->drawEllipse(boundingRect());
	
	update();
}

// event handlers
void Point::mousePressEvent(QGraphicsSceneMouseEvent *event) {
	event->accept();
}

void Point::contextMenuEvent(QGraphicsSceneContextMenuEvent *event) {
	QMenu menu;
	
	checkoutAction = menu.addAction(QIcon::fromTheme("gtk-jump-to-ltr", QIcon()), "Checkout this version");
	revertAction = menu.addAction(QIcon::fromTheme("gtk-revert-to-saved-ltr", QIcon()), "Revert to this version");
	tagAction = menu.addAction("Tag this version");
	
	updateContextMenus();
	
	connect( checkoutAction , SIGNAL(triggered()) , this , SLOT(checkoutVersion()) );
	connect( revertAction , SIGNAL(triggered()) , this , SLOT(revertToVersion()) );
	connect( tagAction , SIGNAL(triggered()) , this , SLOT(tagVersion()) );
	
	event->accept();
}
