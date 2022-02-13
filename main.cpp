#include "mainwindow.h"

#include <QApplication>
#include "openssl/ssl.h"
#include "depot.h"


int main(int argc, char *argv[])
{
    SSL_library_init();
    SSLeay_add_ssl_algorithms();
    SSL_load_error_strings();

    QApplication a(argc, argv);
    iDateToday = QDate::currentDate();

    Depot depot("depot.dpt");


    //MainWindow w;
    //w.show();
    //return a.exec();
    return 0;
}
