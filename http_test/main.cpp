
#include <openssl/ssl.h>
#include <QApplication>
#include <QDebug>
#include "../html.h"


int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    SSL_library_init();
    SSLeay_add_ssl_algorithms();
    SSL_load_error_strings();

    //QString cAddress = "/aktien/aktien_suche.asp";
    QString cPostData;
    QTextStream ssOut(&cPostData);

    /*ssOut << "intpagenr=12&"
          << "stName=&"
          << "inIndex=0&"
          << "inBranche=0&"
          << "inLand=61&"
          << "inFundamental1=0&"
          << "inFundamentalJahr1=2021&"
          << "inFunndaGrKl1=2&"
          << "stFundaZahl1=&"
          << "inFundamental2=0&"
          << "inFundamentalJahr2=2021&"
          << "inFunndaGrKl2=2&"
          << "stFundaZahl2=&"
          << "inBillanz=0&"
          << "inBillanzJahr=2021&"
          << "inBillanzGrKl=2&"
          << "stBillantZahl=&"
          << "inSonstige=0&"
          << "inSonstigeGrKl=2&"
          << "stSonstigZahl=";*/

    ssOut <<  "https://www.finanzen.net"
              "/aktien/aktien_suche.asp?"
              "intpagenr=53&"
              "inland=0&"
              "inbillanz=0&"
              "inbillanzjahr=2021&"
              "inbillanzgrkl=2&"
              "inbranche=0&"
              "inindex=0&"
              "infunndagrkl1=2&"
              "infunndagrkl2=2&"
              "infundamental1=0&"
              "infundamentaljahr1=2021&"
              "infundamental2=0&"
              "infundamentaljahr2=2021&"
              "insonstige=0&"
              "insonstigegrkl=2";

    html cHttp;
    //cHttp.SetAddr ( "www.finanzen.net" ) ;

    if (cHttp.open(cPostData) != 0)
    {
        qWarning() << "Error getting " << cPostData;
        return -1;
    }

    qInfo() << "Top Nodes:";
    cHttp.List();

    //qInfo() <<

    QTextStream tsin (stdin);

    char c;
    QString data;
    bool quit = false;

    while (!quit)
    {
      tsin >> c;
      switch (c)
      {
      case 'c': // cd total path
        qInfo() << "cd:";
        tsin >> data;
        cHttp.cdTotalDir(data);
        break;
      case 's': //search
        qInfo() << "Search:";
        tsin >> data;
        cHttp.Search(data);
        break;
      case 'p':
        cHttp.PrintChildNodes();
        break;
      case 'q':
        qInfo() << "quit";
        quit = true;
        break;
      }
    }

    return 0;
}

