#ifndef TICK_H
#define TICK_H

#include "params.h"

#include <QtGui>

class Tick : public QGraphicsLineItem {
	public:
		Tick(QPointF *center, QString labelText, qreal width = TICK_DEFAULT_WIDTH, qreal height = TICK_DEFAULT_HEIGHT);
		
		void drawLabel(QGraphicsScene *scene);
	
	private:
		QPointF *center;
		qreal height;
		qreal width;
		QString labelText;
		QGraphicsSimpleTextItem *label;
};

#endif
