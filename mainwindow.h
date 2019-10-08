#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <windows.h>
#include <QMainWindow>
#include <QPainter>
#include <QMessageBox>
#include <QIcon>
#include <QLabel>
#include <QMovie>
#include <QSize>
#include <QKeyEvent>
#include <QAction>
#include <QTimer>
#include <QDebug>
#include <QWidget>
#include <QLineEdit>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QTime>
#include <QFile>
#include <QInputDialog>

#include "http.h"
#include "game.h"

using namespace std;

struct Pst
{
    int x;
    int y;
    int drt;
    int step;   //步进，范围-9到10
    bool eaten; //鬼在惊吓状态下是否被玩家吃掉
    std::list<Point *> path;
};
namespace Ui
{
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void paintEvent(QPaintEvent *);
    //void myPaintEvent();
    void keyPressEvent(QKeyEvent *event);
    void playerMoveEvent(Player &x, int identity);
    void getScreenPosition(int x, int y, int step, int drt, int *outx, int *outy, int identity);
    void initGame();

    int map[31][28] = {{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, {1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1}, {1, 3, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 3, 1}, {1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1}, {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, {1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 0, 1}, {1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 0, 1}, {1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1}, {1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 5, 5, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 2, 2, 2, 2, 2, 2, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1}, {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 2, 2, 2, 2, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, {1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 2, 2, 2, 2, 2, 2, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1}, {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, {1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1}, {1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1}, {1, 3, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 3, 1}, {1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1}, {1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1}, {1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1}, {1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1}, {1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1}, {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};
    //地图说明：0表示豆子，1表示墙，2表示鬼屋内部，3表示大力，4表示空地，5表示鬼屋门

    QLabel *labelPlayer = new QLabel(this);
    QLabel *labelGhost[4];
    QMovie *gifPlayer[4];
    QMovie *gifGhost[4];

    QMovie *gifScared = new QMovie(":/source/img/scared.gif");
    QMovie *gifShine = new QMovie(":/source/img/shine.gif");
    QMovie *gifEye = new QMovie(":/source/img/eye.gif");

    QMediaPlayer *mediaWin = new QMediaPlayer;
    QMediaPlayer *mediaOver = new QMediaPlayer;
    QMediaPlayer *mediaEaten = new QMediaPlayer;
    QMediaPlayer *mediaEat = new QMediaPlayer;
    QMediaPlayer *mediaPower = new QMediaPlayer;
    QMediaPlaylist *mediaBgmLoop = new QMediaPlaylist;
    QMediaPlayer *mediaBgm = new QMediaPlayer;

    int px, py; //用于承接计算得到的屏幕像素
    int gX[4];
    int gY[4];

    int remainTime1 = -1, remainTime2 = -1;
    int totalScore, lowStep, highStep, acStep, updateTime; //级别，计算加权总分，步进低，步进高，吃到大力之后的加速程度，全局刷新周期。上述变量（除了totalScore）的计算均在playerMoveEvent下与步进相关的区域完成
    float mulStep;
    // 用于屏幕显示的倍数
    QTimer *scaredTimer1 = new QTimer(this);
    QTimer *scaredTimer2 = new QTimer(this);
    QTimer *gameProcTimer = new QTimer(this);
    QTimer *updateTimer = new QTimer(this);

    Pacman pacman;
    Ghost ghost[4];

    QTime m_time;
    int m_frameCount;

public slots:
    void scaredSlot1();
    void scaredSlot2();

private slots:
    void on_b_pause_clicked(bool checked);
    void gameProc();
    void on_b_stop_clicked(bool checked);

    void on_HiFps_stateChanged(int arg1);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
