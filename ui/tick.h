#ifndef TICK_H
#define TICK_H

#include "params.h"

#include <QtGui>

class Tick : public QGraphicsLineItem {
	public:
		Tick(QPointF *center, qreal width = TICK_DEFAULT_WIDTH, qreal height = TICK_DEFAULT_HEIGHT);
	
	private:
		QPointF *center;
		qreal height;
		qreal width;
		QString *text;
};

#endif
