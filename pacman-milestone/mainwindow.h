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
#include <QVector>

#define xaxis 30
#define yaxis 30
#define length 12


extern char maze[22][37];
extern QGraphicsPixmapItem *web[22][37];
extern QGraphicsScene *scene;
extern QVector<QPair<int,int>> del_awaits; // (i,j)
extern QVector<QGraphicsPixmapItem*> cherries;
extern QVector<QGraphicsPixmapItem*> miniBalls;
extern int total_corn, ate_corn, total_ate; /* count dot and gates for ghosts */

class Character: public QGraphicsPixmapItem {
public:
    virtual void north() = 0;
    virtual void south()=0;
    virtual void west()=0;
    virtual void east()=0;
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
    bool throughgate;
    char dir;

    struct pixels {
        QPixmap pixmap;
        pixels *next;
    };
};

class Pacman: public Character {
public:
    Pacman(int,int);
    void north();
    void south();
    void west();
    void east();
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
    int addscore(int, int);
    int px,py,posx,posy,dir, nextdir,changept;
    bool inbox;
    pixels *pix[4];
   //pixels *pix[4][10], *anindex[4];
};

class Ghost: public Character {
public:
    Ghost(int, int, int);
    void north();
    void south();
    void west();
    void east();
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
    int px,py,posx,posy,dir, nextdir,changept, color;
    bool inbox;
    pixels *anim[4][10], *anindex[4];

};

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    Character *gPac, *ghost[4];
   // QVector<QGraphicsPixmapItem*> cherries;
    void build_maze();
    //void score();
    //void pacman_movement();
    //void ghost_movement();
    ~MainWindow();
public slots:
    void pacman_movement();
    void show_hide_cherry();
    void ghost_movement();
    void score();


private:
    Ui::MainWindow *ui;
    QTimer *PowerBlink, *ptik, *gtik, *scoring;
    int initial_blink = 1;
};


#endif // MAINWINDOW_H
