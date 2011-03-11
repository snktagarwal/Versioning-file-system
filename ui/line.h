#ifndef LINE_H
#define LINE_H

#include <QtGui>
#include <QtGlobal>
class Point;

class Line : public QGraphicsLineItem {
	public:
		Line(Point *parent, Point *child, qreal width);
		~Line();
		
		Point *getParent() const;
		Point *getChild() const;
		
		void setColor(const QColor &color);
		QColor getColor() const;
		
		void trackPoints();
	
	private:
		Point *parent;
		Point *child;
		qreal width;
};

#endif
