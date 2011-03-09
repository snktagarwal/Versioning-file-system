#include <iostream>
#include <QVariant>

#include "point.h"
#include "line.h"
#include "params.h"

Point::Point(qreal x, qreal y, qreal r, QString text, Point *parent) {	
	this->x = x;
	this->y = y;
	this->r = r;
	this->setToolTip(text);
	this->parent = parent;
	this->current = false;
	
	textColor = TEXT_COLOR;
	backgroundColor = POINT_DEFAULT_BG_COLOR;
	outlineColor = POINT_DEFAULT_OUTLINE_COLOR;
	outlineWidth = POINT_DEFAULT_OUTLINE_WIDTH;
	
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

qreal Point::getX() const {
	return x;
}
qreal Point::getY() const {
	return y;
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

void Point::setY(qreal y) {
	this->y = y;
}
void Point::setRadius(qreal r) {
	this->r = r; 
}
void Point::setCurrent(bool isCurrent) {
	this->current = isCurrent;
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

void Point::addLine(Line *line) {
	lines.insert(line);
}
void Point::removeLine(Line *line) {
	lines.remove(line);
}

void Point::addChild(Point *child) {
	children.insert(child);
	std::cout << "No. of children: " << children.size() << std::endl;
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
    if (change == ItemPositionHasChanged) {
        foreach (Line *line, lines)
            line->trackPoints();
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
	
	painter->setPen(pen);
	painter->setBrush(backgroundColor);
	
	painter->drawEllipse(boundingRect());
	
	std::cout << "(" << getX() << "," << getY() << ") isSelected(): " << isSelected() << std::endl;
	
	//painter->setPen(textColor);
	//painter->drawText();
	update();
}

// event handlers
void Point::mousePressEvent(QGraphicsSceneMouseEvent *event) {
	std::cout << "Point (" << getX() << "," << getY() << ") was clicked." << std::endl;
}
