#include "mainwindow.h"

//void Item::moveup(){}; //out of line definition

Ghost::Ghost(int j, int i):px(j),py(i),dir(0) {

    setPixmap(QPixmap("/users/hudai/Desktop/BU/EC535/G-Power/GPower/ghost/pink_left.png"));
    // want to set inital values here and define different images to show change in direction
    }

// move pacman_movement from mainwindow.cpp to here and then include it in the function
