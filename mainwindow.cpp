#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : //初始化函数
                                          QMainWindow(parent),
                                          ui(new Ui::MainWindow)
{
    srand(time(0));
    ui->setupUi(this);
    QFont font;
    font.setFamily("微软雅黑");
    font.setPixelSize(16);
    this->setFont(font);
    this->setStyleSheet("#MainWindow{background-image: url(:/source/img/welcome.jpg);}");

    ui->b_stop->setVisible(false);
    ui->b_pause->setVisible(false);
    ui->HiFps->setVisible(false);
    statePause = -1;

    labelGhost[0] = new QLabel(this);
    labelGhost[1] = new QLabel(this);
    labelGhost[2] = new QLabel(this);
    labelGhost[3] = new QLabel(this);

    gifPlayer[0] = new QMovie(":/source/img/playerA.gif");
    gifPlayer[1] = new QMovie(":/source/img/playerD.gif");
    gifPlayer[2] = new QMovie(":/source/img/playerW.gif");
    gifPlayer[3] = new QMovie(":/source/img/playerS.gif");

    gifGhost[0] = new QMovie(":/source/img/Ghost1.gif");
    gifGhost[1] = new QMovie(":/source/img/Ghost2.gif");
    gifGhost[2] = new QMovie(":/source/img/Ghost3.gif");
    gifGhost[3] = new QMovie(":/source/img/Ghost4.gif");

    mediaBgmLoop->addMedia(QUrl("qrc:/source/media/bgm.mp3"));
    mediaBgmLoop->setPlaybackMode(QMediaPlaylist::Loop);
    mediaBgm->setPlaylist(mediaBgmLoop);
    mediaBgm->setVolume(30);
    mediaBgm->playbackRateChanged(qreal(1.25));
    mediaBgm->play();
    mediaEaten->setMedia(QUrl("qrc:/source/media/eaten.mp3"));
    mediaEaten->setVolume(30);
    mediaEat->setMedia(QUrl("qrc:/source/media/eat.mp3"));
    mediaEat->setVolume(30);
    mediaOver->setMedia(QUrl("qrc:/source/media/gameover.mp3"));
    mediaOver->setVolume(30);
    mediaPower->setMedia(QUrl("qrc:/source/media/power.mp3"));
    mediaPower->setVolume(30);
    mediaWin->setMedia(QUrl("qrc:/source/media/win.mp3"));
    mediaWin->setVolume(30);

    ui->b_stop->setFocusPolicy(Qt::NoFocus);
    ui->HiFps->setFocusPolicy(Qt::NoFocus);
    ui->b_pause->setFocusPolicy(Qt::StrongFocus);
    ui->b_pause->setChecked(true);
    ui->b_stop->setChecked(false);
    ui->b_stop->setCheckable(false);
    ui->b_pause->setCheckable(true);

    labelPlayer->setGeometry(270, 460, 20, 20);
    labelPlayer->setFixedSize(20, 20);
    for (int i = 0; i < 4; i++)
    {
        gifPlayer[i]->setScaledSize(QSize(20, 20));
        gifPlayer[i]->start();
        gifGhost[i]->setScaledSize(QSize(20, 20));
        gifGhost[i]->start();
    }
    gifScared->setScaledSize(QSize(20, 20));
    gifShine->setScaledSize(QSize(20, 20));
    gifEye->setScaledSize(QSize(20, 20));
    gifScared->start();
    gifShine->start();
    gifEye->start();

    connect(scaredTimer1, SIGNAL(timeout()), this, SLOT(scaredSlot1()));
    connect(scaredTimer2, SIGNAL(timeout()), this, SLOT(scaredSlot2()));
    connect(gameProcTimer, SIGNAL(timeout()), this, SLOT(gameProc()));
    connect(updateTimer, SIGNAL(timeout()), this, SLOT(repaint()));
    //update->start(16);
}

MainWindow::~MainWindow()
{
    //回收变量空间
    for (int i = 0; i < 4; i++)
    {
        delete gifGhost[i];
        delete gifPlayer[i];
        delete labelGhost[i];
    }
    delete labelPlayer;
    delete gifScared;
    delete gifShine;
    delete gifEye;
    delete mediaWin;
    delete mediaOver;
    delete mediaEaten;
    delete mediaEat;
    delete mediaPower;
    delete mediaBgmLoop;
    delete mediaBgm;
    delete scaredTimer1;
    delete scaredTimer2;
    delete updateTimer;
    delete ui;
}

void MainWindow::gameProc()
{
    //qDebug() << "gameProc";
    //qDebug() << ghost[0].step << " " << ghost[1].step << " " << ghost[2].step << " " << ghost[3].step << " ";
    for (int i = 0; i < 4; i++) //处理转向
    {
        if (statePause == 0 && ghost[i].step == 0 && (ghost[i].x != pacman.x || ghost[i].y != pacman.y))
        {
            ghost[i].goNextStep(pacman);
        }
    }
    for (int i = 0; i < 4; i++)
    {
        playerMoveEvent(ghost[i], 0);
        getScreenPosition(ghost[i].x, ghost[i].y, ghost[i].step, ghost[i].drt, &gX[i], &gY[i], 0);
        if (ghost[i].scaredState == 0) // 处理 正常态 惊吓态 闪烁态 被吃态 gif
        {
            labelGhost[i]->setMovie(gifGhost[i]);
        }
        else if (ghost[i].scaredState == 1)
        {
            labelGhost[i]->setMovie(gifScared);
        }
        else if (ghost[i].scaredState == 2)
        {
            labelGhost[i]->setMovie(gifShine);
        }
        if (ghost[i].eaten == true)
        {
            labelGhost[i]->setMovie(gifEye);
        }
        labelGhost[i]->setGeometry(gX[i], gY[i], 20, 20);
    }
    getScreenPosition(pacman.x, pacman.y, pacman.step, pacman.drt, &px, &py, 1);
    labelPlayer->setGeometry(px, py, 20, 20);
    labelPlayer->setMovie(gifPlayer[pacman.drt]);
    playerMoveEvent(pacman, 1);
    //update();
}

void MainWindow::initGame()
{
    this->setStyleSheet("#MainWindow{background-image: url(:/source/img/map-logo.png);}");
    ui->b_stop->setVisible(true);
    ui->b_pause->setVisible(true);
    ui->HiFps->setVisible(true);
    if (ui->HiFps->checkState())
    {
        updateTimer->start(8);
    }
    else
    {
        updateTimer->start(30);
    }
    stateHealth = 4;
    globalScaredState = 0;
    scorePac = 0;
    scoreGhost = 0;

    lowStep = -4;
    highStep = 5;
    mulStep = 2;
    updateTime = 50;

    pacman.Player::init(23, 14);
    ghost[0].init(14, 11);
    ghost[1].init(14, 12);
    ghost[2].init(14, 15);
    ghost[3].init(14, 16);
    bool ok = false;
    statePause = 1;
    for (int i = 0; i < 31; i++)
        for (int j = 0; j < 28; j++)
        {
            map[i][j] = backupMap[i][j];
        }
    while (level = QInputDialog::getInt(this, "Pacman Project", "加权总分 = (得分 + 剩余生命数 * 15) * 难度系数\n难度               难度系数\n  1		：	  1.0\n  2		：	  1.2\n  3		：	  1.5\n请输入难度：", 1, 1, 3, 1, &ok, (Qt::WindowCloseButtonHint | Qt::MSWindowsFixedSizeDialogHint)))
    {
        if (ok)
        {
            statePause = 0;
            break;
        }
    }

    switch (level)
    {
    case 1: //慢速，两倍
        lowStep = -4;
        highStep = 5;
        mulStep = 2;
        acStep = 3;      // 8
        updateTime = 15; //20
        break;
    case 2: //中速，1.5倍
        lowStep = -6;
        highStep = 6;
        mulStep = 1.5;
        acStep = 3;      //5
        updateTime = 12; //15
        break;
    case 3: //高速，1.25倍
        lowStep = -7;
        highStep = 8;
        mulStep = 1.25;
        acStep = 3;
        updateTime = 9; //10
    }
    labelPlayer->setGeometry(270, 460, 20, 20);
    labelPlayer->setMovie(gifGhost[0]);
    labelPlayer->show();

    for (int i = 0; i < 4; i++)
    {
        labelGhost[i]->setGeometry(ghost[i].y * 20, ghost[i].x * 20, 20, 20);
        labelGhost[i]->setMovie(gifGhost[i]);
        labelGhost[i]->show();
    }

    gameProcTimer->start(updateTime);
    //qDebug() << "start " << updateTime;
    //update();
}

void MainWindow::playerMoveEvent(Player &p, int identity)
{ //用于常规步进,identity表明对象身份，1为玩家，0为鬼
    if (statePause != 0)
    { //游戏未运行，不予判断
        return;
    }
    if (p.x == pacman.x && p.y == pacman.y && identity == 0)
    {
        if (p.scaredState)
        { //鬼被吃掉
            if (!p.eaten)
            {
                scoreGhost += 10;
                p.eaten = true;
                mediaEat->stop();
                mediaEat->play();
            }
        }
        else
        { //玩家扣血
            if (!p.eaten)
            {
                stateHealth--;
                if (stateHealth < 1)
                {
                    statePause = 2;
                }
                mediaEaten->play();
                //初始化全员信息
                pacman.Player::init(23, 14);
                ghost[0].Player::init(14, 11);
                ghost[1].Player::init(14, 12);
                ghost[2].Player::init(14, 15);
                ghost[3].Player::init(14, 16);
                scaredTimer1->stop();
                scaredTimer2->stop();
                scaredSlot2();
            }
        }
    }

    if (p.step == 0)
    {
        //判断事件触发
        if (identity == 1)
        { //玩家专属事件
            if (map[p.x][p.y] == 3)
            { //吃到了大力
                if (scaredTimer1->remainingTime() != -1 || scaredTimer2->remainingTime() != -1)
                {
                    scaredTimer1->stop();
                    scaredTimer2->stop();
                    updateTime += acStep;
                }
                map[p.x][p.y] = 4;
                mediaPower->play();
                scorePac += 1;
                globalScaredState = 1;
                for (int i = 0; i < 4; i++)
                {
                    ghost[i].scaredState = 1;
                }
                scaredTimer1->stop();
                scaredTimer2->stop();
                scaredTimer1->start(7000);
                updateTime -= acStep;
                gameProcTimer->stop();
                gameProcTimer->start(updateTime);
            }
            if (map[p.x][p.y] == 0)
            { //吃到了豆子
                map[p.x][p.y] = 4;
                scorePac += 1;
            }
        }
        else
        { //鬼专属事件

            if (map[p.x][p.y] == 2)

            { //鬼碰到鬼屋
                p.eaten = false;
                if (level == 3)
                {
                    p.scaredState = 0;
                }
            }
        }

        //判断撞墙
        switch (p.drt)
        { //玩家/鬼碰到墙
        case 0:
            if (map[p.x][p.y - 1] == 1)
            {
                return;
            }
            break;
        case 1:
            if (map[p.x][p.y + 1] == 1)
            {
                return;
            }
            break;
        case 2:
            if (map[p.x - 1][p.y] == 1)
            {
                return;
            }
            break;
        case 3:
            if (map[p.x + 1][p.y] == 1)
            {
                return;
            }
            break;
        default:
            break;
        }
        if (identity == 1)
        {
            //玩家碰到了鬼屋/门
            switch (p.drt)
            {
            case 0:
                if (map[p.x][p.y - 1] == 2 || map[p.x][p.y - 1] == 5)
                {
                    return;
                }
                break;
            case 1:
                if (map[p.x][p.y + 1] == 2 || map[p.x][p.y + 1] == 5)
                {
                    return;
                }
                break;
            case 2:
                if (map[p.x - 1][p.y] == 2 || map[p.x - 1][p.y] == 5)
                {
                    return;
                }
                break;
            case 3:
                if (map[p.x + 1][p.y] == 2 || map[p.x + 1][p.y] == 5)
                {
                    return;
                }
                break;
            default:
                break;
            }
        }
    }

    //撞墙+事件触发判断结束，开始步进
    if (p.drt != 5)
    {
        p.step++;
    }
    switch (p.drt)
    { // 上 0 下 1 左 2 右 3
    case 0:
        if (p.step > 10 && identity == 0)
        {
            p.y--;
            p.step = -9;
        }
        else if (p.step > highStep && identity == 1)
        {
            p.y--;
            p.step = lowStep;
        }
        break;
    case 1:
        if (p.step > 10 && identity == 0)
        {
            p.y++;
            p.step = -9;
        }
        else if (p.step > highStep && identity == 1)
        {
            p.y++;
            p.step = lowStep;
        }
        break;
    case 2:
        if (p.step > 10 && identity == 0)
        {
            p.x--;
            p.step = -9;
        }
        else if (p.step > highStep && identity == 1)
        {
            p.x--;
            p.step = lowStep;
        }
        break;
    case 3:
        if (p.step > 10 && identity == 0)
        {
            p.x++;
            p.step = -9;
        }
        else if (p.step > highStep && identity == 1)
        {
            p.x++;
            p.step = lowStep;
        }
        break;
    default:
        break;
    }
    //步进结束
}

void MainWindow::paintEvent(QPaintEvent *)
{ //绘制GUI

    if (statePause == 4 || statePause == -1)
    {
        return;
    }
    if (statePause != 0)
    {
        if (statePause == 2)
        {
            statePause = 4;
            mediaOver->play();
            totalScore = stateHealth * 15 + scorePac + scoreGhost;
            if (level == 2)
            {
                totalScore *= 1.2;
            }
            else if (level == 3)
            {
                totalScore *= 1.5;
            }
            http h;
            QString topRank = h.post("Dead", 0);
            QMessageBox::about(this, "Pacman Project", "光荣战败\n分数：" + QString::number(scorePac + scoreGhost) + "\t剩余生命：" + QString::number(stateHealth) + "\n加权总分：" + QString::number(totalScore) + "\n在线TOP5：\n" + topRank);
            return;
        }
        if (statePause == 3)
        {
            statePause = 4;
            mediaWin->play();
            totalScore = stateHealth * 15 + scorePac + scoreGhost;
            if (level == 2)
            {
                totalScore *= 1.2;
            }
            else if (level == 3)
            {
                totalScore *= 1.5;
            }
            http h;
            bool ok = false;
            QString name = QInputDialog::getText(this, "Pacman Project", "恭喜您取得了胜利！您可以留下姓名上传成绩！", QLineEdit::Normal, "name", &ok);

            if (ok)
            {
                QString topRank = h.post(name, totalScore);
                QMessageBox::about(this, "Pacman Project", "取得胜利\n分数：" + QString::number(scorePac + scoreGhost) + "\t剩余生命：" + QString::number(stateHealth) + "\n加权总分：" + QString::number(totalScore) + "\n在线TOP5：\n" + topRank);
            }
            else
            {
                QMessageBox::about(this, "Pacman Project", "取得胜利\n分数：" + QString::number(scorePac + scoreGhost) + "\t剩余生命：" + QString::number(stateHealth) + "\n加权总分：" + QString::number(totalScore));
            }
            return;
        }
    }
    QPainter p(this);
    p.setBrush(QBrush(Qt::white, Qt::SolidPattern));
    p.setFont(QFont("微软雅黑", 20, 700, false));
    p.setPen(Qt::white);

    for (int i = 0; i < 31; i++)
        for (int j = 0; j < 28; j++)
        {
            if (map[i][j] == 0)
            {
                p.drawEllipse(5 + j * 20, 5 + i * 20, 5, 5);
            }
            if (map[i][j] == 3)
            {
                p.drawEllipse(3 + j * 20, 3 + i * 20, 10, 10);
            }
        }
    p.drawText(QPoint(600, 150), "Score:" + QString::number(scorePac + scoreGhost));
    p.drawText(QPoint(600, 190), "Health:" + QString::number(stateHealth));

    if (scorePac == 298)
    {
        statePause = 3;
    }
    /*
    //输出调试信息===================================================
    p.setFont(QFont("微软雅黑", 10, 700, false));
    p.drawText(QPoint(570, 360), "updateTime:" + QString::number(updateTime));
    p.drawText(QPoint(570, 380), "statePause:" + QString::number(statePause) + " scaredState:" + QString::number(globalScaredState));
    p.drawText(QPoint(570, 400), "P x:" + QString::number(pacman.x) + " y:" + QString::number(pacman.y) + " step：" + QString::number(pacman.step) + " drt：" + QString::number(pacman.drt));
    p.drawText(QPoint(570, 420), "G1 x:" + QString::number(ghost[0].x) + " y:" + QString::number(ghost[0].y) + " step：" + QString::number(ghost[0].step) + " drt：" + QString::number(ghost[0].drt));
    p.drawText(QPoint(570, 440), "G2 x:" + QString::number(ghost[1].x) + " y:" + QString::number(ghost[1].y) + " step：" + QString::number(ghost[1].step) + " drt：" + QString::number(ghost[1].drt));
    p.drawText(QPoint(570, 460), "G3 x:" + QString::number(ghost[2].x) + " y:" + QString::number(ghost[2].y) + " step：" + QString::number(ghost[2].step) + " drt：" + QString::number(ghost[2].drt));
    p.drawText(QPoint(570, 480), "G4 x:" + QString::number(ghost[3].x) + " y:" + QString::number(ghost[3].y) + " step：" + QString::number(ghost[3].step) + " drt：" + QString::number(ghost[3].drt));
*/
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
    case Qt::Key_W:
        if (pacman.drt == 2)
        {
            return;
        }
        if (map[pacman.x - 1][pacman.y] != 1 && map[pacman.x - 1][pacman.y] != 5)
        {
            pacman.drt = 2;
            pacman.step = 0;
            pacman.drt = 2;
        }
        else if (map[pacman.x - 1][pacman.y - 1] != 1 && pacman.drt == 0 && pacman.step >= 5)
        {
            pacman.y--;
            pacman.drt = 2;
            pacman.step = 0;
        }
        else if (map[pacman.x - 1][pacman.y + 1] != 1 && pacman.drt == 0 && pacman.step <= -2)
        {
            pacman.y++;
            pacman.drt = 2;
            pacman.step = 0;
        }
        else if (map[pacman.x - 1][pacman.y + 1] != 1 && pacman.drt == 1 && pacman.step >= 5)
        {
            pacman.y++;
            pacman.drt = 2;
            pacman.step = 0;
        }
        else if (map[pacman.x - 1][pacman.y - 1] != 1 && pacman.drt == 1 && pacman.step <= -2)
        {
            pacman.y--;
            pacman.drt = 2;
            pacman.step = 0;
        }

        break;

    case Qt::Key_S:
        if (pacman.drt == 3)
        {
            return;
        }
        if (map[pacman.x + 1][pacman.y] != 1 && map[pacman.x + 1][pacman.y] != 5)
        {
            pacman.drt = 3;
            pacman.step = 0;
            pacman.drt = 3;
        }
        else if (map[pacman.x + 1][pacman.y - 1] != 1 && pacman.drt == 0 && pacman.step >= 5)
        {
            pacman.y--;
            pacman.drt = 3;
            pacman.step = 0;
        }
        else if (map[pacman.x + 1][pacman.y + 1] != 1 && pacman.drt == 0 && pacman.step <= -2)
        {
            pacman.y++;
            pacman.drt = 3;
            pacman.step = 0;
        }
        else if (map[pacman.x + 1][pacman.y + 1] != 1 && pacman.drt == 1 && pacman.step >= 5)
        {
            pacman.y++;
            pacman.drt = 3;
            pacman.step = 0;
        }
        else if (map[pacman.x + 1][pacman.y - 1] != 1 && pacman.drt == 1 && pacman.step <= -2)
        {
            pacman.y--;
            pacman.drt = 3;
            pacman.step = 0;
        }

        break;

    case Qt::Key_A:
        if (pacman.drt == 0)
        {
            return;
        }
        if (map[pacman.x][pacman.y - 1] != 1 && map[pacman.x][pacman.y - 1] != 5)
        {
            pacman.drt = 0;
            pacman.step = 0;
            pacman.drt = 0;
        }
        else if (map[pacman.x + 1][pacman.y - 1] != 1 && pacman.drt == 3 && pacman.step >= 5)
        {
            pacman.x++;
            pacman.drt = 0;
            pacman.step = 0;
        }
        else if (map[pacman.x - 1][pacman.y - 1] != 1 && pacman.drt == 3 && pacman.step <= -2)
        {
            pacman.x--;
            pacman.drt = 0;
            pacman.step = 0;
        }
        else if (map[pacman.x - 1][pacman.y - 1] != 1 && pacman.drt == 2 && pacman.step >= 5)
        {
            pacman.x--;
            pacman.drt = 0;
            pacman.step = 0;
        }
        else if (map[pacman.x + 1][pacman.y - 1] != 1 && pacman.drt == 2 && pacman.step <= -2)
        {
            pacman.x++;
            pacman.drt = 0;
            pacman.step = 0;
        }

        break;

    case Qt::Key_D:
        if (pacman.drt == 1)
        {
            return;
        }
        if (map[pacman.x][pacman.y + 1] != 1 && map[pacman.x][pacman.y + 1] != 5)
        {
            pacman.drt = 1;
            pacman.step = 0;
            pacman.drt = 1;
        }
        else if (map[pacman.x + 1][pacman.y + 1] != 1 && pacman.drt == 3 && pacman.step >= 5)
        {
            pacman.x++;
            pacman.drt = 1;
            pacman.step = 0;
        }
        else if (map[pacman.x - 1][pacman.y + 1] != 1 && pacman.drt == 3 && pacman.step <= -2)
        {
            pacman.x--;
            pacman.drt = 1;
            pacman.step = 0;
        }
        else if (map[pacman.x - 1][pacman.y + 1] != 1 && pacman.drt == 2 && pacman.step >= 5)
        {
            pacman.x--;
            pacman.drt = 1;
            pacman.step = 0;
        }
        else if (map[pacman.x + 1][pacman.y + 1] != 1 && pacman.drt == 2 && pacman.step <= -2)
        {
            pacman.x++;
            pacman.drt = 1;
            pacman.step = 0;
        }
        break;
    case Qt::Key_Space:
        on_b_pause_clicked(false);

        break;
        //Ghost1 人为接管Ghost1以便于调试==========================================================
        /*
    case Qt::Key_I:
        if (map[ghost[0].x - 1][ghost[0].y] != 1)
        {
            //if (pacman.drt!=3&&(pacman.step<9&&pacman.step>-9)) return;
            ghost[0].drt = 2;
            ghost[0].step = 0;
        }
        break;

    case Qt::Key_K:
        if (map[ghost[0].x + 1][ghost[0].y] != 1)
        {
            //if (pacman.drt!=2&&(pacman.step<9&&pacman.step>-9)) return;
            ghost[0].drt = 3;
            ghost[0].step = 0;
        }
        break;

    case Qt::Key_J:
        if (map[ghost[0].x][ghost[0].y - 1] != 1)
        {
            //if (pacman.drt!=1&&(pacman.step<9&&pacman.step>-9)) return;
            ghost[0].drt = 0;
            ghost[0].step = 0;
        }
        break;

    case Qt::Key_L:
        if (map[ghost[0].x][ghost[0].y + 1] != 1)
        {
            //if (pacman.drt!=0&&(pacman.step<9&&pacman.step>-9)) return;
            ghost[0].drt = 1;
            ghost[0].step = 0;
        }
        break;
    case Qt::Key_O:
        //ghostGoNextStep(&ghost[0], pacman);
        ghost[0].step = 0;
        break;
    */
    default:
        return;
    }
}

void MainWindow::on_b_pause_clicked(bool checked)
{
    if (statePause == -1)
    {
        initGame();
        statePause = 0;
        mediaBgm->stop();
        mediaBgm->play();
        return;
    }
    if (statePause == 0) // 游戏状态->暂停状态
    {
        statePause = 1;
        mediaBgm->pause();
        ui->b_pause->setStyleSheet("border-image: url(:/source/img/play.png)");
        gameProcTimer->stop();
        if (globalScaredState != 0)
        {
            remainTime1 = scaredTimer1->remainingTime();
            remainTime2 = scaredTimer2->remainingTime();
            scaredTimer1->stop();
            scaredTimer2->stop();
        }
    }
    else if (statePause == 1) // 暂停状态->游戏状态
    {
        statePause = 0;
        mediaBgm->play();
        gameProcTimer->start(updateTime);
        ui->b_pause->setStyleSheet("border-image: url(:/source/img/pause.png)");
        if (remainTime1 != -1)
        {
            scaredTimer1->start(remainTime1);
            remainTime1 = -1;
        }
        if (remainTime2 != -1)
        {
            scaredTimer2->start(remainTime2);
            remainTime2 = -1;
        }
    }
    return;
}

void MainWindow::on_b_stop_clicked(bool checked)
{
    mediaBgm->stop();
    mediaBgm->play();
    initGame();
    statePause = 0;
    scaredTimer1->stop();
    scaredTimer2->stop();
    remainTime1 = -1;
    remainTime2 = -1;
    return;
}

void MainWindow::getScreenPosition(int x, int y, int step, int drt, int *outx, int *outy, int identity)
{ //将对象坐标信息转换为屏幕像素信息
    // 上 0 下 1 左 2 右 3
    // 左 0 右 1 上 2 下 3

    int tx, ty;
    tx = y * 20;
    ty = x * 20;
    switch (drt)
    {
    case 0:
        if (!identity)
        {
            tx -= step;
        }
        else
        {
            tx -= step * mulStep;
        }
        break;
    case 1:
        if (!identity)
        {
            tx += step;
        }
        else
        {
            tx += step * mulStep;
        }
        break;
    case 2:
        if (!identity)
        {
            ty -= step;
        }
        else
        {
            ty -= step * mulStep;
        }
        break;
    case 3:
        if (!identity)
        {
            ty += step;
        }
        else
        {
            ty += step * mulStep;
        }
        break;
    default:
        break;
    }
    *outx = tx;
    *outy = ty;
}

void MainWindow::scaredSlot1()
{

    scaredTimer1->stop();
    globalScaredState = 2;
    for (int i = 0; i < 4; i++)
    {
        ghost[i].scaredState <<= 1;
    }
    scaredTimer2->start(3000);
}

void MainWindow::scaredSlot2()
{
    scaredTimer2->stop();

    switch (level)
    {
    case 1:
        updateTime = 15;
        break;
    case 2:
        updateTime = 12;
        break;
    case 3:
        updateTime = 9;
        break;
    }
    gameProcTimer->stop();
    gameProcTimer->start(updateTime);
    globalScaredState = 0;
    for (int i = 0; i < 4; i++)
    {
        ghost[i].scaredState = 0;
    }
}

void MainWindow::on_HiFps_stateChanged(int arg1)
{
    if (ui->HiFps->checkState())
    {
        updateTimer->stop();
        updateTimer->start(8);
    }
    else
    {
        updateTimer->stop();
        updateTimer->start(30);
    }
}
