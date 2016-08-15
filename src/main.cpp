#include "shermanmainwin.h"
#include <QApplication>

#include <iostream>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ShermanMainWin w;
    w.show();
    return a.exec();
}
