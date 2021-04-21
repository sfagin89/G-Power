#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setStyleSheet("QMainWindow {background: 'black';}"); /* set game background to be black */
    w.show();
    return a.exec();
}
