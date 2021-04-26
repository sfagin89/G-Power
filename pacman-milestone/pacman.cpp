#include "mainwindow.h"
Pacman::Pacman(int j, int i):curr_x(j),curr_y(i) {
    setPixmap(QPixmap(":/pacman/start.png"));
    for (int i = 0; i < 4; i++) {
        /* order will be 0,1,2,3 --> north, east, west, south */
        pix[i] = new pixels;
    }
    pix[0]->pixmap.load(":/pacman/0.png");
    pix[1]->pixmap.load(":/pacman/1.png");
    pix[2]->pixmap.load(":/pacman/2.png");
    pix[3]->pixmap.load(":/pacman/3.png");
    }

void Pacman::north() {
    int posy = static_cast<int>(this->y());
    curr_y = (posy-yaxis)/length;
    if(curr_y>0 && maze[curr_y-1][curr_x] == 'Y'){
        addscore(curr_y-1,curr_x);
        for(int i=0; i<length; ++i) {
            this->setPixmap(pix[0]->pixmap);
            this->setY(--posy);
            QCoreApplication::processEvents();
            QThread::usleep(8000);
        }
        posy = static_cast<int>(this->y());
        --curr_y;
    }
}

void Pacman::east() {
    int posx = static_cast<int>(this->x());
    curr_x = (posx-xaxis)/length;
    if(curr_x < 36 && maze[curr_y][curr_x+1] == 'Y'){
        addscore(curr_y,curr_x+1);
        for(int i=0; i<length; ++i) {
            this->setPixmap(pix[1]->pixmap);
            this->setX(++posx);
            QCoreApplication::processEvents();
            QThread::usleep(8000);
        }
        posx = static_cast<int>(this->x());
        ++curr_x;
    }
}

void Pacman::west() {
    int posx = static_cast<int>(this->x());
    curr_x = (posx-xaxis)/length;
    if(curr_x>0 && maze[curr_y][curr_x-1] == 'Y'){
        addscore(curr_y,curr_x-1);
        for(int i=0; i<length; ++i) {
            this->setPixmap(pix[2]->pixmap);
            this->setX(--posx);
            QCoreApplication::processEvents();
            QThread::usleep(8000);
        }
        posx = static_cast<int>(this->x());
        --curr_x;
    }
}

void Pacman::south() {
    int posy = static_cast<int>(this->y());
    curr_y = (posy-yaxis)/length;
    if(curr_y<21 && maze[curr_y+1][curr_x] == 'Y'){
        addscore(curr_y+1,curr_x);
        for(int i=0; i<length; ++i) {
            this->setPixmap(pix[3]->pixmap);
            this->setY(++posy);
            QCoreApplication::processEvents();
            QThread::usleep(8000);
        }
        posy = static_cast<int>(this->y());
        ++curr_y;
    }
}

int Pacman::addscore(int x, int y) {
    if ((x == 18 && y == 19) || (x == 18 && y == 18)) {
        /* do not delete gate */
        return 0;
    }
    for (int i = 0; i < cherries.size(); i++) {
        /* count cherries, score +10; win when total ate equals to total corns */
        if (cherries.at(i) == web[x][y]) {
            total_ate++;
            keep_score +=10;
            count->setText(QString::number(keep_score));
            cherries.remove(i);
            delete web[x][y];
            web[x][y] = nullptr;
            if (total_ate == total_corn) {
                won = true;
            }
            return 0;
        }
    }
    if (miniBalls.indexOf(web[x][y]) != -1) {
        /* count mini balls, score +5; win when total ate equals to total corns */
        total_ate++;
        keep_score +=5;
        count->setText(QString::number(keep_score));
        delete web[x][y];
        web[x][y] = nullptr;
        if (total_ate == total_corn) {
            won = true;
        }
    }
    return 0;
}

