#include <iostream>

#include "point.h"
#include "line.h"

Point::Point(QGraphicsScene *scene, qreal x, qreal y, qreal r, QString tagText, QString tooltipText, Point *parent) {	
	if(tagText.length() > TAG_MAX_LENGTH)
		tagText = tagText.left(TAG_MAX_LENGTH).append("...");
	tagText.prepend("[").append("]");
	
	this->x = x;
	this->y = y;
	this->r = r;
	this->tagText = tagText;
	this->tooltipText = tooltipText;
	this->parent = parent;
	this->ancestorCount = 0;
	this->current = false;
	
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
	
	setFlags(ItemIsSelectable);
	setCacheMode(QGraphicsItem::ItemCoordinateCache);
	setAcceptHoverEvents(true);
	
	timer = new QTimer(this);
	timer->setSingleShot(true);
	QObject::connect( timer, SIGNAL(timeout()), this, SLOT(startTimeLine()) );
}

Point::~Point() {
	foreach(Line *line, lines)
		delete line;
	foreach(Point *p, children)
		delete p;
}

qreal Point::getX() const {
	return x;
}
qreal Point::getY() const {
	return y;
}
qreal Point::getRadius() const {
	return r;
}
Point *Point::getParent() const {
	return parent;
}
QSet<Point *> Point::getChildren() const {
	return children;
}
int Point::childCount() const {
	return children.size();
}
int Point::getAncestorCount() const {
	return ancestorCount;
}
QGraphicsItemAnimation *Point::getAnimation() const {
	return animation;
}
QTimeLine *Point::getTimeLine() const {
	return timeline;
}
QColor Point::getBackgroundColor() const {
	return backgroundColor;
}
QGraphicsSimpleTextItem *Point::getTag() const {
	return tag;
}

void Point::setX(qreal x) {
	this->x = x;
}
void Point::setY(qreal y) {
	this->y = y;
}
void Point::setRadius(qreal r) {
	this->r = r; 
}
void Point::setCurrent(bool isCurrent) {
	this->current = isCurrent;
}
void Point::setAnimation(QGraphicsItemAnimation *animation) {
	this->animation = animation;
}
void Point::setTimeLine(QTimeLine *timeline) {
	this->timeline = timeline;
}
void Point::setBackgroundColor(const QColor &color) {
	backgroundColor = color;
}
void Point::setOutlineColor(const QColor &color) {
	outlineColor = color;
	update();
}
void Point::setOutlineWidth(qreal width) {
	outlineWidth = width;
	update();
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

void Point::addLine(Line *line) {
	lines.insert(line);
}
void Point::removeLine(Line *line) {
	lines.remove(line);
}

void Point::addChild(Point *child) {
	children.insert(child);
	//std::cout << "No. of children: " << children.size() << std::endl;
}

void Point::incrementAncestorCount() {
	ancestorCount++;
}

void Point::startTimer(int msec) {
	timer->start(msec);
}

void Point::startTimeLine() {
	timeline->start();
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
	//std::cout << "here: " << getX() << "," << getY() << std::endl;
  if (change == ItemPositionHasChanged) {
		foreach(Line *line, lines) {
			line->trackPoints();
			//std::cout << "here too" << std::endl;
		}
		//tag->update(tag->boundingRect());
		//std::cout << "here" << std::endl;
	}
	return QGraphicsItem::itemChange(change, value);
}

void Point::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
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
		//setBackgroundColor(POINT_CURRENT_BG_COLOR);
	}
	
	painter->setRenderHint(QPainter::Antialiasing,true);
	painter->setPen(pen);
	painter->setBrush(backgroundColor);
	
	painter->drawEllipse(boundingRect());
	
	//tag->update(tag->boundingRect());
	
	//std::cout << "(" << getX() << "," << getY() << ") isSelected(): " << isSelected() << std::endl;
	
	//if(tagText.compare("") != 0) {
		//qDebug() << "tagText: " << tagText << " | " << tagTextRect();
		//painter->setPen(TEXT_COLOR);
		//painter->drawText(tagTextRect(), Qt::AlignCenter, "tag");
	//}
	
	update();
}

// event handlers
void Point::mousePressEvent(QGraphicsSceneMouseEvent *event) {
	std::cout << "Point (" << getX() << "," << getY() << ") was clicked." << std::endl;
	event->accept();
}

/* void Point::hoverEnterEvent(QGraphicsSceneHoverEvent *event) {
	std::cout << "Point (" << getX() << "," << getY() << ") was hovered over (ENTER)." << std::endl;
	QStyleOptionGraphicsItem *option = new QStyleOptionGraphicsItem;
	option->state = QStyle::State_MouseOver;
	paint(new QPainter, option, NULL); */
	
	//QPointF scenePos = event->pos();
	//qreal distanceSquared = (scenePos.x()-getX())*(scenePos.x()-getX()) - (scenePos.y()-getY())*(scenePos.y()-getY());
	//std::cout << distanceSquared << std::endl;
	
	/* std::cout << "point: " << getX() << "," << getY() << std::endl;
	std::cout << "pos: " << (event->pos()).x() << "," << (event->pos()).y() << std::endl;
	std::cout << "scene: " << (event->scenePos()).x() << "," << (event->scenePos()).y() << std::endl;
	std::cout << "screen: " << (event->screenPos()).x() << "," << (event->screenPos()).y() << std::endl; */
	
	//if( distanceSquared < r*r ) {	
		//QGraphicsOpacityEffect *opacityEffect = new QGraphicsOpacityEffect;
		//this->setGraphicsEffect(opacityEffect);
	
		/*QPropertyAnimation animation(this, "backgroundColor");
		animation.setDuration(3000);
		QColor color = getBackgroundColor();
		color.setAlpha(1.0);
		animation.setStartValue(color);
		color.setAlpha(0.6);
		animation.setEndValue(color);

		//animation.setEasingCurve(QEasingCurve::OutBounce);

		animation.start();*/
	//}
/* }

void Point::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) {
	std::cout << "Point (" << getX() << "," << getY() << ") was hovered over (ENTER)." << std::endl;
	QStyleOptionGraphicsItem *option = new QStyleOptionGraphicsItem;
	option->state = QStyle::State_MouseOver;
	paint(new QPainter, option, NULL); */
	
	//QPointF scenePos = event->pos();
	//qreal distanceSquared = (scenePos.x()-getX())*(scenePos.x()-getX()) - (scenePos.y()-getY())*(scenePos.y()-getY());
	//std::cout << distanceSquared << std::endl;
	
	//if( distanceSquared > r*r ) {	
		//QGraphicsOpacityEffect *opacityEffect = new QGraphicsOpacityEffect;
		//opacityEffect->setOpacity(1.0);
		//this->setGraphicsEffect(opacityEffect);
	
		/*QPropertyAnimation animation(this, "backgroundColor");
		animation.setDuration(3000);
		QColor color = getBackgroundColor();
		color.setAlpha(1.0);
		animation.setStartValue(color);
		color.setAlpha(0.6);
		animation.setEndValue(color);

		//animation.setEasingCurve(QEasingCurve::OutBounce);

		animation.start();*/
	//}
//}
