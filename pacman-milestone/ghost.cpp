#include "mainwindow.h"

//void Character::north(){}; //out of line definition

Ghost::Ghost(int j, int i, int c):px(j),py(i),color(c){

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

void Ghost::north(){
    int ypos = this->get_py();
    int xpos = this->get_px();
    if(maze[ypos-1][xpos] == 'N' )
    {
        this->dir = 'D';
        this->set_py(ypos+1);
    }
    else{
         this->set_py(ypos-1);
    }
}
void Ghost::south(){
    int ypos = this->get_py();
    int xpos = this->get_px();
    if(maze[ypos][xpos-1] == 'N' )/* if we have reached a maze or the end of the maze*/
    {
        this->dir = 'R';
        this->set_px(xpos+1);
    }
    else{
        this->set_px(xpos-1);
    }

}
void Ghost::east(){
//    int ypos = this->get_py();
    int xpos = this->get_px();
//    if(maze[ypos][xpos+1] == 'N')/* if we have reached wall of maze or the end of the maze*/
//    {   qInfo()<< "EAST WALL";
//        this->dir = 'L';
//        this->set_px(xpos-1);
//        qInfo() << this->dir;
//     }
//    else{
        qInfo()<< "EAST NO WALL";
        this->set_px(xpos+1);
        qInfo() << this->dir;
//    }
}
void Ghost::west(){
    qInfo() << this;
    int ypos = this->get_py();
    int xpos = this->get_px();
    if(maze[ypos][xpos-1] == 'N' )/* if we have reached maze or the end of the maze*/
    {   qInfo()<< "I am here";
        this->dir = 'R';
        this->set_px(xpos+1);
    }
    else{
        qInfo() << "I am else :)";
        this->set_px(xpos-1);
    }
}
