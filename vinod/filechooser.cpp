#include"filechooser.h"
#include<QtGui>
#include<QtGlobal>
#include<iostream>
int flag=0;
FileChooser::FileChooser( QWidget *parent, char *name)
        :QWidget( parent )
    {
//The layouts in which widgets are placed
QVBoxLayout *layoutv = new QVBoxLayout( this );
QHBoxLayout *layout1 = new QHBoxLayout( this );
QHBoxLayout *layout2 = new QHBoxLayout( this );
   layoutv->setMargin(100);
   resize(2222,5550);
   layout1->setMargin(0);
   layout2->setMargin(0);
/* lab=new QLabel("Individual File Analysis");
 layout1->addWidget(lab);*/
        lineEdit = new QLineEdit( "",this );
        lineEdit->setReadOnly(true);
        layout1->addWidget( lineEdit );
 //when the address is changed over lineEdit text box
       connect( lineEdit, SIGNAL( textChanged(QString & ) ),
                 this, SIGNAL( fileNameChanged(QString & ) ) );

        button = new QPushButton( "Browse", this );
        button->setFixedWidth( button->fontMetrics().width( " Browse " ) );
        getGraph = new QPushButton( "GetGraph", this );
        getGraph->setFixedWidth( button->fontMetrics().width( " GetGraph " ) );
        layout1->addWidget( button );
        layout2->addStretch();
        layout2->addWidget(getGraph);
     QHBoxLayout *lay=new QHBoxLayout();
     lay->setMargin(0);
     QHBoxLayout *laylabel=new QHBoxLayout();
     laylabel->setMargin(0);
    //laylabel->addWidget(lab);
    // layoutv->addLayout(laylabel);
    layoutv->addLayout(layout1);
    layoutv->addLayout(layout2);
    //Adding widget number 2
    wid2=new QWidget();
    //Setting a layout to the widget2 and some label has been already added to layout
    lab2=new QLabel("");
    lay->addWidget(lab2);
    wid2->setLayout(lay);
    wid2->setGeometry(200,100,230,450);
    layoutv->addWidget(wid2); 
    setLayout(layoutv);
        connect( button, SIGNAL( clicked() ),
                 this, SLOT( chooseFile() ) );  
        connect(getGraph, SIGNAL(clicked()),
                    this, SLOT(showGraph()));
   
       }
  void FileChooser::setFileName(QString &fn )
    {
        lineEdit->setText( fn );
    }

    QString FileChooser::fileName()
    {
        return lineEdit->text();
    }
  void FileChooser::chooseFile()
    {
        QString fn;
        //if ( mode() == File )
            fn = QFileDialog::getOpenFileName( this, QString::null, lineEdit->text());
        /*else
            fn = QFileDialog::getExistingDirectory( lineEdit->text(),this );*/

        if ( !fn.isEmpty() ) {
            lineEdit->setText( fn );
            emit fileNameChanged( fn );
        }
    }
  
   void FileChooser::showGraph(){
   flag=1;
   this->repaint();
   this->update();
   this->show();
   
 }
   void FileChooser::paintEvent( QPaintEvent * )
{
        if(flag==1){
        QString str3,str4;int d1=xmax-xmin;double d2=ymax-ymin;
        str3.append(QString("%1").arg(d1));
        str4.append(QString("%1").arg(d2));
  //Scale of the graph
        QString spac="";
        for(int k=0;k<23;k++)spac=spac+"\n";
        //lab2->setText("ActualData/MetaData RATIO"+spac+"\t\t\t\t\tEpoc time"+"\nSCALE:X-axis--->660units-"+str3+"seconds\n         Y-axis---/>310units-"+str4+"ratio");
        QPainter painter(this);
        QColor *color=new QColor(255,255,204);
        Qt::BrushStyle bs=Qt::SolidPattern;
        QBrush *brush=new QBrush(*color, bs);
        painter.setBackground (*brush);
	painter.begin(this);
	painter.setRenderHint(QPainter::Antialiasing, true);
	painter.setPen(QPen(Qt::white, 2, Qt::SolidLine, Qt::RoundCap));
	painter.setBrush(QBrush(*color, Qt::SolidPattern));
	QPoint point=FileChooser::getGraph->pos();
        QPoint point2=FileChooser::lineEdit->pos();
        int xp=point.x();
        int yp=point.y();int xp1=point2.x();
//filling the background
        painter.drawRect(xp1,yp+30,xp-30,500 );
        color=new QColor(153,102,0);
        brush=new QBrush(*color, bs);
        painter.setPen(QPen(Qt::green,0, Qt::SolidLine, Qt::RoundCap));
        color=new QColor(204,204,0);
        brush=new QBrush(*color, bs);
//filling the clor gradient    
        QLinearGradient grad1(xp1+365,yp+80,xp1+365,yp+400);
        grad1.setColorAt(0, Qt::green);
        grad1.setColorAt(1, Qt::blue);
        painter.fillRect(xp1+30,yp+80,670,320,grad1);
        painter.setPen(QPen(Qt::red, 2, Qt::SolidLine, Qt::RoundCap));
//drawing the axis of the graph taking the button positions as the reference
       
        painter.drawLine(xp1+30,yp+70,xp1+30,yp+400);
        painter.drawLine(xp1+30,yp+400,xp1+710,yp+400);
//graph started
        painter.setPen(QPen(Qt::black, 2, Qt::SolidLine, Qt::RoundCap));
//reading from the file-the corresponding points to be joints
        QFile file(lineEdit->text());
	file.open(QIODevice::ReadOnly | QIODevice::Text);
        QStringList list; 
        QStringList pointlist;
	QTextStream in(&file);
	QString line = in.readLine();
        while(!(line.isEmpty()))
        {
          list=line.split(" ");
          line=in.readLine();
//Adding the epoc time and ratio values from every line of the opened file
          pointlist<<list.at(0)<<list.at(3);
        }
	QString str;
        int c=pointlist.size();
        QStringList pl1,pl2,pl3,pl4;int g1=0,g2=1;
//Separating time and ratio values correspondingly to diffrent lists
        for(int g=0;g<c/2;g++)
        {
           pl1<<pointlist.at(g1); 
           pl2<<pointlist.at(g2); 
           g1=g1+2;g2=g2+2;
       }
       bool ok;
       int l1=0,xmin=0;double l2=0,ymin=0;
//finding xmax and ymax from the points to set scale
       for(int h1=0;h1<c/2;h1++)
          {
           int s1=pl1.at(h1).toInt(&ok,10);
           if(s1>=l1)
             l1=s1;
          }
       xmin=l1;
       for(int h1=0;h1<c/2;h1++)
         {
          int s1=pl1.at(h1).toInt(&ok,10);
          if(s1<=xmin)
            xmin=s1;
         }
       for(int h2=0;h2<c/2;h2++)
          {
           double s1=pl2.at(h2).toDouble(&ok);
           if(s1>=l2)
           l2=s1;
          }
       ymin=l2;
       for(int h2=0;h2<c/2;h2++)
         {
          double s1=pl2.at(h2).toDouble(&ok);
          if(s1<=ymin)
            ymin=s1;
         }
       xmax=l1;
       ymax=l2;
       color=new QColor(255,153,0);
       brush=new QBrush(*color, bs);
//Adding the points inforamtion to two lists(x , y separately)
        for(int j=0;j<c/2;j++)
           {
            int aa=pl1.at(j).toInt(&ok,10);
            double bb=pl2.at(j).toDouble(&ok);
            double p1,p2;
            p1=xp1+30+((xmax-aa)*660.0/(xmax-xmin));
            QString str1;
            str1.append(QString("%1").arg(p1));
            pl3<<str1;
            p2=yp+400-(310*bb);//-((310.0)*(bb-ymin)/(ymax-ymin));
            QString str2;
            str2.append(QString("%1").arg(p2));
            pl4<<str2;
//Marking points over the plane
            painter.fillRect((int)p1,(int)p2,6,6,*brush);
           
        }
//Making divisions over the axes
      //X-axis
              for(int i1=0;i1<7;i1++)
               {
                 painter.drawLine(xp1+30+110*i1,yp+395,xp1+30+110*i1,yp+405);
                 int time=xmin+(i1*(xmax-xmin)/6);
                 QString str1;
                 str1.append(QString("%1").arg(time));
                 QFontMetrics metrics=qApp->font();
                 QRectF rect=metrics.boundingRect(str1);
                 painter.drawText(xp1+30+110*i1-(rect.width())/2,yp+420,str1);
               }
      //Y-axis
             for(int i1=0;i1<7;i1++)
               {
                 painter.drawLine(xp1+30-5,yp+400-i1*(310/6),xp1+30+5,yp+400-i1*(310/6));
                 double ratio=ymin+(i1*(ymax-ymin)/6);
                 QString str1;
                 str1.append(QString("%1").arg(ratio));
                 QFontMetrics metrics=qApp->font();
                 QRectF rect=metrics.boundingRect(str1);
                 painter.drawText(xp1+30-rect.width()-2,yp+400-i1*(310/6),str1);
               }
//Mentioning scale and the axes namings
             painter.drawText(xp1+30,yp+50,"ActualData-MetaData ratio");
             painter.drawText(xp1+360,yp+450,"Epoc time");
             painter.drawText(xp1+730,yp+50,"SCALE:");
             int jj=(xmax-xmin);
             QString st4;
             st4.append(QString("%1").arg(jj));
             painter.drawText(xp1+745,yp+62,"X-Axis->660unit="+st4+" sec");
             double jj1=(ymax-ymin);
             QString st5;
             st5.append(QString("%1").arg(jj1));
             painter.drawText(xp1+745,yp+74,"Y-Axis->310unit="+st5+" ratio");
//Joining the adjacent points in the list of points by line
        for(int d=0;d<c/2-1;d++)
          {
           int v1,v2,v3,v4;
           v1=pl3.at(d).toDouble(&ok);
           v2=pl4.at(d).toDouble(&ok);
           v3=pl3.at(d+1).toDouble(&ok);
           v4=pl4.at(d+1).toDouble(&ok);
           painter.drawLine(v1,v2,v3,v4);
        }
        painter.end();
}
 }


