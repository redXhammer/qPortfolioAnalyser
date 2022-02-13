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
    return QDate::fromString(WknFile.GetData("Fond","LastUpdate"));
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
#ifndef NOOUTPUT
  qInfo() << "Datum der Fonddatei" << DateOfWkn();
#endif
  iStatus = 1;
  return;
}

bool fond::LoadAllData()
{
    if (cSecu.length() == 0 || STsURL.length() == 0)
    {
        if (GetSecu(sWkn,cSecu,STsURL) == false)
        {
            qInfo() << "Secu Error";
            iStatus = 2;
            return -1;
        }
        else
        {
            WknFile.AddData ("Fond","SECU",cSecu ) ;
            WknFile.AddData ("Fond","URL",STsURL ) ;
        }
    }

    WknFile.open( sWknFileName ); // Öffnen der WKN file
    if (LoadFondKurse() < 1)
    {
        WknFile.DropData();
        return false;
    }

    LoadFondAuss();

    return true;
}

int fond::LoadFondKurse()
{
  KursUndDatum kudHelp;

  QList<KursUndDatum> vKurse = WknFile.GetVect("Fond","Kurse");
  if (vKurse.size() == 0)
  {
    qInfo() << "No File Opened";
    DownloadFondData();
    qInfo() << "Speichern : " << SaveFondData() << " Einträge";

    iKUD = 0;
    iStatus = 0;
    return dqKUD.size();
  }

    kudHelp = vKurse.at(0);

	if ((DateOfWkn() < (iDateToday)) || AKTUALISIEREN) /************** Beginn des Aktualisierungsprozess *************/
	{
#ifndef NOOUTPUT
        qInfo() << "Datum des letzen lokalen Fondeintrags: " << kudHelp.cDatum;
        qInfo() << "Aktualisiere ... ";
#endif

		DownloadFondData(kudHelp.cDatum);

#ifndef NOOUTPUT
        qInfo() << "Fertig";
#endif


        dqKUD.append(vKurse);

    #ifndef NOOUTPUT
        qInfo() << "Speichern : " << SaveFondData() << " Einträge";
	#else
		SaveFondData();
    #endif
	} else {
    #ifndef NOOUTPUT
        qInfo() <<"First: " << kudHelp.cDatum << " "<< kudHelp.iKurs;
    #endif
        dqKUD.append(vKurse);
    #ifndef NOOUTPUT
        kudHelp = dqKUD.back();
        qInfo() <<"Last: " << kudHelp.cDatum << " "<< kudHelp.iKurs;
    #endif
	}

	iKUD = 0;
	iStatus = 0;
    return dqKUD.size();
}

int fond::LoadFondAuss()
{

    //if (DateOfWkn() < iDateToday)
    {
        html hSite;
        QString sUrlHistEig = "https://www.ariva.de";
        sUrlHistEig += STsURL;
        sUrlHistEig += "/ausschuettungen/";
        hSite.open(sUrlHistEig);

        hSite.cdTotalDir("/html/body/div3/div2/div8/div/table");

        QVector<HtmlNode*> vhnTR = hSite.hCurrentNode->GetElementsByName("tr");

        for (int iElem = 1; iElem < vhnTR.size(); iElem++)
        {
            HtmlNode* hnNode = vhnTR[iElem];

            if (QString("Dividende") == hnNode->GetElementsByName("td")[1]->vectHtmlChildNode[0]->sName)
            {
                if (hnNode->GetElementsByName("td")[3]->vectHtmlChildNode.size() == 0) continue;




                KursUndDatum kudAuss;
                int iPosRev = hnNode->GetElementsByName("td")[3]->vectHtmlChildNode[0]->sName.lastIndexOf(',');

                double dAussWert = hnNode->GetElementsByName("td")[3]->vectHtmlChildNode[0]->sName.replace(iPosRev,1,".").toDouble(/*&bOK*/) ;

                kudAuss.cDatum = QDate::fromString(hnNode->GetElementsByName("td")[0]->vectHtmlChildNode[0]->sName); // - 1;
                double dCurrWert = GetCurrentWert(kudAuss.cDatum);


                kudAuss.iKurs = 1;
                kudAuss.iKurs += dAussWert/GetCurrentWert(kudAuss.cDatum);

                qInfo() << "Divi am " << hnNode->GetElementsByName("td")[0]->vectHtmlChildNode[0]->sName
                        << " mit "    << hnNode->GetElementsByName("td")[3]->vectHtmlChildNode[0]->sName
                        << " Euro ("  << dAussWert<< "," << dCurrWert
                        << ") ("      << kudAuss.iKurs << ")";

                if (dCurrWert == 0) continue;

                if (dqAusschuettung.size()>0) {
                    kudAuss.iKurs *= (dqAusschuettung.back().iKurs);
                }

                dqAusschuettung.push_back(kudAuss);
                itAussHigh = dqAusschuettung.begin();
                itAussLow = itAussHigh;
                itAussHigh++;
            }

            else if (QString("Split") == hnNode->GetElementsByName("td")[1]->vectHtmlChildNode[0]->sName)
            {
                KursUndDatum kudAuss;

                QString sVerh = hnNode->GetElementsByName("td")[2]->vectHtmlChildNode[0]->sName;
                int iPos = sVerh.indexOf(":");

                kudAuss.cDatum = QDate::fromString(hnNode->GetElementsByName("td")[0]->vectHtmlChildNode[0]->sName); // - 1;

                //kudAuss.iKurs = 0;
                kudAuss.iKurs = sVerh.mid(iPos+1).toDouble(/*&bOK*/) / sVerh.mid(0,iPos).toDouble(/*&bOK*/);

                if (dqAusschuettung.size()>0) {
                    kudAuss.iKurs *= (dqAusschuettung.back().iKurs);
                }


                qInfo() << "Split am "        << hnNode->GetElementsByName("td")[0]->vectHtmlChildNode[0]->sName
                        << " mit Verhältnis " << hnNode->GetElementsByName("td")[2]->vectHtmlChildNode[0]->sName
                        << " ("               << kudAuss.iKurs << ")";

                dqAusschuettung.push_back(kudAuss);
                itAussHigh = dqAusschuettung.begin();
                itAussLow = itAussHigh;
                itAussHigh++;
            }
        }
    }
    return 0;
}

QList<KursUndDatum> fond::DownloadFondAuss()
{
    QList<KursUndDatum> dqAuss;
    cSecu = WknFile.GetData("Fond","SECU") ;


    return dqAuss;
}




int fond::SaveFondData()
{
  int iSaveCount = 0;
  //std::fstream oFile(sWknFileName.c_str(),ios::out);		// Speichern der Daten
  QList<KursUndDatum> &lkudOldVect = WknFile.GetVect("Fond","Kurse");

  QList<KursUndDatum>::iterator vIt(dqKUD.begin());  // Iterator am Anfang des Vektors
  for (;vIt != dqKUD.end(); vIt++)
  {
    if ((*vIt).cDatum.isValid())
    {
      lkudOldVect.append(*vIt) ;
      iSaveCount++;
    }
  }
  WknFile.AddData("Fond","LastUpdate",iDateToday.toString());
  WknFile.save(sWknFileName);
#ifndef LOWOUTPUT
  qInfo() << "geschrieben";
#endif

  return iSaveCount;
}





int fond::DownloadFondData(QDate cdMax) {

	//int iPage = 0;
	int iNewDataCount = 0;

	KursUndDatum kudKursUndDatum;



#ifndef NOOUTPUT
    qInfo() << "Erstelle Fondtabelle ...";
#endif


    QString sBID = WknFile.GetData("Fond","BoerseID");
    if (sBID.length() == 0)
    {
        html hSite;
        QString cUrl =  "https://www.ariva.de";
        cUrl += STsURL;
        cUrl += "/historische_kurse";
        hSite.open(cUrl);
        hSite.cdTotalDir("/html/body/div3/div2/div8/div2/ul2");

        QVector<HtmlNode*> vhtmlnodeListTR;
        QVector<HtmlNode*>::iterator itNodeFromList;
        vhtmlnodeListTR = hSite.hCurrentNode->GetElementsByName("li");

        int iAnz = 0;
        QString sBID2 ;
        QString sBoerse;

        for (itNodeFromList = vhtmlnodeListTR.begin();
            itNodeFromList != vhtmlnodeListTR.end();
            itNodeFromList++)
        {
            HtmlNode* pNode = *itNodeFromList;
            pNode = pNode->GetElementsByName("a")[0]; //a mit href -> BID
            sBID = pNode->vectHtmlParams[0];

            QString sNumData;
            if (pNode->vectHtmlParams[1] == "class=\"bold")
                sNumData = pNode->vectHtmlParams[5] ;
            else
                sNumData = pNode->vectHtmlParams[3] ;

            if (iAnz < sNumData.mid(17).toInt(/* bOK */) )
            {
                size_t iPosBoerseId = sBID.lastIndexOf("boerse_id") + 10;
                size_t iPosTueddel = sBID.lastIndexOf("\"");
                sBID2 = sBID.mid(iPosBoerseId,iPosTueddel-iPosBoerseId);
                iAnz = sNumData.mid(17).toDouble(/* bOK */);
                pNode = pNode->vectHtmlChildNode[0]; // Name
                sBoerse = pNode->sName;
            }

        }

        int iBID = sBID2.toInt(/* bOK */);
        qInfo() << "Benutze: " << sBoerse << " (" << iBID << ")";

        WknFile.AddData("Fond","BoerseID",sBID2);
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
	if (iNewDataCount > 0) cout << "aktuellstes Datum: " << dqKUD.at(0).cDatum;
#endif

	return 0;
}


int fond::DownloadData(QDate DStart, QDate DEnd, int iBoerseID, bool bClean_Split, bool bClean_Payout ,bool bClean_Bezug)
{

#ifndef NOOUTPUT
		cout << "Anfangsdatum: " << DStart;
		cout << ", Enddatum: " << DEnd << endl;
#endif

  QString cUrl;
  QTextStream ssUrl(&cUrl);

  ssUrl << "/quote/historic/historic.csv?secu=" << cSecu;

  ssUrl << "&boerse_id=" << iBoerseID;

  if (DStart.isValid())
	{
    ssUrl << "&min_time=" << DStart.toString();
  } else {
    QDate dH = QDate::fromString("01.01.1990");
    ssUrl << "&min_time=" << dH.toString();
  }

  ssUrl << "&max_time=" << DEnd.toString();
  ssUrl << "&clean_split=" << (bClean_Split?"1":"0");
  ssUrl << "&clean_payout=" << (bClean_Payout?"1":"0");
  ssUrl << "&clean_bezug=" << (bClean_Bezug?"1":"0");
  ssUrl << "&trenner=%3B&go=Download";

#ifndef LOWOUTPUT
    cout << "Connecting with " << cAddr << endl;
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
    //cout << "double fond::GetCurrentWert" << endl;
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

	while ((cDatum < dqKUD[iKUD+1].cDatum) && ((iKUD+1) < dqKUD.size()))
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

    hSite.cdTotalDir("/html/body/div3/div2/div8");

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



bool GetSecu(const QString cSearch, QString &sSecu, QString &sURL)
{
    QString cUrl =  "https://www.ariva.de/search/livesearch.m?searchname=";
    cUrl += cSearch;

    html hSite;
    hSite.open(cUrl);

    // Secu
    if (hSite.cdTotalDir("/div/input1") == false) return false;
    QString sHelp = hSite.hCurrentNode->vectHtmlParams[2];
    sSecu = sHelp.mid(7,sHelp.size() - 8);

    //Url
    if (hSite.cdTotalDir("/div/table/tr/td1/a") == false) return false;
    sHelp =  hSite.hCurrentNode->vectHtmlParams[0];
    sURL = sHelp.mid(6,sHelp.size()-7);

	return true;
}


