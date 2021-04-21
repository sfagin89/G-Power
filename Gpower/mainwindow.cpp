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
            xpos = gpac->get_posx();
            ypos = gpac->get_posy();

            if(map[ypos-1][xpos] == '1' || ypos-1 < 0){
                gpac->dir = 'S'; // direction of pacman is stoped
            }
            else{
                gpac->dir = 'U';
                gpac->posy = --ypos;
            }
            break;
        case 'R': /* Right */
            xpos = gpac->get_posx();
            ypos = gpac->get_posy();

            if(map[ypos][xpos+1] == '1' || xpos+1 >36)/* if we have reached
                                                        a wall or the end of the map*/
            {
                gpac->dir = 'S'; // direction of pacman is stoped
            }
            else{
                gpac->dir = 'R';
                gpac->posx = ++xpos;
            }
            break;
        case 'L': /* Left */
            xpos = gpac->get_posx();
            ypos = gpac->get_posy();

            if(map[ypos][xpos-1] == '1' || xpos-1 <0)/* if we have reached
                                                        a wall or the end of the map*/
            {
                gpac->dir = 'S'; // direction of pacman is stoped
            }
            else{
                gpac->dir = 'L';
                gpac->posx = --xpos;
            }
            break;
        case 'D': /* Down */
            xpos = gpac->get_posx();
            ypos = gpac->get_posy();

            if(map[ypos+1][xpos] == '1' || ypos+1 >36)/* if we have reached
                                                        a wall or the end of the map*/
            {
                gpac->dir = 'S'; // direction of pacman is stoped
            }
            else{
                gpac->dir = 'D';
                gpac->posx = ++ypos;
            }
            break;
        default: /* this is the case where we have direction stopped or an empty file (ex. when game starts)
            /* pacman does not move */
            break;
    }

}


MainWindow::~MainWindow()
{
    delete ui;
}

