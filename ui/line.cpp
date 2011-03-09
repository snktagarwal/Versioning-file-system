#include <QPen>

#include "line.h"
#include "point.h"
#include "params.h"

Line::Line(Point *parent, Point *child, qreal width = LINE_DEFAULT_WIDTH) {
	this->parent = parent;
	this->child = child;
	this->width = width;
	
	parent->addLine(this);
	child->addLine(this);
	
	setZValue(LINE_DEFAULT_Z_VALUE);
	setColor(LINE_DEFAULT_COLOR);
	setCacheMode(QGraphicsItem::ItemCoordinateCache);
	setLine(QLineF(parent->getX(), parent->getY(), child->getX(), child->getY()));
}

Line::~Line() {
	parent->removeLine(this);
	child->removeLine(this);
}

Point *Line::getParent() const {
	return parent;
}

Point *Line::getChild() const {
	return child;
}

QColor Line::getColor() const {
	return pen().color();
}

void Line::setColor(const QColor &color) {
	setPen(QPen(color, 1.0));
}

void Line::trackPoints()
{
    setLine(QLineF(parent->pos(), child->pos()));
}
