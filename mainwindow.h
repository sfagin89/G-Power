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
#include <iostream>
#include <QIODevice>
#include <QFileSystemWatcher>
#include <QMessageBox>
#define xaxis 30
#define yaxis 30
#define length 12


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


class Character: public QGraphicsPixmapItem {
public:
    virtual void up();
    virtual void down()=0;
    virtual void left()=0;
    virtual void right()=0;
    char direction;
    int positionX;
    int positionY;
    struct animation {
        QPixmap img;
        animation *next;
    };
};

class Pacman: public Character {
public:
    Pacman(int,int);
    void up();
    void down();
    void left();
    void right();
private:
    int x, y, navigate;
};

class ghost: public Character {
public:
    ghost(int,int);
    void up();
    void down();
    void left();
    void right();
private:

};

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    Character *gPac, *ghost[1];
    void build_maze();
   // void pacman_movement();
    ~MainWindow();
private slots:
    void pacman_movement();
    void showModified(const QString& str) {
        Q_UNUSED(str)
        QFile movement("/Users/mandyyao/Desktop/read.txt");
        movement.open(QIODevice::ReadWrite|QIODevice::Text);
        QTextStream stream(&movement);
        QByteArray direction = movement.readLine();
        if (direction != "") {

            char get = direction.at(0); // gets updated text
            qInfo() << get; // prints updated text
            // change global variable = huda
        }
    }
private:
    Ui::MainWindow *ui;
    QGraphicsScene *scene;
    QTimer *ptik;

};


#endif // MAINWINDOW_H
