#define FondFile
#include "fond.h"

#include <QTextStream>

#define MAXBUFSIZE 100000
#define MAXBUFCOUNT MAXBUFSIZE / 13
#define FDMAX 70
//#define LOWOUTPUT
//#define NOOUTPUT
#define AKTUALISIEREN 0
#include <QDebug>

typedef unsigned long DWORD;

QDate fond::DateOfWkn()
{
    return QDate::fromString(WknFile.GetData("Fond","LastUpdate"),"dd.MM.yyyy");
}

QString fond::GetWknFileName()
{
    return sWkn;
}

fond::fond(const QString &sWkn2) /***************** Konstructor der Fond-Klasse mit übergabe der WKN **************/
{
    iKUD = 0;
    sWknFileName = "fonddaten/";

    sWkn = sWkn2;
    sWknFileName += sWkn;
    sWknFileName += ".fdd";
    WknFile.open( sWknFileName ); // Öffnen der WKN file
#ifndef NOOUTPUT
    qInfo() << "Datum der Fonddatei" << DateOfWkn();
#endif
    iStatus = 1;
    return;
}

bool fond::LoadAllData()
{
    cSecu = WknFile.GetData("Fond","SECU");
    STsURL = WknFile.GetData ("Fond","URL") ;
    STsName = WknFile.GetData ("Fond","NAME") ;


    qInfo() << STsName << "Loaded Secu:" << cSecu << "Url:" << STsURL;

    if (cSecu.length() == 0 || STsURL.length() == 0 || STsName.length() == 0 )
    {
        if (GetSecu(sWkn,cSecu,STsURL,STsName) == false)
        {
            qInfo() << "Secu Error";
            iStatus = 2;
            return -1;
        }
        else
        {
            WknFile.AddData ("Fond","SECU",cSecu  );
            WknFile.AddData ("Fond","URL", STsURL );
            WknFile.AddData ("Fond","NAME",STsName);
        }
    }

    int newKurse;
    if ((newKurse = LoadFondKurse()) < 1)
    {
        WknFile.DropData();
        return false;
    }

    LoadFondAuss();

    if (newKurse > 0)
    {
#ifndef NOOUTPUT
        qInfo() << "Speichern : " << SaveFondData() << " Einträge";
#else
        SaveFondData();
#endif
    }

    return true;
}

int fond::LoadFondKurse()
{
    KursUndDatum kudHelp;

    QList<KursUndDatum> vKurse = WknFile.GetVect("Fond","Kurse");
    if (vKurse.size() == 0)
    {
        qInfo() << "No old kurse";
        iKUD = 0;
        iStatus = 0;
        return DownloadFondData();
    }

    kudHelp = vKurse.at(0);
    int iNumNewKurse = 0;

    QDate dateOfData = DateOfWkn();
    if (dateOfData.isNull() || (dateOfData < iDateToday) || AKTUALISIEREN) /************** Beginn des Aktualisierungsprozess *************/
    {
#ifndef NOOUTPUT
        qInfo() << "Datum des letzen lokalen Fondeintrags: " << kudHelp.cDatum;
        qInfo() << "Aktualisiere ... ";
#endif

        iNumNewKurse = DownloadFondData(kudHelp.cDatum);

#ifndef NOOUTPUT
        qInfo() << "Fertig";
#endif


        dqKUD.append(vKurse);


    } else {
        dqKUD.append(vKurse);
#ifndef NOOUTPUT
        qInfo().noquote() << "Newest:" << kudHelp.cDatum.toString("d.M.yyyy") << kudHelp.iKurs
                          << "Oldest:" << dqKUD.back().cDatum.toString("d.M.yyyy") << dqKUD.back().iKurs;
#endif
    }

    iKUD = 0;
    iStatus = 0;
    return iNumNewKurse;
}

int fond::LoadFondAuss()
{
    QDate dateOfData = DateOfWkn();
    if (dateOfData.isNull() || (dateOfData < iDateToday))
    {
        dqAusschuettung = DownloadFondAuss();
    }
    else
    {
        dqAusschuettung = WknFile.GetVect("Fond","Auss");
    }

    itAussHigh = dqAusschuettung.begin();
    itAussLow = itAussHigh;
    itAussHigh++;

    return 0;
}

QList<KursUndDatum> fond::DownloadFondAuss()
{
    QList<KursUndDatum> dqAuss;
    html hSite;
    QString sUrlHistEig = "https://www.ariva.de";
    sUrlHistEig += STsURL;
    sUrlHistEig += "/ausschuettungen/";
    hSite.open(sUrlHistEig);

    if (!hSite.cdTotalDir("/html/body/div1/div2/div9/div/div2/div5/div/table"))
    {
        qInfo() << "===============PrintChildNodes==================";
        hSite.PrintChildNodes();
        qInfo() << "=======End=====PrintChildNodes==================";
        return dqAuss;
    }

    //hSite.PrintChildNodes(*hSite.hCurrentNode,true,0);

    QVector<HtmlNode*> vhnTR = hSite.hCurrentNode->GetElementsByName("tr");

    for (int iElem = 1; iElem < vhnTR.size(); iElem++)
    {
        /*
         * tr
         *  |-td
         *  | |-16.11.21
         *  |-td
         *  | |-Ausschüttung
         *  |-td
         *  | |-&nbsp;
         *  |-td
         *  | |-1,2 EUR
         *  |-td
         * -tr
         * tr
         *  |-td
         *  | |-25.11.20
         * ...
         **/

        HtmlNode* hnNode = vhnTR[iElem];
        QVector<HtmlNode*> listTd = hnNode->GetElementsByName("td");

        if (listTd.size() < 4) continue;

        QString strDate = listTd[0]->vectHtmlChildNode[0]->sName;
        QString strType = listTd[1]->vectHtmlChildNode[0]->sName;


        //qInfo() << strType;

        if (QString("Dividende") == strType || QString("Ausschüttung") == strType)
        {
            if (listTd[3]->vectHtmlChildNode.size() == 0) continue;
            QString strBetrag = listTd[3]->vectHtmlChildNode[0]->sName;

            KursUndDatum kudAuss;
            strBetrag = strBetrag.remove(" EUR");
            int iPosRev = strBetrag.lastIndexOf(',');

            double dAussWert = strBetrag.replace(iPosRev,1,".").toDouble(/*&bOK*/) ;

            kudAuss.cDatum = QDate::fromString(strDate,"dd.MM.yy");
            if (kudAuss.cDatum.year() < 1980)
                kudAuss.cDatum = kudAuss.cDatum.addYears(100);


            double dCurrWert = GetCurrentWert(kudAuss.cDatum);


            kudAuss.iKurs = 1;
            kudAuss.iKurs += dAussWert/GetCurrentWert(kudAuss.cDatum);

            qInfo() << "Divi am " << strDate
                    << " mit "    << strBetrag
                    << " Euro ("  << dAussWert<< "," << dCurrWert
                    << ") ("      << kudAuss.iKurs << ")";

            if (dCurrWert == 0) continue;

            if (dqAuss.size()>0) {
                kudAuss.iKurs *= (dqAuss.back().iKurs);
            }

            dqAuss.push_back(kudAuss);
        }

        else if (QString("Split") == strType)
        {
            KursUndDatum kudAuss;

            QString sVerh = listTd[2]->vectHtmlChildNode[0]->sName;
            int iPos = sVerh.indexOf(":");

            kudAuss.cDatum = QDate::fromString(strDate,"dd.MM.yy");

            //kudAuss.iKurs = 0;
            kudAuss.iKurs = sVerh.mid(iPos+1).toDouble(/*&bOK*/) / sVerh.mid(0,iPos).toDouble(/*&bOK*/);

            if (dqAuss.size()>0) {
                kudAuss.iKurs *= (dqAuss.back().iKurs);
            }

            qInfo() << "Split am "        << strDate
                    << " mit Verhältnis " << listTd[2]->vectHtmlChildNode[0]->sName
                    << " ("               << kudAuss.iKurs << ")";

            dqAuss.push_back(kudAuss);
        }
    }

    return dqAuss;
}




int fond::SaveFondData()
{
    int iSaveCount = 0;

    //Update Kurse
    QList<KursUndDatum> &lkudOldKurse = WknFile.GetVect("Fond","Kurse");
    lkudOldKurse.clear();
    QList<KursUndDatum>::iterator vIt(dqKUD.begin());  // Iterator am Anfang des Vektors
    for (;vIt != dqKUD.end(); vIt++)
    {
        if ((*vIt).cDatum.isValid())
        {
            lkudOldKurse.append(*vIt) ;
            iSaveCount++;
        }
    }

    //Update Auss
    QList<KursUndDatum> &lkudOldAuss = WknFile.GetVect("Fond","Auss");
    lkudOldAuss.clear();
    vIt = dqAusschuettung.begin();  // Iterator am Anfang des Vektors
    for (;vIt != dqAusschuettung.end(); vIt++)
    {
        if ((*vIt).cDatum.isValid())
        {
            lkudOldAuss.append(*vIt) ;
            iSaveCount++;
        }
    }

    WknFile.AddData("Fond","LastUpdate",iDateToday.toString("dd.MM.yyyy"));
    WknFile.save(sWknFileName);
    return iSaveCount;
}





int fond::DownloadFondData(QDate cdMax) {

    //int iPage = 0;
    int iNewDataCount = 0;

    KursUndDatum kudKursUndDatum;

    QString sBID = WknFile.GetData("Fond","BoerseID");

#ifndef NOOUTPUT
    qInfo() << "Erstelle Fondtabelle mit BorseID" << sBID;
#endif

    if (sBID.length() == 0)
    {
        sBID = "131";
        int iBID = sBID.toInt();
        qInfo() << "Benutze: " << "Tradegate" << " (" << iBID << ")";

        WknFile.AddData("Fond","BoerseID",sBID);
        if (DownloadData(cdMax, iDateToday,iBID) != 0) {iStatus = 2;return -1;}
    } else {
        int iBID = sBID.toInt(/* bOK */);
        if (DownloadData(cdMax, iDateToday,iBID) != 0) {iStatus = 2;return -1;}
    }



    while(cHttp.GetFondData(kudKursUndDatum) == 0)
    {
        if (cdMax.isValid() && (cdMax >= kudKursUndDatum.cDatum))
        {
            break;
        }
        dqKUD.push_back(kudKursUndDatum);
        iNewDataCount++;
    };


    cHttp.Empty();

#ifndef NOOUTPUT
    qInfo() << "Anzahl neuer Einträge: " << iNewDataCount;
    if (iNewDataCount > 0) qInfo() << "aktuellstes Datum: " << dqKUD.at(0).cDatum;
#endif

    return iNewDataCount;
}


int fond::DownloadData(QDate DStart, QDate DEnd, int iBoerseID, bool bClean_Split, bool bClean_Payout ,bool bClean_Bezug)
{

#ifndef NOOUTPUT
    qInfo() << "Anfangsdatum: " << DStart	 << ", Enddatum: " << DEnd ;
#endif

    QString cUrl;
    QTextStream ssUrl(&cUrl);

    ssUrl << "/quote/historic/historic.csv?secu=" << cSecu;

    ssUrl << "&boerse_id=" << iBoerseID;

    if (DStart.isValid())
    {
        ssUrl << "&min_time=" << DStart.toString("d.M.yyyy");
    } else {
        QDate dH = QDate::fromString("01.01.1990");
        ssUrl << "&min_time=" << dH.toString("d.M.yyyy");
    }

    ssUrl << "&max_time=" << DEnd.toString("d.M.yyyy");
    ssUrl << "&clean_split=" << (bClean_Split?"1":"0");
    ssUrl << "&clean_payout=" << (bClean_Payout?"1":"0");
    ssUrl << "&clean_bezug=" << (bClean_Bezug?"1":"0");
    ssUrl << "&trenner=%3B&go=Download";

#ifndef LOWOUTPUT
    qInfo() << "Connecting with " << cUrl;
#endif
    if (cHttp.verbinden(cHttp.cAddr,443) != 0) return -1;

    if (cHttp.SendGetRequest(cUrl) == false) return - 1;



    cHttp.ReplaceEveryThing (" " ,"X");
    cHttp.ReplaceEveryThing (";" ," ");
    cHttp.DeleteUntil("Volumen");
    cHttp.DeleteEveryThing(".");
    cHttp.ReplaceEveryThing(",",".");



    if (cHttp.sHttpData.length() > 0)
    {
        cHttp.ssFondData << cHttp.sHttpData;
    }
    cHttp.sHttpData.clear();

    return 0;
}



double fond::GetCurrentWert(const QDate& cDatum)
{
    int iCountStep = 0;

    if (iStatus == 1) if (LoadAllData() == false) return 0;


    if (dqKUD.size() == 1)
    {
        if (dqKUD[iKUD].cDatum > cDatum) return 0;
        return dqKUD[iKUD].iKurs;
    }

    while (dqKUD[iKUD].cDatum <= cDatum)
    {
        if (iKUD == 0)
        {
            //cout << "Abort with " <<  iCountStep << " Steps: " << itKUDlow->cDatum << " " << itKUDlow->iKurs << " for " << (Datum) cDatum << endl;
            return dqKUD[iKUD].iKurs;
        }
        //itKUDlow--;
        //itKUDhigh--;
        iKUD--;
        iCountStep++;

    }

    while ( ((iKUD+2) < dqKUD.size()) && (cDatum < dqKUD[iKUD+1].cDatum))
    {
        //itKUDlow++;
        //itKUDhigh++;
        iKUD++;
        iCountStep++;
    }

    //cout << (Datum) cDatum << "Post: " << itKUDhigh->cDatum << " ";
    //cout << itKUDlow->cDatum << endl;


    if (dqKUD[iKUD+1].cDatum > cDatum)
    {
        qWarning().noquote() << "Requested date ("<< cDatum << ") before start of historical data ("<< dqKUD[iKUD+1].cDatum << ")";
        return 0;
    }

    //cout << "With " <<  iCountStep << " Steps: " << itKUDhigh->cDatum << " " << itKUDhigh->iKurs << " for " << (Datum) cDatum << endl;

    return dqKUD[iKUD+1].iKurs;
}

double fond::GetAussFactor(const QDate& cDat)
{
    //cout << "double fond::GetAussFactor" << endl;
    if (iStatus == 1) if (LoadAllData() == false) return 0;
    if (dqAusschuettung.size() == 0) return 1;
    if (dqAusschuettung.size() == 1)
    {
        if (itAussLow->cDatum > cDat) return itAussLow->iKurs;
        return 1;
    }

    while (itAussLow->cDatum <= cDat)
    {
        if (itAussLow == dqAusschuettung.begin())
        {
            return 1;
        }
        itAussLow--;
        itAussHigh--;
    }
    while (cDat < itAussHigh->cDatum)
    {
        itAussLow++;
        itAussHigh++;
        if (itAussHigh == dqAusschuettung.end())
        {
            itAussLow--;
            itAussHigh--;
            return itAussHigh->iKurs;
        }

    }
    //cout << (Datum) cDat << "Post: " << itAussHigh->cDatum << " ";
    //cout << itAussLow->cDatum << endl;

    return itAussLow->iKurs;
}

double fond::GetTotalWert(const QDate &cDatum)
{
    return GetCurrentWert(cDatum) / GetAussFactor(cDatum.addDays(-1));
}

double fond::GetCurrentAverage(const QDate &cDatum, int iTage)
{
    double dSum = 0;
    for (int iDat = -iTage; iDat < 0; iDat++)
        dSum += GetTotalWert(cDatum.addDays(iDat));

    dSum /= iTage;


    return dSum;
}

void fond::DownloadFundamental()
{
    QString cUrl =  "https://www.ariva.de";
    cUrl += STsURL;
    cUrl += "/bilanz-guv";

    html hSite;
    hSite.open(cUrl);

    if (!hSite.cdTotalDir("/html/body/div3/div2/div8"))
    {
        qInfo() << "===============PrintChildNodes==================";
        hSite.PrintChildNodes();
        qInfo() << "=======End=====PrintChildNodes==================";
        return;
    }

    /*std::vector<HtmlNode*> vhnDiv = hSite.hCurrentNode->GetElementsByName("div");
    std::vector<HtmlNode*>::iterator itDiv;

    for (itDiv = vhnDiv.begin();itDiv != vhnDiv.end();itDiv++)
    {
        HtmlNode * phnDiv = *itDiv;
        if (phnDiv->vectHtmlChildNode.size() == 0) continue;
        if ( strcmp(phnDiv->vectHtmlChildNode[0]->sName.c_str(),"h3") == 0)
        {
            phnDiv = phnDiv->vectHtmlChildNode[0];
            if (phnDiv->vectHtmlChildNode.size() == 0) continue;
            cout << phnDiv->vectHtmlChildNode[0]->sName << endl;

        }

    }*/

    QVector<HtmlNode*> vhnDiv = hSite.hCurrentNode->GetElementsByName("table");
    //QVector<HtmlNode*>::iterator itDiv;
    if (vhnDiv.size() < 6) return;
    hSite.cdDown("table6",false);
    vhnDiv = hSite.hCurrentNode->GetElementsByName("tr");
    if (vhnDiv.size() < 2) return;
    hSite.cdDown("tr2",false);
    vhnDiv = hSite.hCurrentNode->GetElementsByName("td");
    if (vhnDiv.size() < 6) return;
    hSite.cdDown("td6",false);
    if (hSite.hCurrentNode->vectHtmlChildNode.size() == 0) return;
    QString sPers = hSite.hCurrentNode->vectHtmlChildNode[0]->sName;
    sPers = sPers.remove(".");
    //boost::algorithm::erase_all(sPers,);
    qInfo() << sPers.toInt();

    STiPers =  sPers.toInt();
}



bool GetSecu(const QString cSearch, QString &sSecu, QString &sURL, QString &sName)
{
    QString cUrl =  "https://www.ariva.de/search/livesearch.m?searchname=";
    cUrl += cSearch;

    html hSite;
    hSite.open(cUrl);

    // Secu
    if (hSite.cdTotalDir("/div/input1") == false)
    {
        qInfo() << "===============PrintChildNodes==================";
        hSite.PrintChildNodes();
        qInfo() << "=======End=====PrintChildNodes==================";
        return false;
    }
    QString sHelp = hSite.hCurrentNode->vectHtmlParams[2];
    sSecu = sHelp.mid(7,sHelp.size() - 8);

    //Url & Name
    if (hSite.cdTotalDir("/div/table/tr/td1/a") == false) return false;
    sHelp =  hSite.hCurrentNode->vectHtmlParams[0];
    sName =  hSite.hCurrentNode->vectHtmlChildNode[1]->sName;
    sURL = sHelp.mid(6,sHelp.size()-7);
    //sSecu = sURL;
    return true;
}


