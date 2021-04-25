#include "mainwindow.h"


QVector<QPair<int,int>> del_awaits;

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
  //  qInfo() << "called up";
    int posy = static_cast<int>(this->y());
   // int posx = static_cast<int>(this->x());
    //MainWindow w;
//    delete web[17][18];
//    web[17][18] = nullptr


//    for (int i = 0; i < w.miniBalls.size(); i++) {
//       // qInfo() << w.miniBalls.at(i);
//    }
   // qInfo() << "called up 1";
    //  int remender = (posy-yaxis)%length;
    py = (posy-yaxis)/length;
    dir = 0;
    while(py>0 && maze[py-1][px] == 'Y'){
    //    qInfo() << "called up 2" << py-1 << px;
        addscore(py-1,px);
        for(int i=0; i<length; ++i) {
            this->setPixmap(pix[0]->pixmap);
            this->setY(--posy);
            QCoreApplication::processEvents();
            QThread::usleep(8000);
        }
//        qInfo() << "called up 3";
//        if (w.miniBalls.contains(web[py-1][px])) {
//              qInfo() << "yes!" << py-1 << px;
//             // w.score();

//             // del_awaits.push_back(qMakePair(py-1,px));
////              delete w.web[17][18];
////              w.web[17][18] = nullptr;
//        }
//        else if (w.cherries.contains(web[py-1][px])) {
//           // qInfo() << "powerball";
//        }
       // qInfo() << "here: " << py-1 << px;
        --py;
    }

}

void Pacman::east() {
 //   qInfo() << "called right";
    int posx = static_cast<int>(this->x());
    px = (posx-xaxis)/length;
  //  int remender = (posy-yaxis)%length;
    dir = 0;
    while(px < 36 && maze[py][px+1] == 'Y'){
        addscore(py,px+1);
        for(int i=0; i<length; ++i) {
            this->setPixmap(pix[1]->pixmap);
            this->setX(++posx);
            QCoreApplication::processEvents();
            QThread::usleep(8000);
        }
        ++px;
    }
}

void Pacman::west() {
  //  qInfo() << "called left";
    int posx = static_cast<int>(this->x());
    px = (posx-xaxis)/length;
  //  int remender = (posy-yaxis)%length;
    dir = 0;
    while(px>0 && maze[py][px-1] == 'Y'){
        addscore(py,px-1);
        for(int i=0; i<length; ++i) {
            this->setPixmap(pix[2]->pixmap);
            this->setX(--posx);
            QCoreApplication::processEvents();
            QThread::usleep(8000);
        }
        --px;
    }
}

void Pacman::south() {
   // qInfo() << "called down";
    int posy = static_cast<int>(this->y());
  //  int remender = (posy-yaxis)%length;
    py = (posy-yaxis)/length;
    dir = 0;
    while(py<21 && maze[py+1][px] == 'Y'){
        addscore(py+1,px);
        for(int i=0; i<length; ++i) {
            this->setPixmap(pix[3]->pixmap);
            this->setY(++posy);
            QCoreApplication::processEvents();
            QThread::usleep(8000);
        }
        ++py;
    }
}

int Pacman::addscore(int x, int y) {
   // qInfo() << "cherry size! : "<< cherries.size();
    if (x == 18 && y == 19) {
        return 0;
    }

    if (x == 18 && y == 18) {
        return 0;
    }

    for (int i = 0; i < cherries.size(); i++) {
     //   qInfo() << "compare: "<< cherries.at(i)  << web[x][y];
        if (cherries.at(i) == web[x][y]) {
       //     qInfo() << "cherries: " << cherries.at(i);
            cherries.remove(i);
         //   qInfo() << "here size: " << cherries.size();
            delete web[x][y];
            web[x][y] = nullptr;
            return 0;
        }
    }
    qInfo() << x << y;
    delete web[x][y];
    web[x][y] = nullptr;
    return 0;
}
