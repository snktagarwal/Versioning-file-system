#include "tick.h"

Tick::Tick(QPointF *center, qreal width, qreal height) {
	this->center = center;
	this->width = width;
	this->height = height;
	
	setZValue(TICK_DEFAULT_Z_VALUE);
	setPen(QPen(TICK_DEFAULT_COLOR));
	setLine(QLineF(
		center->x(),
		center->y()-(height/2),
		center->x(),
		center->y()+(height/2)
	));
}
