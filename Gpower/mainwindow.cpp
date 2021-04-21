#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);
    setWindowTitle("G-Pow"); /* Title of the pop up screen */
    ui->graphicsView->setStyleSheet("QGraphicsView {border: none;}"); /* screen with no border */
    ui->graphicsView->setBackgroundBrush(Qt::black); /* set background to black color */
    build_maze();
    ptik = new QTimer(this);
    ptik->start(1);
    connect(ptik,SIGNAL(timeout()),this,SLOT(pacman_movement()));
}



void MainWindow::pacman_movement() {

    QFile movement(":/pacman/move.txt");
    movement.open(QIODevice::ReadOnly);
    QTextStream stream(&movement);
    QByteArray direction = movement.readLine();
    char get = direction.at(0);
    switch(get) {
        case 'U': /* Up */

            break;
        case 'R': /* Down */

            break;
        case 'L': /* Left */

            break;
        case 'D': /* Down */

            break;
        default:
            /* pacman does not move */
            break;
    }

}


MainWindow::~MainWindow()
{
    delete ui;
}

