  #include <QApplication>
    //#include "filechooser.h"
#include"mainwindow.h"
    int main( int argc, char ** argv )
    {
        QApplication a( argc, argv );
        //FileChooser *fc = new FileChooser;
        MainWindow *window=new MainWindow;
        //MainWindow::tw->show();//window->show();
        //fc->show();
        return a.exec();
    }

