/*#include"filechooser.h"
#include<QtGui>
#include<QtGlobal>
void FileChooser::paintEvent( QPaintEvent * )
{
	 FileChooser::QObject::connect(getGraph, SIGNAL(clicked()),
                    wid2, SLOT(showGraph()));
       
}
void showGraph()
{
  QPainter painter( this );
        QColor *color=new QColor(100,132,23,255);
        Qt::BrushStyle bs=Qt::SolidPattern;
        QBrush *brush=new QBrush(*color, bs);
        painter.setBackground (*brush);
	painter.begin(this);
	painter.setRenderHint(QPainter::Antialiasing, true);
	painter.setPen(QPen(Qt::white, 2, Qt::SolidLine, Qt::RoundCap));
	painter.setBrush(QBrush(Qt::green, Qt::SolidPattern));
	QPoint point=FileChooser::getGraph->pos();
        QPoint point2=FileChooser::lineEdit->pos();
        int xp=point.x();
        int yp=point.y();
        painter.drawRect(20,yp+30,1000,500 );
        painter.setPen(QPen(Qt::black, 2, Qt::SolidLine, Qt::RoundCap));
        painter.drawLine(50,yp+80,50,yp+400);
        painter.drawLine(50,yp+400,800,yp+400);
        //graph started
        painter.drawLine(50,yp+400,50,yp+200);
	painter.drawLine(50,yp+200,150,yp+220);
        painter.drawLine(150,yp+220,200,yp+120);
         painter.drawLine(200,yp+120,300,yp+240);
        painter.end();
 }*/
