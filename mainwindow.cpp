#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <QFileSystemWatcher>
#include <QIODevice>
//static int i = 0;
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    scene = new QGraphicsScene(53,52, ui->graphicsView->width(), ui->graphicsView->height());
            //new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);
    setWindowTitle("G-Pow"); /* Title of the pop up screen */
    ui->graphicsView->setStyleSheet("QGraphicsView {border: none;}"); /* screen with no border */
    ui->graphicsView->setBackgroundBrush(Qt::black); /* set background to black color */
    build_maze();
}

QTextStream& qStdOut()
{
    static QTextStream ts( stdout );
    return ts;
}

void MainWindow::pacman_movement() {

}


MainWindow::~MainWindow()
{
    delete ui;
}

