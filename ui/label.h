#ifndef LABEL_H
#define LABEL_H

#include <QGraphicsTextItem>
#include <QEvent>
#include <QString>

#include "point.h"

class Label : public QGraphicsTextItem {
	public:
		Label(const QString &text);
	
	protected:
		bool sceneEventFilter(Point *p, QEvent *event);
	
	private:
};

#endif
