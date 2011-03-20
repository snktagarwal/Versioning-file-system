#include "tick.h"

Tick::Tick(QPointF *center, QString labelText, qreal width, qreal height) {
	this->center = center;
	this->width = width;
	this->height = height;
	this->labelText = labelText;
	
	setZValue(TICK_DEFAULT_Z_VALUE);
	setPen(QPen(TICK_DEFAULT_COLOR));
	setLine(QLineF(
		center->x(),
		center->y()-(height/2),
		center->x(),
		center->y()+(height/2)
	));
}

void Tick::drawLabel(QGraphicsScene *scene) {
	label = new QGraphicsSimpleTextItem(labelText);
	QFontMetricsF metrics = qApp->font();
	QRectF rect = metrics.boundingRect(labelText);
	qreal labelTextX = center->x() - (rect.width()/2) + 30;
	qreal labelTextY = 5;//center->y() - height - (rect.height()/2) - 15;
	label->setPos(labelTextX, labelTextY);
	scene->addItem(label);
}
