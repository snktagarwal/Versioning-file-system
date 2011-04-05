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
	
	public:
		Point(QString filepath, QGraphicsScene *scene, qreal x, qreal y, qreal r = POINT_DEFAULT_RADIUS, QString tagText = "", QString tooltipText = "", Point *parent = 0, GraphWindow *window = 0);
		~Point();

		qreal getX() const;
		qreal getY() const;
		qreal getXAtScale(float scale) const;
		qreal getRadius() const;
		Point *getParent() const;
		QSet<Point *> getChildren() const;
		int childCount() const;
		int getAncestorCount() const;
		qreal getOutlineWidth() const;
		QColor getBackgroundColor() const;
		QGraphicsSimpleTextItem *getTag() const;
		int getInvalidCount() const;
		bool isValid() const;
		bool isHead() const;
		QString getRelativeFilePath() const;
		QAction *getCheckoutAction() const;
		QAction *getRevertAction() const;

		void setX(qreal x);
		void setY(qreal y);
		void setRadius(qreal r);
		void setCurrent(bool isCurrent);
		void setInvalidCount(int invalidCount);
		void setValidity(bool valid);
		void setCheckoutEnabled(bool enabled);
		void setRevertEnabled(bool enabled);
		void setTagText(QString tagText);

		void setOutlineColor(const QColor &color);
		void setOutlineWidth(qreal width);
		void setBackgroundColor(const QColor &color);

		void updateTagPosition();

		void addLine(Line *line);
		void removeLine(Line *line);
		void deleteAllLines();
		void deleteTag();

		void addChild(Point *child);
		void removeChild(Point *child);
		void incrementAncestorCount();

		QRectF boundingRect() const;
		QPainterPath shape() const;
		void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
		
		void updateContextMenu();
		void updateContextMenus();
		void removeInvalidVersions(Point *oldCurrent, Point *newCurrent);
  
	protected:
		void mousePressEvent(QGraphicsSceneMouseEvent *event);
		QVariant itemChange(GraphicsItemChange change, const QVariant &value);
		void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
	
	private slots:
		void checkoutVersion();
		void revertToVersion();
		void tagVersion();
	
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
		bool valid;
		QString relativeFilePath;
		QString tooltipText;
		QString tagText;
		QGraphicsSimpleTextItem *tag;
		
		QAction *checkoutAction;
		QAction *revertAction;
		QAction *tagAction;
		
		Point *parent;
		int ancestorCount;
		int invalidCount;
		GraphWindow *window;
};

#endif
