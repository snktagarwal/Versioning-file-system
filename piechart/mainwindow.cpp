#include <QtGui>
#include <cstdlib>

#include "pieview.h"
#include "mainwindow.h"
//FileChooser *f3;
MainWindow::MainWindow(QString mountdir)
{
	QString rootdir = mountdir.replace("mountdir", "rootdir");
	//qDebug() << "rootdir: " << rootdir;
	//qDebug() << "mountdir: " << mountdir_path.replace("mountdir", "rootdir");
	
	QMenu *fileMenu = new QMenu(tr("&File"), this);
	QAction *openAction = fileMenu->addAction(tr("&Open..."));
	openAction->setShortcuts(QKeySequence::Open);
	QAction *saveAction = fileMenu->addAction(tr("&Save As..."));
	saveAction->setShortcuts(QKeySequence::SaveAs);
	QAction *quitAction = fileMenu->addAction(tr("E&xit"));
	quitAction->setShortcuts(QKeySequence::Quit);
	setupModel();
	setupViews();
	showMaximized();

	connect(openAction, SIGNAL(triggered()), this, SLOT(openFile()));
	connect(saveAction, SIGNAL(triggered()), this, SLOT(saveFile()));
	connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));

	menuBar()->addMenu(fileMenu);


	QString disk_space_string;
	QStringList disk_space_pieces;

	vfs_space = vfsSpace(rootdir);
	vfs_data_space = vfsDataSpace(rootdir);
	vfs_metadata_space = vfs_space - vfs_data_space;
	disk_space_string = diskSpace(rootdir);
	disk_space_pieces = disk_space_string.split(" ", QString::SkipEmptyParts);
	disk_used_space = disk_space_pieces.value(2).toULongLong();
	disk_free_space = disk_space_pieces.value(3).toULongLong();
	disk_ext_used_space = disk_used_space - vfs_data_space - vfs_metadata_space;

	openFile(":/Charts/qtdata.cht");

	setWindowTitle(tr("Chart"));
}

void MainWindow::setupModel()
{
	model = new QStandardItemModel(8, 2, this);
	model->setHeaderData(0, Qt::Horizontal, tr("Label"));
	model->setHeaderData(1, Qt::Horizontal, tr("Size in kB"));
}

void MainWindow::setupViews()
{
	QSplitter *splitter = new QSplitter;
	QTableView *table = new QTableView;
	pieChart = new PieView;
	splitter->addWidget(table);
	splitter->addWidget(pieChart);

	int tableWidth = 200;
	QList<int> sizes;
	sizes.append(tableWidth);
	sizes.append(this->geometry().width()-tableWidth);

	splitter->setSizes(sizes);

	table->setModel(model);
	table->setColumnWidth(0,200);
	table->setColumnWidth(1,100);
	
	pieChart->setModel(model);

	QItemSelectionModel *selectionModel = new QItemSelectionModel(model);
	table->setSelectionModel(selectionModel);
	pieChart->setSelectionModel(selectionModel);

	QHeaderView *headerView = table->horizontalHeader();
	headerView->setStretchLastSection(true);

	setCentralWidget(splitter);
}

void MainWindow::openFile(const QString &path)
{
	QString fileName;
	if (path.isNull())
		fileName = QFileDialog::getOpenFileName(this, tr("Choose a data file"), "", "*.cht");
	else
		fileName = path;
	
	if (!fileName.isEmpty()) {
		QFile file(fileName);
	
		if (file.open(QFile::ReadOnly | QFile::Text)) {
			QTextStream stream(&file);
			QString line;
		
			model->removeRows(0, model->rowCount(QModelIndex()), QModelIndex());
			
			int row = 0;
			do {
				line = stream.readLine();
				if (!line.isEmpty()) {
					model->insertRows(row, 1, QModelIndex());
					
					QStringList pieces = line.split(",", QString::SkipEmptyParts);
					model->setData(model->index(row, 0, QModelIndex()),
												 pieces.value(0));
					
					quint64 a;
					switch(row) {
						case 0:
							a=disk_free_space;
							break;
						case 1:
							a=vfs_data_space;
							break;
						case 2:
							a=vfs_metadata_space;
							break;
						case 3:
							a=disk_ext_used_space;
							break;
					}
					
					model->setData(model->index(row, 1, QModelIndex()), a);
					model->setData(model->index(row, 0, QModelIndex()), QColor(pieces.value(2)), Qt::DecorationRole);
					row++;
				}
			} while (!line.isEmpty());
			
			file.close();
		}
	}
}

void MainWindow::saveFile()
{
	QString fileName = QFileDialog::getSaveFileName(this, tr("Save file as"), "", "*.cht");
	
	if (!fileName.isEmpty()) {
		QFile file(fileName);
		QTextStream stream(&file);
		
		if (file.open(QFile::WriteOnly | QFile::Text)) {
			for (int row = 0; row < model->rowCount(QModelIndex()); ++row) {
				QStringList pieces;
				
				pieces.append(model->data(model->index(row, 0, QModelIndex()), Qt::DisplayRole).toString());
				pieces.append(model->data(model->index(row, 1, QModelIndex()), Qt::DisplayRole).toString());
				pieces.append(model->data(model->index(row, 0, QModelIndex()), Qt::DecorationRole).toString());
				
				stream << pieces.join(",") << "\n";
			}
		}
		
		file.close();
	}
}

quint64 MainWindow::vfsSpace(QString rootdir) {
	char cmd[500];
	sprintf(cmd, "cd %s; du -s .| cut -d . -f 1", rootdir.toLatin1().data());
	
	FILE *output = popen(cmd,"r");
	char out[100];
	fgets(out, sizeof(out)-1, output);
	pclose(output);
	qDebug() << "out: " << out;
	
	quint64 usage = atoi(out);
	//printf("Total Space: %ld\n", usage);
	return usage;
}

quint64 MainWindow::vfsDataSpace(QString rootdir) {
	char cmd[500];
	sprintf(cmd, "cd %s; du -s --exclude=*ver* .| cut -d . -f 1", rootdir.toLatin1().data());
	
	FILE *output = popen(cmd,"r");
	char out[100];
	fgets(out, sizeof(out)-1, output);
	pclose(output);
	qDebug() << "out: " << out;
	
	quint64 usage = atoi(out);
	//printf("Used Space: %ld\n", usage);
	return usage;
}

QString MainWindow::diskSpace(QString rootdir) {
	char cmd[500];
	sprintf(cmd, "cd %s; df .| sed '/Filesystem/d'", rootdir.toLatin1().data());
	
	FILE *output = popen(cmd,"r");
	char out[100];
	fgets(out, sizeof(out)-1, output);
	pclose(output);
	qDebug() << "out: " << out;
	
	//printf("Used Space: %ld\n", usage);
	return out;
}
