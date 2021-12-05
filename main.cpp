#include "mainwindow.h"

#include <QApplication>
#include "fond.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    iDateToday = QDate::currentDate();
    MainWindow w;
    w.show();
    return a.exec();
}
