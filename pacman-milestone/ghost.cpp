#include "mainwindow.h"

Ghost::Ghost(int j, int i, int c):curr_x(j),curr_y(i){

    switch(c){

    case 0:
        setPixmap(QPixmap(":/ghost/pink_left.png"));

        // want to set inital values here and define different images to show change in direction
        break;
    case 1:
        setPixmap(QPixmap(":/ghost/yellow_left.png"));
        // want to set inital values here and define different images to show change in direction
        break;
    case 2:
        setPixmap(QPixmap(":/ghost/red_left.png"));
        // want to set inital values here and define different images to show change in direction
        break;
    case 3:
        setPixmap(QPixmap(":/ghost/blue_left.png"));
        // want to set inital values here and define different images to show change in direction
        break;
    }

}

/******************************************* unused definition below **********************************************/
void Ghost::north(){
}
void Ghost::south(){
}
void Ghost::east(){
}
void Ghost::west(){
}
