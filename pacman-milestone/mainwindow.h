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
#include <fstream>

#define xaxis 30
#define yaxis 30
#define length 12


extern char maze[22][37];
extern QGraphicsPixmapItem *web[22][37];
extern QGraphicsScene *scene;
extern QVector<QGraphicsPixmapItem*> cherries;
extern QVector<QGraphicsPixmapItem*> miniBalls;
extern int total_corn, keep_score, total_ate; /* count dot and gates for ghosts */
extern bool lost, won;
extern QLabel *scoreName, *count, *status_win, *status_lose;
extern int cherryFreeze;
extern bool cherryFreezeFlag;


class Character: public QGraphicsPixmapItem {
public:
    virtual void north() = 0;
    virtual void south()=0;
    virtual void west()=0;
    virtual void east()=0;
    virtual int get_curr_x()=0;
    virtual int get_curr_y()=0;
    virtual void set_curr_x(int)=0;
    virtual void set_curr_y(int)=0;
    char compass;

    struct pixels {
        QPixmap pixmap;
    };
    pixels *ghost_pix[4][2];
};

extern Character *gPac, *ghost[4];

class Pacman: public Character {
public:
    Pacman(int,int);
    void north();
    void south();
    void west();
    void east();
    int get_curr_x(){return curr_x;}
    int get_curr_y(){return curr_y;}
    void set_curr_x(int n){curr_x=n;}
    void set_curr_y(int n){curr_y=n;}
private:
    int addscore(int, int);
    int curr_x,curr_y,compass;
    pixels *pix[4];
};

class Ghost: public Character {
public:
    Ghost(int, int, int);
    void north();
    void south();
    void west();
    void east();
    int get_curr_x(){return curr_x;}
    int get_curr_y(){return curr_y;}
    void set_curr_x(int n){curr_x=n;}
    void set_curr_y(int n){curr_y=n;}
private:
    int curr_x,curr_y;
    //pixels *ghost_pix[4][2];
};

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    void build_maze();
    ~MainWindow();
public slots:
    void pacman_movement();
    void show_hide_cherry();
    void ghost_movement();
    void exit_function();
private:
    Ui::MainWindow *ui;
    QTimer *PowerBlink, *ptik, *gtik, *exit_;
    int initial_blink = 1;
};


#endif // MAINWINDOW_H
