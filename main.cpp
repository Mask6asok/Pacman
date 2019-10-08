#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowTitle("Pacman-by Mask Cno NJUST 2019");
    w.setWindowIcon(QIcon(":/source/img/logo.png"));
    w.setFixedSize(760, 620);
    w.show();

    return a.exec();
}
