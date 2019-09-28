#include "mainwindow.h"
#include "addrecipewindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
       MainWindow w;
       w.show();
//    AddRecipeWindow w;
//    w.show();

    return a.exec();
}
