#include<QtGui>
#include<QtGlobal>
 class FileChooser : public QWidget//QMainWindow
    {
        Q_OBJECT
    public:
        FileChooser(char *filepath);
        //QString fileName();
        
  public slots:
        //void setFileName( QString &fn );
    signals:
        //void fileNameChanged( QString & );
    private slots:
        //void chooseFile();
        void showGraph();
    private:
    		QString filepath;
        //QLineEdit *lineEdit;
        //QPushButton *button;
        QPushButton *getGraph;
        QWidget *wid2;
        QLabel *lab2;
        int xmax,xmin;
        double ymax,ymin;
void paintEvent(QPaintEvent *event);
    };

