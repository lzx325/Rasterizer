#include "mainwindow.h"

#include <QApplication>
#include <armadillo>
#include <QObject>
#include <QDebug>
#include <iostream>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();

}
