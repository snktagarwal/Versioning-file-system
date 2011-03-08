#include <iostream>
#include <QVariant>

#include "point.h"
#include "line.h"

Point::Point(qreal x, qreal y, qreal r, QString text, Point *parent) {	
	this->x = x;
	this->y = y;
	this->r = r;
	this->setToolTip(text);
	this->parent = parent;
	
	textColor = Qt::black;
	backgroundColor = Qt::blue;
	outlineColor = Qt::darkBlue;
	
	this->setFlags(ItemIsSelectable);
	this->setCacheMode(QGraphicsItem::ItemCoordinateCache);
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

void Point::setBackgroundColor(const QColor &color) {
	backgroundColor = color;
}

void Point::addLine(Line *line) {
	lines.insert(line);
	update();
}
void Point::removeLine(Line *line) {
	lines.remove(line);
	update();
}

void Point::addChild(Point *child) {
	children.insert(child);
	std::cout << "No. of children: " << children.size() << std::endl;
}

QRectF Point::boundingRect() const
{
    QRectF rect(getX()-r, getY()-r, 2*r, 2*r);
    return rect;
}

QPainterPath Point::shape() const {
	QPainterPath path;
	path.addEllipse(boundingRect());
	return path;
}

void Point::mousePressEvent(QGraphicsSceneMouseEvent *event) {
	std::cout << "Point (" << getX() << "," << getY() << ") was clicked." << std::endl;
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
	QPen pen(outlineColor);
	if(option->state & QStyle::State_Selected) {
		//setBackgroundColor(QColor(Qt::blue).lighter());
		setBackgroundColor(Qt::red);
	}
	else {
		setBackgroundColor(Qt::blue);
	}
	
	painter->setPen(pen);
	painter->setBrush(backgroundColor);
	
	painter->drawEllipse(boundingRect());
	
	std::cout << cacheMode() << " (" << getX() << "," << getY() << ") isSelected(): " << isSelected() << std::endl;
	
	//painter->setPen(textColor);
	//painter->drawText();
	update();
}
