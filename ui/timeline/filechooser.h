#include <QtGui>
#include <QtGlobal>

class FileChooser : public QWidget
{
	Q_OBJECT

	public:
		FileChooser(char *filepath);
	
	private slots:
		void showGraph();

	private:
		QString filepath;
		QPushButton *getGraph;
		QWidget *wid2;
		QLabel *lab2;
		int xmax,xmin;
		double ymax,ymin;
		void paintEvent(QPaintEvent *event);
};

