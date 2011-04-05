#include <QtGui>
#include <QtGlobal>
#include <iostream>

#include "filechooser.h"

#define MIN_MAX
// #define ZERO_ONE

#define META_DATA_FILE ("/tmp/rvfs/md")

int flag=0;

FileChooser::FileChooser(char *filepath) {
	this->filepath = filepath;
	QString command = "__guimd "+this->filepath;
	system(command.toLatin1().data());

	//The layouts in which widgets are placed
	showGraph();
} 

void FileChooser::showGraph() {
	flag = 1;
	this->repaint();
	this->update();
	this->show();
}

void FileChooser::paintEvent(QPaintEvent *) {
	QFontMetrics metrics=qApp->font();
	
	// getting the available space in the tab
	QRect geom = this->geometry();
	int tabHeight = geom.height();
	int tabWidth = geom.width();
	
	if(flag==1) {
		QString str3,str4;
		int d1 = xmax - xmin;
		double d2 = ymax - ymin;
		str3.append(QString("%1").arg(d1));
		str4.append(QString("%1").arg(d2));
		
		//Scale of the graph
		QString spac = "";
		
		for(int k=0;k<23;k++)
			spac = spac + "\n";
		
		QPainter painter(this);
		
		QColor *color = new QColor(255,255,204);
		Qt::BrushStyle bs = Qt::SolidPattern;
		QBrush *brush = new QBrush(*color, bs);
		
		painter.setBackground(*brush);
		painter.begin(this);
		painter.setRenderHint(QPainter::Antialiasing, true);
		painter.setPen(QPen(Qt::white, 2, Qt::SolidLine, Qt::RoundCap));
		painter.setBrush(QBrush(*color, Qt::SolidPattern));
    
    int topMargin = 50;
    int bottomMargin = 50;
    int leftMargin = 80;
    int rightMargin = leftMargin;
    int graphWidth = tabWidth-leftMargin-rightMargin;
    int graphHeight = tabHeight-topMargin-bottomMargin;
    int graphOutlineWidth = 2;
    
		// filling the background
		color = new QColor(153,102,0);
		brush = new QBrush(*color, bs);
		painter.setPen(QPen(Qt::green,0, Qt::SolidLine, Qt::RoundCap));
		color = new QColor(204,204,0);
		brush = new QBrush(*color, bs);
		
		// filling the clor gradient
		QLinearGradient grad1(leftMargin+graphWidth/2,topMargin,leftMargin+graphWidth/2,topMargin+graphHeight);
		grad1.setColorAt(0, Qt::green);
		grad1.setColorAt(1, Qt::blue);
		
		painter.fillRect(leftMargin, topMargin, graphWidth, graphHeight, grad1);
		painter.setPen(QPen(Qt::black, 2, Qt::SolidLine, Qt::RoundCap));
		
		// drawing the axis of the graph taking the button positions as the reference
		painter.drawLine(leftMargin, topMargin, leftMargin, topMargin+graphHeight);
		painter.drawLine(leftMargin, topMargin+graphHeight, leftMargin+graphWidth, topMargin+graphHeight);
		
		// graph started
		painter.setPen(QPen(Qt::black, 2, Qt::SolidLine, Qt::RoundCap));
		
		//reading from the file-the corresponding points to be joints
		QFile file(META_DATA_FILE);
		file.open(QIODevice::ReadOnly | QIODevice::Text);
		QStringList list;
		QStringList pointlist;
		QTextStream in(&file);
		QString line = in.readLine();
		while(!(line.isEmpty())) {
			list = line.split(" ");
			line = in.readLine();
			
			//Adding the epoch time and ratio values from every line of the opened file
			pointlist << list.at(0) << list.at(3);
		}
		
		QString str;
		int c=pointlist.size();
		QStringList pl1,pl2,pl3,pl4;int g1=0,g2=1;
		
		//Separating time and ratio values correspondingly to diffrent lists
		for(int g=0;g<c/2;g++) {
			pl1 << pointlist.at(g1);
			pl2 << pointlist.at(g2); 
			g1 = g1 + 2;
			g2 = g2 + 2;
		}
		bool ok;
		int l1 = 0, xmin = 0;
		double l2 = 0, ymin = 0;
		
		//finding xmax and ymax from the points to set scale
		for(int h1=0;h1<c/2;h1++) {
			int s1 = pl1.at(h1).toInt(&ok,10);
			if(s1>=l1)
				l1 = s1;
		}
		xmin = l1;
       
		for(int h1=0;h1<c/2;h1++) {
			int s1 = pl1.at(h1).toInt(&ok,10);
			if(s1<=xmin)
				xmin = s1;
		}

		for(int h2=0;h2<c/2;h2++) {
			double s1 = pl2.at(h2).toDouble(&ok);
			if(s1>=l2)
				l2 = s1;
		}
		ymin = l2;

		for(int h2=0;h2<c/2;h2++) {
			double s1 = pl2.at(h2).toDouble(&ok);
			if(s1<=ymin)
				ymin = s1;
		}
		
		xmax = l1;
		ymax = l2;
		
		color = new QColor(255,153,0);
		brush = new QBrush(*color, bs);
		
		//Adding the points inforamtion to two lists(x , y separately)
		for(int j=0;j<c/2;j++) {
			int aa = pl1.at(j).toInt(&ok,10);
			double bb = pl2.at(j).toDouble(&ok);
			double p1,p2;
			p1 = leftMargin + ( (aa-xmin) * graphWidth / (xmax-xmin) );
			QString str1;
			str1.append(QString("%1").arg(p1));
			pl3 << str1;
			#ifdef MIN_MAX
				p2 = topMargin + graphHeight - ( (graphHeight) * (bb-ymin) / (ymax-ymin) );
			#endif
			#ifdef ZERO_ONE
				p2 = topMargin + graphHeight - graphHeight * bb;
			#endif
			QString str2;
			str2.append(QString("%1").arg(p2));
			pl4 << str2;
		}
		
		//Making divisions over the axes
		int tickHeight = 10;
		
		//X-axis
		for(int i1=0;i1<7;i1++) {
			painter.drawLine(leftMargin+(i1*graphWidth)/6, topMargin+graphHeight-tickHeight/2, leftMargin+(i1*graphWidth)/6, topMargin+graphHeight+tickHeight/2);
			
			int time = xmin + ( i1 * (xmax-xmin) / 6 );
			QString str1;
			str1 = QDateTime::fromMSecsSinceEpoch(1000*qint64(time)).toString("hh:mm AP, d MMM, yyyy");
			
			QRectF rect = metrics.boundingRect(str1);
			painter.drawText( leftMargin + (i1*graphWidth) / 6 - rect.width() / 2, topMargin + graphHeight + rect.height() + 10, str1);
		}

		//Y-axis
		for(int i1=0;i1<7;i1++) {
			painter.drawLine(leftMargin-tickHeight/2, topMargin+graphHeight-(i1*graphHeight)/6, leftMargin+tickHeight/2, topMargin+graphHeight-(i1*graphHeight)/6);
			double ratio=ymin+(i1*(ymax-ymin)/6);
			
			QString str1;
			str1.append(QString("%1").arg(ratio));
			str1 = str1.left(7);
			
			QRectF rect=metrics.boundingRect(str1);
			painter.drawText(leftMargin-rect.width()-10, topMargin+graphHeight-(i1*graphHeight)/6 + rect.height()/2, str1);
		}
		
		//Mentioning scale and the axes namings
		QString graphTitle = "Actual Data-Meta Data Ratio v/s Time";
		QString XLabel = "Time";
		QString YLabel = "Ratio";
		QRectF graphTitleRect = metrics.boundingRect(graphTitle);
		QRectF XLabelRect = metrics.boundingRect(XLabel);
		QRectF YLabelRect = metrics.boundingRect(YLabel);
		
		painter.drawText(leftMargin, topMargin/2+graphTitleRect.height()/2, graphTitle);
		painter.drawText(leftMargin+graphWidth/2-XLabelRect.width()/2, topMargin+graphHeight+2*XLabelRect.height()+10, XLabel);
		painter.rotate(90.0);
		painter.drawText(topMargin+graphHeight/2-YLabelRect.width()/2, -5, YLabel);
		painter.rotate(-90.0);
		
		//Joining the adjacent points in the list of points by line
		for(int d=0;d<c/2-1;d++) {
			int v1,v2,v3,v4;
			v1 = pl3.at(d).toDouble(&ok);
			v2 = pl4.at(d).toDouble(&ok);
			v3 = pl3.at(d+1).toDouble(&ok);
			v4 = pl4.at(d+1).toDouble(&ok);
			painter.drawLine(v1,v2,v3,v4);
		}
		
		for(int j=0;j<c/2;j++) {
			int aa = pl1.at(j).toInt(&ok,10);
			double p1 = leftMargin + ( (aa-xmin) * graphWidth / (xmax-xmin) );
			double bb = pl2.at(j).toDouble(&ok);
			double p2;
			
			#ifdef MIN_MAX
				p2 = topMargin + graphHeight - ( (graphHeight) * (bb-ymin) / (ymax-ymin) );
			#endif
			#ifdef ZERO_ONE
				p2 = topMargin + graphHeight - graphHeight * bb;
			#endif
			
			//Marking points over the plane
			int pointHeight = 6;
			int pointWidth = pointHeight;
			painter.fillRect((int)p1-pointWidth/2,(int)p2-pointHeight/2, pointWidth, pointHeight, *brush);
		}
		
		painter.end();
}
 }
