#ifndef POINT_H
#define	POINT_H

#include <QtGui>
#include <QtGlobal>
#include <QObject>

#include "params.h"

class Line;
class GraphWindow;

class Point : public QObject, public QGraphicsItem {
	Q_OBJECT
	//Q_PROPERTY(QColor backgroundColor READ getBackgroundColor WRITE setBackgroundColor)
	
	public:
		Point(QGraphicsScene *scene, qreal x, qreal y, qreal r = POINT_DEFAULT_RADIUS, QString tagText = "", QString tooltipText = "", Point *parent = 0, GraphWindow *window = 0);
		~Point();
		
		qreal getX() const;
		qreal getY() const;
		qreal getXAtScale(float scale) const;
		qreal getRadius() const;
		Point *getParent() const;
		QSet<Point *> getChildren() const;
		int childCount() const;
		int getAncestorCount() const;
		QGraphicsItemAnimation *getAnimation() const;
		QTimeLine *getTimeLine() const;
		qreal getOutlineWidth() const;
		QColor getBackgroundColor() const;
		QGraphicsSimpleTextItem *getTag() const;
		
		void setX(qreal x);
		void setY(qreal y);
		void setRadius(qreal r);
		void setCurrent(bool isCurrent);
		void setAnimation(QGraphicsItemAnimation *animation);
		void setTimeLine(QTimeLine *timeline);
		
    void setOutlineColor(const QColor &color);
    void setOutlineWidth(qreal width);
    void setBackgroundColor(const QColor &color);
    
    void updateTagPosition();
    
    void addLine(Line *line);
    void removeLine(Line *line);
    
    void addChild(Point *child);
    void incrementAncestorCount();
		
		void startTimer(int msec);
		
		QRectF boundingRect() const;
    QPainterPath shape() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
  
  protected:
  	void mousePressEvent(QGraphicsSceneMouseEvent *event);
  	//void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
  	//void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
  	QVariant itemChange(GraphicsItemChange change, const QVariant &value);
  	//bool sceneEventFilter(QGraphicsItem *watched, QEvent *event);
  	void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
	
	public slots:
		void startTimeLine();
		//void animationEnded();
	
	private:
		QRectF outlineRect() const;
		
		QSet<Line *> lines;
		QSet<Point *> children;
		QColor textColor;
		QColor backgroundColor;
		QColor outlineColor;
		qreal outlineWidth;
		
		qreal x;
		qreal y;
		qreal r;
		bool current;
		QString tooltipText;
		QString tagText;
		QGraphicsSimpleTextItem *tag;
		
		Point *parent;
		int ancestorCount;
		QGraphicsItemAnimation *animation;
		QTimeLine *timeline;
		QTimer *timer;
		GraphWindow *window;
};

#endif