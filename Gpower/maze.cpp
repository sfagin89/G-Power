#include "ui_mainwindow.h"
#include "mainwindow.h"
#include <QFile>

QGraphicsScene *scene;
int gate_x, gate_y, total_corn, ate_corn, total_ate = 0; /* count dot and gates for ghosts */
QLabel *scoreName, *count, *status_win, *status_lose = 0; /* score counter and status on top of game */
//Character *gPac, *ghost[1]; /* initiate pacman and 4 ghosts */
QVector<QGraphicsPixmapItem*> cherries; /* for score keeping later on */
bool lost=false, won=false; /* turns true lost-->ghost & pac algorithm, won-->total_corn == ate_corn */
char maze[22][37]; /* wall ratio based on maze.txt */
QGraphicsPixmapItem *walls[22][37]; /* wall ratio based on maze.txt */

void MainWindow::build_maze() {
    memset(maze, '0', sizeof(maze));
    QFile Gmaze(":/pacman/maze.txt");
    QPixmap wall(":/pacman/wall.png");
    QPixmap Gpower(":/pacman/Gpower.png");
    QPixmap dot(":/pacman/dot.png");
    QPixmap cherry(":/pacman/cherry.png");
    Gmaze.open(QIODevice::ReadOnly);
    if (!Gmaze.isOpen()) {
        /* error checking, make sure maze.txt exist */
        return;
    }
    QTextStream stream(&Gmaze);
    //QString each_line = stream.readLine();
    for(int i = 1; i < 21; i++) {
        QByteArray maze_line = Gmaze.readLine();
        for (int j = 1; j < 37; j++) {
            maze[i][j] = maze_line.at(j-1);
            switch(maze[i][j]) {
            /*
             * assigning maze[i][j] to either 'Y' or 'N'
             * 'Y' for normal food, pacman, ghost, cherry
             * 'N' for wall, gate, Gpower
            */
            case 'f': /* normal food */
                total_corn++;
                walls[i][j] = new QGraphicsPixmapItem(dot);
                scene->addItem(walls[i][j]);
                walls[i][j]->setPos(xaxis+j*length, yaxis+i*length);
                maze[i][j] = 'Y';
                break;
            case 'w': /* wall */
                walls[i][j] = new QGraphicsPixmapItem(wall);
                scene->addItem(walls[i][j]);
                walls[i][j]->setPos(xaxis+j*length, yaxis+i*length);
                maze[i][j] = 'N';
                break;
            case 'p': /* pacman */
//                walls[i][j] = nullptr;
//                gPac = new Pacman(j,i);
//                gPac->setPos(xaxis+j*length, yaxis+i*length);
//                scene->addItem(gPac);
//                maze[i][j] = 'Y';
                break;
            case 'g': /* ghost */
                maze[i][j] = 'Y';
                break;
            case 'b': /* cherry (power ball) */
                total_corn++;
                walls[i][j] = new QGraphicsPixmapItem(cherry);
                scene->addItem(walls[i][j]);
                walls[i][j]->setPos(xaxis+j*length, yaxis+i*length);
                cherries.push_front(walls[i][j]);
                maze[i][j] = 'Y';
                break;
            case '0': /* gate for ghost */
                walls[i][j] = new QGraphicsPixmapItem(QPixmap(":/pacman/gate.png"));
                scene->addItem((walls[i][j]));
                walls[i][j]->setPos(xaxis+j*length, yaxis+i*length);
                maze[i][j] = 'N';
                break;
            case '9': /* G power symbol */
                walls[i][j] = new QGraphicsPixmapItem(Gpower);
                scene->addItem(walls[i][j]);
                walls[i][j]->setPos(xaxis+j*length, yaxis+i*length);
                gate_x=j;
                gate_y=i;
                maze[i][j] = 'N';
                break;
            }
        }
    }

    status_win = new QLabel(this);
    status_win->hide();
    status_win->setText("Won!");
    status_win->setStyleSheet("color: yellow;"
                              "font: bold;"
                              "font-size: 15px;");
    status_win->setGeometry(408,10,150,25);
    status_lose = new QLabel(this);
    status_lose->hide();
    status_lose->setText("Lost!");
    status_lose->setStyleSheet("color: yellow;"
                               "font: bold;"
                               "font-size: 15px;");
    status_lose->setGeometry(408,10,150,25);

    //gPac->setZvalue(2);
    scoreName = new QLabel(this);
    scoreName->setText("Score");
    scoreName->setStyleSheet("color: yellow;"
                             "font: bold;"
                             "font-size: 15px;");
    scoreName->setGeometry(33,10,60,25);


    count = new QLabel(this);
    count->setIndent(-80);
    count->setText("0");
    count->setStyleSheet("color: yellow;"
                         "font: bold;"
                         "font-size: 15px;");
    count->setGeometry(83,10,60,25);

}
