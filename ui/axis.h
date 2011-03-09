#ifndef AXIS_H
#define AXIS_H

#include <QGraphicsItem>
#include <QColor>
#include <QtGlobal>
#include <QPointF>
#include <QRectF>
#include <QStyleOptionGraphicsItem>
#include <QWidget>
#include <QPainter>
#include <QList>

#include "params.h"
#include "tick.h"

class Axis : public QGraphicsItem {
	public:
		Axis(QPointF *p1, QPointF *p2, qreal width = AXIS_DEFAULT_WIDTH, qreal tickSeparation = AXIS_DEFAULT_TICK_SEPARATION);
		~Axis();
		
		QPointF *getPoint(int i) const;
		
		void drawTicks(QGraphicsScene *scene);
		
		QRectF boundingRect() const;
		QPainterPath shape() const;
		void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
	
	private:
		QPointF *p1;
		QPointF *p2;
		qreal width;
		
		qreal tickSeparation;
		QSet<Tick *> ticks;
};

#endif
