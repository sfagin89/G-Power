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
    scene = new QGraphicsScene(53, 52, ui->graphicsView->width(), ui->graphicsView->height());
    ui->graphicsView->setScene(scene);
    setWindowTitle("G-Pow"); /* Title of the pop up screen */
    ui->graphicsView->setStyleSheet("QGraphicsView {border: none;}"); /* screen with no border */
    ui->graphicsView->setBackgroundBrush(Qt::black); /* set background to black color */
    build_maze();
   // score();
//    delete web[17][18];
//    web[17][18] = nullptr;
    ptik = new QTimer(this);
    ptik->start(1000);
    connect(ptik,SIGNAL(timeout()),this,SLOT(pacman_movement()));
    PowerBlink = new QTimer(this);
    PowerBlink->start(1000);
    connect(PowerBlink, SIGNAL(timeout()),this,SLOT(show_hide_cherry()));

    ghost[0]->dir = 'U';
    ghost[1]->dir = 'D';
    ghost[2]->dir = 'U';
    ghost[3]->dir = 'L';
    gtik = new QTimer(this);
    gtik->start(300);
    connect(gtik,SIGNAL(timeout()),this,SLOT(ghost_movement()));


//    scoring = new QTimer(this);
//    scoring->start(200);
//    connect(scoring, SIGNAL(timeout()),this,SLOT(score()));



//    gtik = new QTimer(this);
//    gtik->start(100);
   // connect(ptik,SIGNAL(timeout()),this,SLOT(ghost_movement()));
//    connect(ptik,SIGNAL(timeout()),this,SLOT(gPac->pacman_movement())); // is is for when the function gets
//                                                                          // gets moved into the other file
}


void MainWindow::show_hide_cherry() {
    qInfo() << "size: " << cherries.size();
    initial_blink = (initial_blink^1);
    if (initial_blink == 0) {
        for (int num = 0; num < cherries.size(); num++) {
            cherries.at(num)->hide();
        }
    }
    else if (initial_blink == 1) {
        for (int num = 0; num < cherries.size(); num++) {
            cherries.at(num)->show();
        }
    }
}

void MainWindow::pacman_movement() { /* needs UI update to show
                                       appropriate pacman image*/
    int xpos, ypos;
    QFile movement("/Users/mandyyao/Desktop/535Submit/pacman-milestone/pacman/move.txt");
    movement.open(QIODevice::ReadOnly);
    QTextStream stream(&movement);
    QByteArray direction = movement.readLine();
    char get = direction.at(0);
    xpos = gPac->get_px();
    ypos = gPac->get_py();
    /* the print statments below show that the algorithm can detect
     * the wall that the Pacman runs into in the up or down direction
     * and we can see that the px and py variables re updated each time*/
    switch(get) {
        case 'U': /* Up */
       //     qInfo() << "up!!!!";
            if(maze[ypos-1][xpos] == 'N' || ypos-1 < 0){
                gPac->dir = 'S'; // direction of pacman is stoped
            }
            else{
                gPac->dir = 'U';
                gPac->set_py(--ypos);
                gPac->north();
         //       qInfo() << "north";
            }
            break;
        case 'R': /* Right */
           // qInfo() << "Right!!!!";
            if(maze[ypos][xpos+1] == 'N' || xpos+1 >36)/* if we have reached
                                                        a maze or the end of the maze*/
            {
                gPac->dir = 'S'; // direction of pacman is stoped
            }
            else{
                gPac->dir = 'R';
                gPac->set_px(++xpos);
                gPac->east();
        //        qInfo() << "east";
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
                gPac->west();
         //       qInfo() << "west";
            }
            break;
        case 'D': /* Down */
       // qInfo() << "Down!!!!";
            if(maze[ypos+1][xpos] == 'N' || ypos+1 > 21)/* if we have reached
                                                        a maze or the end of the maze*/
            {
                gPac->dir = 'S'; // direction of pacman is stoped
            }
            else{
                gPac->dir = 'D';
                gPac->set_py(++ypos);
                gPac->south();
           //     qInfo() << "south";

            }
            break;
        default: /* this is the case where we have direction stopped or an empty file (ex. when game starts)*/
            break;


    }
//    int new_posx = gPac->get_px();
//    int new_posy = gPac->get_py();
//    this->setPixmap()
//    gPac->setPos(xaxis+new_posx*length, yaxis+new_posy*length);
//    web[new_posx][new_posy] = nullptr;
//    QCoreApplication::processEvents();
//    QThread::usleep(500);

}

void MainWindow::ghost_movement(){ /* ghost is expected to back and forth from its current direction*/
        int i;
        char dir;
        for(i=0 ; i<4 ; i++){
        int xpos = ghost[i]->get_px();
        int ypos = ghost[i]->get_py();
        dir = ghost[i]->dir;

        switch(dir) {

        case 'U': /* Up */


           if(maze[ypos-1][xpos] == 'N' ){
               ghost[i]->dir = 'D';
           }
           else{
               ghost[i]->set_py(ypos-1);
           }
           break;
        case 'R': /* Right */


           if(maze[ypos][xpos+1] == 'N' )/* if we have reached
                                                       a maze or the end of the maze*/
           {
               ghost[i]->dir = 'L';
           }
           else{
               ghost[i]->set_px(xpos+1);
           }
           break;
        case 'L': /* Left */


           if(maze[ypos][xpos-1] == 'N' )/* if we have reached
                                                       a maze or the end of the maze*/
           {
               ghost[i]->dir = 'R';
           }
           else{
               ghost[i]->set_px(xpos-1);
           }
           break;
        case 'D': /* Down */

           if(maze[ypos+1][xpos] == 'N' )/* if we have reached
                                                       a maze or the end of the maze*/
           {
               ghost[i]->dir = 'U';
           }
           else{
               ghost[i]->set_py(ypos+1);
           }
           break;
        default: /* this is the case where we have direction stopped or an empty file (ex. when game starts)
            pacman does not move */
           break;
        }
        int newx = ghost[i]->get_px();
        int newy = ghost[i]->get_py();
        ghost[i]->setPos(xaxis+newx*length, yaxis+newy*length);
        QCoreApplication::processEvents();
        QThread::usleep(8000);

    }
}
void MainWindow::score() {
//   qInfo() << "called score!";
//   delete web[6][12];
//   web[6][12] = nullptr;
//    int x = gPac->get_px();
//    qInfo() << x;
//    int y = gPac->get_py();
//    qInfo() << y;
//    delete web[y][x];
//    web[y][x] = nullptr;



//    delete web[y][x];
//    web[y][x] = nullptr;
//    delete web[18][17];
//    web[18][17] = nullptr;
//    qInfo() << "score called ";
//    if (del_awaits.empty()) return;
//    for (int i = 0; i < del_awaits.size(); i++) {
//        qInfo() << "size: " << del_awaits.size();
//        qInfo() << del_awaits.at(i);
        //qInfo() << del_awaits.at(i).first << del_awaits.at(i).second;
//        delete web[17][18];
//        web[17][18] = nullptr;
//        delete web[(int)del_awaits.at(i).first][(int)del_awaits.at(i).second];
//        web[(int)del_awaits.at(i).first][(int)del_awaits.at(i).second] = nullptr;
//        del_awaits.erase(del_awaits.begin());
//    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
