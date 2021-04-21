#include "mainwindow.h"
#include "ui_mainwindow.h"

/* objects needed to update pacman's position
 * 1. current positions of y (posy)
 * 2. current positions of x (posx)
 * 3. current/updated direction of pacman (may not be needed if we have pacman stop when turning into a wall)
 * 4. score
 * 5. win flag
 * 6. lose flag
 * */

/* objects needed to update ghosts's position
 * 1. current positions of y (posy)
 * 2. current positions of x (posx)
 * 3. current/updated direction of pacman (may not be needed if we have pacman stop when turning into a wall)
 * 4. steps (how many steps has it taken in the current direction to know when it is time to switch directions)
 * */

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



void MainWindow::pacman_movement() { /* needs UI update to show
                                       appropriate pacman image*/

    QFile movement(":/pacman/move.txt");
    movement.open(QIODevice::ReadOnly);
    QTextStream stream(&movement);
    QByteArray direction = movement.readLine();
    char get = direction.at(0);
    xpos = gPac->get_posx();
    ypos = gPac->get_posy();
    switch(get) {
        case 'U': /* Up */
            if(map[ypos-1][xpos] == '1' || ypos-1 < 0){
                gPac->dir = 'S'; // direction of pacman is stoped
            }
            else{
                gPac->dir = 'U';
                gPac->posy = --ypos;
            }
            break;
        case 'R': /* Right */
            if(map[ypos][xpos+1] == '1' || xpos+1 >36)/* if we have reached
                                                        a wall or the end of the map*/
            {
                gPac->dir = 'S'; // direction of pacman is stoped
            }
            else{
                gPac->dir = 'R';
                gPac->posx = ++xpos;
            }
            break;
        case 'L': /* Left */
            if(map[ypos][xpos-1] == '1' || xpos-1 <0)/* if we have reached
                                                        a wall or the end of the map*/
            {
                gPac->dir = 'S'; // direction of pacman is stoped
            }
            else{
                gPac->dir = 'L';
                gPac->posx = --xpos;
            }
            break;
        case 'D': /* Down */
            if(map[ypos+1][xpos] == '1' || ypos+1 >36)/* if we have reached
                                                        a wall or the end of the map*/
            {
                gPac->dir = 'S'; // direction of pacman is stoped
            }
            else{
                gPac->dir = 'D';
                gPac->posx = ++ypos;
            }
            break;
        default: /* this is the case where we have direction stopped or an empty file (ex. when game starts)
            /* pacman does not move */
            break;
    }

}

MainWindow::ghost_movement(){ /* ghost is expected to back and forth from its current direction*/

for(i=0 ; i<1 ; i++){
   xpos = ghost[i]->get_px();
   ypos = ghost[i]->get_py();
   dir = ghost[i]->dir;

   steps = ghost[i]->count;

   switch(dir) {

    case 'U': /* Up */


        if(map[ypos-1][xpos] == '1' || steps>=3){
            ghost[i]->dir = 'D';
            ghost[i]->count = 0;
        }
        else{
            ghost[i]->set_py(ypos-1);
            ghost[i]->count = ++steps;
        }
        break;
    case 'R': /* Right */


        if(map[ypos][xpos+1] == '1' || steps>=3)/* if we have reached
                                                    a wall or the end of the map*/
        {
            ghost[i]->dir = 'L';
            ghost[i]->count = 0;
        }
        else{
            ghost[i]->set_px(xpos+1);
            ghost[i]->count = ++steps;
        }
        break;
    case 'L': /* Left */


        if(map[ypos][xpos-1] == '1' || steps >=3)/* if we have reached
                                                    a wall or the end of the map*/
        {
            ghost[i]->dir = 'R';
            ghost[i]->count = 0;
        }
        else{
            ghost[i]->set_px(xpos-1);
            ghost[i]->count = ++steps;
        }
        break;
    case 'D': /* Down */

        if(map[ypos+1][xpos] == '1' || ypos+1 >36)/* if we have reached
                                                    a wall or the end of the map*/
        {
            ghost[i]->dir = 'U';
            ghost[i]->count = ++steps;
        }
        else{
            ghost[i]->set_py(ypos+1);
            ghost[i]->count = ++steps;
        }
        break;
    default: /* this is the case where we have direction stopped or an empty file (ex. when game starts)
        /* pacman does not move */
        break;
}

}

}

MainWindow::~MainWindow()
{
    delete ui;
}

