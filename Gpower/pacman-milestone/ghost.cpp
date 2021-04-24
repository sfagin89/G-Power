#include "mainwindow.h"

//void Character::north(){}; //out of line definition

Ghost::Ghost(int j, int i, int c):px(j),py(i),color(c){


//    // want to set inital values here and define different images to show change in direction

// }

//Ghost::Ghost(int j, int i, int ghostNum):px(j),py(i),dir(0) {

//    switch(ghostNum){

//    case 0:
        setPixmap(QPixmap(":/ghost/pink_left.png"));
//        // want to set inital values here and define different images to show change in direction
//        break;
//    case 1:
//        setPixmap(QPixmap(":/ghost/yellow_left.png"));
//        // want to set inital values here and define different images to show change in direction
//        break;
//    case 2:
//        setPixmap(QPixmap(":/ghost/red_left.png"));
//        // want to set inital values here and define different images to show change in direction
//        break;
//    case 3:
//        setPixmap(QPixmap(":/ghost/blue_left.png"));
//        // want to set inital values here and define different images to show change in direction
//        break;
//    }
}
// move pacman_movement from mainwindow.cpp to here and then include it in the function
void Ghost::north(){

}
void Ghost::south(){

}
void Ghost::east(){

}
void Ghost::west(){

}
