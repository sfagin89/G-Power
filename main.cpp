#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QFileSystemWatcher watcher;
    watcher.addPath("/Users/mandyyao/Desktop/read.txt");
    QStringList file = watcher.files();
    Q_FOREACH(QString directory, file)
        qDebug() << "Directory name" << directory << "\n";
    MainWindow* mc = new MainWindow;
    QObject::connect(&watcher, SIGNAL(fileChanged(QString)), mc, SLOT(showModified(QString)));
    MainWindow w;
    w.setStyleSheet("QMainWindow {background: 'black';}"); /* set game background to be black */
    w.show();
    return a.exec();
}
