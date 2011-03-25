  #include <QApplication>
  #include <iostream>
    //#include "filechooser.h"
#include"mainwindow.h"
    int main( int argc, char ** argv )
    {
    		if(argc != 2) {
    			std::cout << "Invalid input." << std::endl;
    			return EXIT_FAILURE;
    		}
        QApplication a( argc, argv );
        //FileChooser *fc = new FileChooser;
        MainWindow *window=new MainWindow( QString(argv[1]) );
        //MainWindow::tw->show();
        window->show();
        //fc->show();
        return a.exec();
    }

