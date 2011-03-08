#ifndef POINT_H
#define	POINT_H

#include <QtGlobal>
#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QSet>
#include <QString>
#include <QRectF>
#include <QPainterPath>
#include <QWidget>
#include <QPainter>
#include <QStyleOptionGraphicsItem>

class Line;

class Point : public QGraphicsItem {
	public:
		Point(qreal x, qreal y, qreal r, QString text, Point *parent = 0);
		~Point();
		
		qreal getX() const;
		qreal getY() const;
		qreal getRadius() const;
		Point *getParent() const;
		
		void setY(qreal y);
		void setRadius(qreal r);
		
		//void setText(const QString &text);
    //QString text() const;
    //void setTextColor(const QColor &color);
    //QColor textColor() const;
    //void setOutlineColor(const QColor &color);
    //QColor outlineColor() const;
    void setBackgroundColor(const QColor &color);
    //QColor backgroundColor() const;
    
    void addLine(Line *line);
    void removeLine(Line *line);
    
    void addChild(Point *child);
    QSet<Point *> getChildren() const;
    int childCount() const;
		
		QRectF boundingRect() const;
    QPainterPath shape() const;
    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option, QWidget *widget);
  
  protected:
  	void mousePressEvent(QGraphicsSceneMouseEvent *event);
  	QVariant itemChange(GraphicsItemChange change, const QVariant &value);
	
	private:
		QRectF outlineRect() const;
		
		QSet<Line *> lines;
		QSet<Point *> children;
		QColor textColor;
		QColor backgroundColor;
		QColor outlineColor;
		
		qreal x;
		qreal y;
		qreal r;
		
		Point *parent;
};

#endif
