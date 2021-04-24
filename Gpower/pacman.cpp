#include "mainwindow.h"

void Character::north(){}; //out of line definition

Pacman::Pacman(int j, int i):px(j),py(i),dir(0) {

    setPixmap(QPixmap(":/pacman/start.png"));
    // order will be 0,1,2,3 --> north, east, west, south
    for (int i = 0; i < 4; i++) {
        pix[i] = new pixels;
    }
    pix[0]->pixmap.load(":/pacman/0.png");
    pix[1]->pixmap.load(":/pacman/1.png");
    pix[2]->pixmap.load(":/pacman/2.png");
    pix[3]->pixmap.load(":/pacman/3.png");

    // want to set inital values here and define different images to show change in direction

    }

// move pacman_movement from mainwindow.cpp to here and then include it in the function

void Pacman::north() {
    //qInfo() << "called up";
    int posy = static_cast<int>(this->y());
   // qInfo() << posy;
    int posx = static_cast<int>(this->x());
  //  qInfo() << posx;
   // QGraphicsPixmapItem * temp[22][37];
    MainWindow w;
    for (int i = 0; i < w.miniBalls.size(); i++) {
       // qInfo() << w.miniBalls.at(i);
    }

    //  int remender = (posy-yaxis)%length;
    py = (posy-yaxis)/length;
    dir = 0;
    while(py>0 && maze[py-1][px] == 'Y'){
        if (w.miniBalls.contains(w.web[py-1][px])) {
            //  qInfo() << "yes!";
            //  w.web[px][py-1]->setPixmap(pix[0]->pixmap);
//              delete w.web[py-1][px];
//              w.web[py-1][px] = nullptr;
        }
        else if (w.cherries.contains(w.web[py-1][px])) {
           // qInfo() << "powerball";
        }
       // qInfo() << "here: " << py-1 << px;



        for(int i=0; i<length; ++i) {
            this->setPixmap(pix[0]->pixmap);
            this->setY(--posy);
            QCoreApplication::processEvents();
            QThread::usleep(8000);
        }
        --py;
    }

}

void Pacman::east() {
    qInfo() << "called right";
//    int new_posx = gPac->get_px();
//    int new_posy = gPac->get_py();
//    this->setPixmap(pix[1]->pixmap);
//    gPac->setPos(xaxis+new_posx*length, yaxis+new_posy*length);
//    QCoreApplication::processEvents();
//    QThread::usleep(500);
}

void Pacman::west() {
    qInfo() << "called left";
//    int new_posx = gPac->get_px();
//    int new_posy = gPac->get_py();
//    this->setPixmap(pix[2]->pixmap);
//    gPac->setPos(xaxis+new_posx*length, yaxis+new_posy*length);
//    QCoreApplication::processEvents();
//    QThread::usleep(500);
}


void Pacman::south() {
    qInfo() << "called down";
    int posy = static_cast<int>(this->y());
  //  int remender = (posy-yaxis)%length;
    py = (posy-yaxis)/length;
    dir = 0;
    while(py<21 && maze[py+1][px] == 'Y'){
        for(int i=0; i<length; ++i) {
            this->setPixmap(pix[3]->pixmap);
            this->setY(++posy);
            QCoreApplication::processEvents();
            QThread::usleep(8000);
        }
        ++py;
    }
}

int Pacman::addscore() {



    return 0;
}
