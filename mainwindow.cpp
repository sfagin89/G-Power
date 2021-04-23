#include "mainwindow.h"
#include "ui_mainwindow.h"

/* objects needed to update pacman's position
 * 1. current positions of y (posy)
 * 2. current positions of x (posx)
 * 3. current/updated direction of pacman (may not be needed if we have pacman stop when turning into a maze)
 * 4. score
 * 5. win flag
 * 6. lose flag
 * */

/* objects needed to update ghosts's position
 * 1. current positions of y (posy)
 * 2. current positions of x (posx)
 * 3. current/updated direction of pacman (may not be needed if we have pacman stop when turning into a maze)
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
    ptik->start(2000);
    connect(ptik,SIGNAL(timeout()),this,SLOT(pacman_movement()));
    //connect(ptik,SIGNAL(timeout()),this,SLOT(gPac->pacman_movement())); // is is for when the function gets
                                                                          // gets moved into the other file
}



void MainWindow::pacman_movement() { /* needs UI update to show
                                       appropriate pacman image*/
    int xpos, ypos;
    QFile movement(":/pacman/move.txt");
    movement.open(QIODevice::ReadOnly);
    QTextStream stream(&movement);
    QByteArray direction = movement.readLine();
    char get = direction.at(0);
    xpos = gPac->get_px();
    ypos = gPac->get_py();
    /* the print statments below show that the algorithm can detect
     * the wall that the Pacman runs into in the up or down direction
     * and we can see that the px and py variables re updated each time*/
    qInfo() << "y pos:";
    qInfo() << ypos;
    switch(get) {
        case 'U': /* Up */
            if(maze[ypos-1][xpos] == 'N' || ypos-1 < 0){
                gPac->dir = 'S'; // direction of pacman is stoped
            }
            else{
                gPac->dir = 'U';
                gPac->set_py(--ypos);
            }
            break;
        case 'R': /* Right */
            if(maze[ypos][xpos+1] == 'N' || xpos+1 >36)/* if we have reached
                                                        a maze or the end of the maze*/
            {
                gPac->dir = 'S'; // direction of pacman is stoped
            }
            else{
                gPac->dir = 'R';
                gPac->set_px(++xpos);
            }
            break;
        case 'L': /* Left */
            if(maze[ypos][xpos-1] == 'N' || xpos-1 <0)/* if we have reached
                                                        a maze or the end of the maze*/
            {
                gPac->dir = 'S'; // direction of pacman is stoped
            }
            else{
                gPac->dir = 'L';
                gPac->set_px(--xpos);
            }
            break;
        case 'D': /* Down */
            if(maze[ypos+1][xpos] == 'N' || ypos+1 >36)/* if we have reached
                                                        a maze or the end of the maze*/
            {
                gPac->dir = 'S'; // direction of pacman is stoped
            }
            else{
                gPac->dir = 'D';
                gPac->set_py(++ypos);
            }
            break;
        default: /* this is the case where we have direction stopped or an empty file (ex. when game starts)*/
            break;


    }
    int new_posx = gPac->get_px();
    int new_posy = gPac->get_px();
    gPac->setPos(xaxis+(new_posx*length), yaxis+(new_posy*length));
    QCoreApplication::processEvents();
    QThread::usleep(500);

}

//MainWindow::ghost_movement(){ /* ghost is expected to back and forth from its current direction*/

//for(i=0 ; i<1 ; i++){
//   xpos = ghost[i]->get_px();
//   ypos = ghost[i]->get_py();
//   dir = ghost[i]->dir;

//   steps = ghost[i]->count;

//   switch(dir) {

//    case 'U': /* Up */


//        if(maze[ypos-1][xpos] == '1' || steps>=3){
//            ghost[i]->dir = 'D';
//            ghost[i]->count = 0;
//        }
//        else{
//            ghost[i]->set_py(ypos-1);
//            ghost[i]->count = ++steps;
//        }
//        break;
//    case 'R': /* Right */


//        if(maze[ypos][xpos+1] == '1' || steps>=3)/* if we have reached
//                                                    a maze or the end of the maze*/
//        {
//            ghost[i]->dir = 'L';
//            ghost[i]->count = 0;
//        }
//        else{
//            ghost[i]->set_px(xpos+1);
//            ghost[i]->count = ++steps;
//        }
//        break;
//    case 'L': /* Left */


//        if(maze[ypos][xpos-1] == '1' || steps >=3)/* if we have reached
//                                                    a maze or the end of the maze*/
//        {
//            ghost[i]->dir = 'R';
//            ghost[i]->count = 0;
//        }
//        else{
//            ghost[i]->set_px(xpos-1);
//            ghost[i]->count = ++steps;
//        }
//        break;
//    case 'D': /* Down */

//        if(maze[ypos+1][xpos] == '1' || ypos+1 >36)/* if we have reached
//                                                    a maze or the end of the maze*/
//        {
//            ghost[i]->dir = 'U';
//            ghost[i]->count = ++steps;
//        }
//        else{
//            ghost[i]->set_py(ypos+1);
//            ghost[i]->count = ++steps;
//        }
//        break;
//    default: /* this is the case where we have direction stopped or an empty file (ex. when game starts)
//        /* pacman does not move */
//        break;
//}

//}

//}
void MainWindow::score(){



}
MainWindow::~MainWindow()
{
    delete ui;
}

