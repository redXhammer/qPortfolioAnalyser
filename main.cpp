#include "mainwindow.h"

#include <QApplication>
#include <QDebug>
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
    sUrlHistEig += "/historische_kurse";
    hSite.open(sUrlHistEig);

    //hSite.PrintChildNodes();
    hSite.Search("boerse_id");


    hSite.cdTotalDir("/html/body/div1/div2/div9/div/div2/div2/form/ul/div1/li");
    hSite.PrintChildNodes(*hSite.hCurrentNode,true,0);*/



    /*fond* fd = fsAllFonds.GetFondClass("A2H9Q0");

    QDate ersterKurs = fd->dqKUD.back().cDatum;
    double dErsterKurs = fd->GetCurrentWert(ersterKurs);
    double dErsterKursB = fd->GetTotalWert(ersterKurs);
    double dLetzterKurs = fd->GetCurrentWert(iDateToday);
    double dLetzterKursB = fd->GetTotalWert(iDateToday);

    qInfo() << "Datum erster Kurs:" << ersterKurs;
    qInfo() << "Erster Kurs" << dErsterKurs << "(Bereinigt:" << dErsterKursB << ")";
    qInfo() << "Letzter Kurs" << dLetzterKurs << "(Bereinigt:" << dLetzterKursB << ")";
    qInfo() << "Performance:" << dLetzterKursB / dErsterKursB;*/



    Depot depot("depot.dpt");

    QStringList depOverview = depot.ShowDepotGesamtWert(iDateToday).split("\n");
    foreach (QString depLine, depOverview) {
        qInfo().noquote() << depLine;
    }


    //MainWindow w;
    //w.show();
    //return a.exec();
    return 0;
}
