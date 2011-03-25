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
   MainWindow(QString mountdir);
   QTabWidget *tw;
private slots:
    void openFile(const QString &path = QString());
    void saveFile();
    //void showg();

private:
    void setupModel();
    void setupViews();
    quint64 vfs_space, vfs_data_space, vfs_metadata_space, disk_free_space, disk_used_space, disk_ext_used_space;
    quint64 vfsSpace(QString rootdir);
    quint64 vfsDataSpace(QString rootdir);
    QString diskSpace(QString rootdir); 
    //QPushButton *but;
    //FileChooser *f2;
    
    quint64 values[4];
    QAbstractItemModel *model;
    QAbstractItemView *pieChart;
    QItemSelectionModel *selectionModel;
};

#endif
