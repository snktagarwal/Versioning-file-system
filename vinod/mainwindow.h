#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include<QtGui>
#include<QtGlobal>
#include <QMainWindow>
//#include"filechooser.h"
QT_BEGIN_NAMESPACE
class QAbstractItemModel;
class QAbstractItemView;
class QItemSelectionModel;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
   MainWindow();
   QTabWidget *tw;
private slots:
    void openFile(const QString &path = QString());
    void saveFile();
    //void showg();

private:
    void setupModel();
    void setupViews();
    //QPushButton *but;
    //FileChooser *f2;
    
    QAbstractItemModel *model;
    QAbstractItemView *pieChart;
    QItemSelectionModel *selectionModel;
};

#endif
