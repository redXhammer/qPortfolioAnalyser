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

    //html.open("file://testdata");
    /*html hSite;
    QString sUrlHistEig = "https://www.ariva.de";
    sUrlHistEig += "/msci_world_index_ucits_etf_dr_-_1d-fonds";
    sUrlHistEig += "/ausschuettungen/";
    hSite.open(sUrlHistEig);

    //hSite.PrintChildNodes();
    hSite.Search("13.05.20");

    hSite.cdTotalDir("/html/body/div1/div2/div9/div/div2/div5/div/table");*/

    Depot depot("depot.dpt");


    //MainWindow w;
    //w.show();
    //return a.exec();
    return 0;
}
