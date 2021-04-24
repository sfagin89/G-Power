#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QPixmap>
#include <QKeyEvent>
#include <QString>
#include <QObject>
#include <QCoreApplication>
#include <QDebug>
#include <QTimer>
#include <QLabel>
#include "QThread"

#define xaxis 30
#define yaxis 30
#define length 12


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

extern char maze[22][37];
class Character: public QGraphicsPixmapItem {
public:
    virtual int get_dir()=0;
    virtual int get_px()=0;
    virtual int get_py()=0;
    virtual int get_posx()=0;
    virtual int get_posy()=0;
    virtual int get_changept()=0;
    virtual void set_px(int)=0;
    virtual void set_py(int)=0;
    virtual void set_posx(int)=0;
    virtual void set_posy(int)=0;
    virtual void set_dir(int)=0;
    virtual void set_nextdir(int)=0;
    virtual void set_changept(int)=0;
    int state, bluetik,begintik;
    int count;
    bool throughgate;
    char dir;

    struct animation {
        QPixmap img;
        animation *next;
    };
};

class Pacman: public Character {
public:
    Pacman(int,int);
    void moveup();
    void movedown();
    void moveleft();
    void moveright();
    int get_px(){return px;}
    int get_py(){return py;}
    int get_posx(){return posx;}
    int get_posy(){return posy;}
    int get_dir(){return dir;}
    int get_nextdir(){return nextdir;}
    int get_changept(){return changept;}
    void set_px(int n){px=n;}
    void set_py(int n){py=n;}
    void set_posx(int n){posx=n;}
    void set_posy(int n){posy=n;}
    void set_dir(int d){dir = d;}
    void set_nextdir(int nd){nextdir = nd;}
    void set_changept(int npt){changept=npt;}
private:
    int addscore();
    int px,py,posx,posy,dir, nextdir,changept;
    animation *anim[4][10], *anindex[4];
};

class Ghost: public Character {
public:
    Ghost(int,int, int);
    void moveup();
    void movedown();
    void moveleft();
    void moveright();
    int get_px(){return px;}
    int get_py(){return py;}
    int get_posx(){return posx;}
    int get_posy(){return posy;}
    int get_dir(){return dir;}
    int get_nextdir(){return nextdir;}
    int get_changept(){return changept;}
    void set_px(int n){px=n;}
    void set_py(int n){py=n;}
    void set_posx(int n){posx=n;}
    void set_posy(int n){posy=n;}
    void set_dir(int d){dir = d;}
    void set_nextdir(int nd){nextdir = nd;}
    void set_changept(int npt){changept=npt;}
private:
    int px,py,posx,posy,dir, nextdir,changept, count;
    bool inbox;
    animation *anim[4][10], *anindex[4];
private:

};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    Character *gPac, *ghost[1];
    void build_maze();
    void score();
    ~MainWindow();
private slots:
    void pacman_movement();
    void ghost_movement();

private:
    Ui::MainWindow *ui;
    QGraphicsScene *scene;
    QTimer *ptik, *gtik;
};


#endif // MAINWINDOW_H
