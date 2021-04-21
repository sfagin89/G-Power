#include "ui_mainwindow.h"
#include "mainwindow.h"
#include <QFile>

QGraphicsScene *scene;
int gate_x, gate_y, total_corn, ate_corn, total_ate = 0; /* count dot and gates for ghosts */
QLabel *score_title, *score, *status_win, *status_lose = 0; /* score counter and status on top of game */
Character *gPac, *ghost[1]; /* initiate pacman and 4 ghosts */
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
    QString each_line = stream.readLine();
    int i = 1;
    while (!each_line.isNull()) {
        QByteArray maze_line = Gmaze.readLine();
        for (int j = 1; j < 37; j++) {
            maze[i][j] = maze_line.at(j-1);
            switch(maze[i][j]) {
            case 'f': /* normal food */
                total_corn++;
                walls[i][j] = new QGraphicsPixmapItem(dot);
                scene->addItem(walls[i][j]);
                walls[i][j]->setPos(xaxis+j*length, yaxis+i*length);
                break;
            case 'w': /* wall */
                walls[i][j] = new QGraphicsPixmapItem(wall);
                scene->addItem(walls[i][j]);
                walls[i][j]->setPos(xaxis+j*length, yaxis+i*length);
                break;
            case 'p': /* pacman */
                break;
            case 'g': /* ghost */
                break;
            case 'b': /* cherry (power ball) */
                total_corn++;
                walls[i][j] = new QGraphicsPixmapItem(cherry);
                scene->addItem(walls[i][j]);
                walls[i][j]->setPos(xaxis+j*length, yaxis+i*length);
                break;
            case '0': /* gate for ghost */
                break;
            case '9': /* G power symbol */
                walls[i][j] = new QGraphicsPixmapItem(Gpower);
                scene->addItem(walls[i][j]);
                walls[i][j]->setPos(xaxis+j*length, yaxis+i*length);
                break;
            }
        }

        i++;
    }










}
