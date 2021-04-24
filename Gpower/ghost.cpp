#include "mainwindow.h"

//void Item::moveup(){}; //out of line definition

Ghost::Ghost(int j, int i, int ghostNum):px(j),py(i),dir(0) {

    switch(ghostNum){

    case 0:
        setPixmap(QPixmap("/users/hudai/Desktop/BU/EC535/G-Power/GPower/ghost/pink_left.png"));
        // want to set inital values here and define different images to show change in direction
        break;
    case 1:
        setPixmap(QPixmap("/users/hudai/Desktop/BU/EC535/G-Power/GPower/ghost/yellow_left.png"));
        // want to set inital values here and define different images to show change in direction
        break;
    case 2:
        setPixmap(QPixmap("/users/hudai/Desktop/BU/EC535/G-Power/GPower/ghost/red_left.png"));
        // want to set inital values here and define different images to show change in direction
        break;
    case 3:
        setPixmap(QPixmap("/users/hudai/Desktop/BU/EC535/G-Power/GPower/ghost/blue_left.png"));
        // want to set inital values here and define different images to show change in direction
        break;
    }
}
// move pacman_movement from mainwindow.cpp to here and then include it in the function
