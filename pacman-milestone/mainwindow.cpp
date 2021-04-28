#include "mainwindow.h"
#include "ui_mainwindow.h"
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    /************************************************ GUI definition below *************************************************/
    ui->setupUi(this);
    scene = new QGraphicsScene(53, 52, ui->graphicsView->width(), ui->graphicsView->height());
    ui->graphicsView->setScene(scene);
    setWindowTitle("G-Pow"); /* Title of the pop up screen */
    ui->graphicsView->setStyleSheet("QGraphicsView {border: none;}"); /* screen with no border */
    ui->graphicsView->setBackgroundBrush(Qt::black); /* set background to black color */
    build_maze();
    /************************************************ GUI definition above *************************************************/
    /******************************************* pacman call definition below **********************************************/
    //QString filename = "/Users/mandyyao/Desktop/535Submit/pacman-milestone/pacman/move.txt";
    QString filename = "/root/move.txt";
    QFile movement(filename);
    if (!movement.open(QIODevice::ReadWrite)) {
        qInfo() << "file open error";
    }
    ptik = new QTimer(this);
    ptik->start(5);
    connect(ptik,SIGNAL(timeout()),this,SLOT(pacman_movement()));
    /******************************************* pacman call definition above **********************************************/

    /******************************************* cherry call definition below **********************************************/
    PowerBlink = new QTimer(this);
    PowerBlink->start(1000);
    connect(PowerBlink, SIGNAL(timeout()),this,SLOT(show_hide_cherry()));
    /******************************************* cherry call definition above **********************************************/

    /******************************************* ghost call definition below **********************************************/
    ghost[0]->compass = 'U';
    ghost[1]->compass = 'D';
    ghost[2]->compass = 'U';
    ghost[3]->compass = 'L';
    gtik = new QTimer(this);
    gtik->start(200);
    connect(gtik,SIGNAL(timeout()),this,SLOT(ghost_movement()));
    /******************************************* ghost call definition above **********************************************/


    /******************************************* exit call function above **********************************************/
    exit_ = new QTimer(this);
    exit_->start(5000);
    connect(exit_,SIGNAL(timeout()),this,SLOT(exit_function()));
    /******************************************* exit call function above **********************************************/

}


void MainWindow::show_hide_cherry() {
    /* allows the blinking of cherries using XOR bit operations */
    if (cherries.empty()) {
       /* stop timer when no more cherries */
       PowerBlink->stop();
    }
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

void MainWindow::pacman_movement() {
    /* reads sensor movement text file and triggers movement of pacman */
   for (int i = 0; i < 4; i++) {
       /* checks if any ghost intersects with pacman
          lose when detected an intersection and stop all timer
        */
       if (ghost[i]->get_curr_x() == gPac->get_curr_x() && ghost[i]->get_curr_y() == gPac->get_curr_y()) {
           gtik->stop();
           ptik->stop();
           PowerBlink->stop();
           status_lose->show();
           lost = true;
       }
   }
    if(won){
        /* won when total corns equals to corns ate */
        gtik->stop();
        ptik->stop();
        PowerBlink->stop();
        status_win->show();
    }
    int x_where, y_where;
    //QFile movement("/Users/mandyyao/Desktop/535Submit/pacman-milestone/pacman/move.txt"); /* sensor file */
    QFile movement("/root/move.txt"); /* sensor file */
    movement.open(QIODevice::ReadOnly);
    QTextStream stream(&movement);
    QByteArray direction = movement.readLine();
    if (direction == nullptr) {
        /* error checking: when text file has nothing, should not crash */
        return;
    }
    char get = direction.at(0);
    x_where = gPac->get_curr_x();
    y_where = gPac->get_curr_y();
    switch(get) {
        case 'U': /* Up */
            if(maze[y_where-1][x_where] == 'N' || y_where-1 < 0){
                gPac->compass = 'S'; // direction of pacman is stoped
            }
            else{
                gPac->compass = 'U';
                gPac->set_curr_y(--y_where);
                gPac->north();
            }
            break;
        case 'R': /* Right */
            if(maze[y_where][x_where+1] == 'N' || x_where+1 >36)/* if we have reached
                                                        a maze or the end of the maze*/
            {
                gPac->compass = 'S'; // direction of pacman is stoped
            }
            else{
                gPac->compass = 'R';
                gPac->set_curr_x(++x_where);
                gPac->east();
            }
            break;
        case 'L': /* Left */
            if(maze[y_where][x_where-1] == 'N' || x_where-1 <0)/* if we have reached
                                                        a maze or the end of the maze*/
            {
                gPac->compass = 'S'; // direction of pacman is stoped
            }
            else{
                gPac->compass = 'L';
                gPac->set_curr_x(--x_where);
                gPac->west();
            }
            break;
        case 'D': /* Down */
            if(maze[y_where+1][x_where] == 'N' || y_where+1 > 21)/* if we have reached
                                                        a maze or the end of the maze*/
            {
                gPac->compass = 'S'; // compassection of pacman is stoped
            }
            else{
                gPac->compass = 'D';
                gPac->set_curr_y(++y_where);
                gPac->south();

            }
            break;
        default: /* pacman stops for inputs other than 'U', 'D', 'L', 'R' */
            break;
    }
}

void MainWindow::ghost_movement(){
    /* ghost is expected to back and forth from its current compassection*/
    int i;
    char direction;
    for(i=0 ; i<4 ; i++){
        if (cherryFreezeFlag == true && cherryFreeze != 0) {
            --cherryFreeze;
            continue;
        }
        else if (cherryFreeze == 0) {
            cherryFreezeFlag = false;
        }
        int x_where = ghost[i]->get_curr_x();
        int y_where = ghost[i]->get_curr_y();
        direction = ghost[i]->compass;
        switch(direction) {
        case 'U': /* Up */
           if(maze[y_where-1][x_where] == 'N' ) {
               /* if we have reached a maze or the end of the maze*/
               ghost[i]->compass = 'D';
           }
           else{
               if (ghost[i]->get_curr_x() == gPac->get_curr_x() && ghost[i]->get_curr_y() == gPac->get_curr_y()) {
                   /* Checking for collision before movement of ghost */
                   gtik->stop();
                   ptik->stop();
                   PowerBlink->stop();
                   status_lose->show();
                   lost = true;
               }
               ghost[i]->set_curr_y(y_where-1);
               if (ghost[i]->get_curr_x() == gPac->get_curr_x() && ghost[i]->get_curr_y() == gPac->get_curr_y()) {
                   /* Checking for collision after movement of ghost */
                   gtik->stop();
                   ptik->stop();
                   PowerBlink->stop();
                   status_lose->show();
                   lost = true;
               }
           }
           break;
        case 'R': /* Right */
           if(maze[y_where][x_where+1] == 'N' ) {
               /* if we have reached a maze or the end of the maze*/
               ghost[i]->compass = 'L';
           }
           else{
               if (ghost[i]->get_curr_x() == gPac->get_curr_x() && ghost[i]->get_curr_y() == gPac->get_curr_y()) {
                   /* Checking for collision before movement of ghost */
                   gtik->stop();
                   ptik->stop();
                   PowerBlink->stop();
                   status_lose->show();
                   lost = true;
               }
               ghost[i]->set_curr_x(x_where+1);
               if (ghost[i]->get_curr_x() == gPac->get_curr_x() && ghost[i]->get_curr_y() == gPac->get_curr_y()) {
                   /* Checking for collision after movement of ghost */
                   gtik->stop();
                   ptik->stop();
                   PowerBlink->stop();
                   status_lose->show();
                   lost = true;
               }
           }
           break;
        case 'L': /* Left */

           if(maze[y_where][x_where-1] == 'N' ) {
               /* if we have reached a maze or the end of the maze*/
               ghost[i]->compass = 'R';
           }
           else{
               if (ghost[i]->get_curr_x() == gPac->get_curr_x() && ghost[i]->get_curr_y() == gPac->get_curr_y()) {
                   /* Checking for collision before movement of ghost */
                   gtik->stop();
                   ptik->stop();
                   PowerBlink->stop();
                   status_lose->show();
                   lost = true;
               }
               ghost[i]->set_curr_x(x_where-1);
               if (ghost[i]->get_curr_x() == gPac->get_curr_x() && ghost[i]->get_curr_y() == gPac->get_curr_y()) {
                   /* Checking for collision after movement of ghost */
                   gtik->stop();
                   ptik->stop();
                   PowerBlink->stop();
                   status_lose->show();
                   lost = true;
               }
           }
           break;
        case 'D': /* Down */

           if(maze[y_where+1][x_where] == 'N' ) {
               /* if we have reached a maze or the end of the maze*/
               ghost[i]->compass = 'U';
           }
           else{
               if (ghost[i]->get_curr_x() == gPac->get_curr_x() && ghost[i]->get_curr_y() == gPac->get_curr_y()) {
                   /* Checking for collision before movement of ghost */
                   gtik->stop();
                   ptik->stop();
                   PowerBlink->stop();
                   status_lose->show();
                   lost = true;
               }
               ghost[i]->set_curr_y(y_where+1);
               if (ghost[i]->get_curr_x() == gPac->get_curr_x() && ghost[i]->get_curr_y() == gPac->get_curr_y()) {
                   /* Checking for collision after movement of ghost */
                   gtik->stop();
                   ptik->stop();
                   PowerBlink->stop();
                   status_lose->show();
                   lost = true;
               }
           }
           break;
        default:
           break;
            }
        int newx = ghost[i]->get_curr_x();
        int newy = ghost[i]->get_curr_y();
        ghost[i]->setPos(xaxis+newx*length, yaxis+newy*length);
        QCoreApplication::processEvents();
        QThread::usleep(8000);
    }
}

void MainWindow::exit_function() {
    if (won || lost) {
        /* timer checks every 5 seconds on whether won or lost flag is turned on to exit game */
        QThread::sleep(3);
        QCoreApplication::quit(); // quits the program
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
