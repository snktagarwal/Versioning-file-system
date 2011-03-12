#include <QtGui>
#include <iostream>

#include "tick.h"
#include "axis.h"

Axis::Axis(QPointF *p1, QPointF *p2, qreal width, qreal tickSeparation) {
	qreal length = p2->x() - p1->x();
	int segmentCount = ((int)(length/tickSeparation)) + 1;
	p2->setX(p1->x()+segmentCount*tickSeparation);
	
	this->p1 = p1;
	this->p2 = p2;
	this->width = width;
	this->tickSeparation = tickSeparation;
	
	setZValue(AXIS_DEFAULT_Z_VALUE);
	//setColor(AXIS_DEFAULT_COLOR);
	setCacheMode(QGraphicsItem::ItemCoordinateCache);
	//setLine(QLineF(p1->x(), p1->y(), p2->x(), p2->y()));
}

Axis::~Axis() {
	foreach(Tick *tick, ticks)
		delete tick;
}

QPointF *Axis::getPoint(int i) const {
	if(i == 1)
		return p1;
	else if(i == 2)
		return p2;
	else
		return NULL;
}

QRectF Axis::boundingRect() const {
    QRectF rect(
    	p1->x(),
    	p1->y() - (width/2),
    	p2->x() - p1->x(),
    	width
    );
    return rect;
}

QPainterPath Axis::shape() const {
	QPainterPath path;
	path.addRect(boundingRect());
	return path;
}

void Axis::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
	QPen pen(QBrush(AXIS_DEFAULT_COLOR), 0.0);
	
	painter->setPen(pen);
	painter->setBrush(AXIS_DEFAULT_COLOR);
	
	painter->drawRect(boundingRect());
	
	//painter->setPen(textColor);
	//painter->drawText();
	update();
}

void Axis::drawTicks(QGraphicsScene *scene, qreal rootX) {
	for( qreal x=p1->x(); x<=p2->x(); x+=tickSeparation ) {
		QString labelText;
		#if defined(__arm__)
			labelText = QString::number(float(x - MARGIN + rootX)/SCALING_FACTOR);
		#else
			labelText = QString::number(double(x - MARGIN + rootX)/SCALING_FACTOR);
		#endif
		
		Tick *tick = new Tick( new QPointF(x,p1->y()), labelText );
		ticks.insert(tick);
		tick->setParentItem(this);
		tick->drawLabel(scene);
		scene->addItem(tick);
	}
}
