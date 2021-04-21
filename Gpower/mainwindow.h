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

class Character: public QGraphicsPixmapItem {
public:
    virtual void go_up();
    virtual void go_down()=0;
    virtual void go_left()=0;
    virtual void go_right()=0;

    struct show {
        QPixmap pic;
        show *next;
    };
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    Character *gPac, *ghost[1];


    void build_maze();
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QGraphicsScene *scene;

};


#endif // MAINWINDOW_H
