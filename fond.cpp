#define FondFile
#include "fond.h"
#include <boost/algorithm/string.hpp>

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
    QDate DHelp;
	DHelp = StrToDate(WknFile.GetData("Fond","LastUpdate"));
	return DHelp;
}

const char* fond::GetWknFileName()
{
	return sWkn.c_str();
}

fond::fond(const char* sWkn2) /***************** Konstructor der Fond-Klasse mit übergabe der WKN **************/
{
    iKUD = 0;
	sWknFileName = "fonddaten/";

	sWkn = sWkn2;
	sWknFileName += sWkn;
	sWknFileName += ".fdd";
#ifndef NOOUTPUT
    qInfo() << "Datum der Fonddatei" << DateOfWkn();
#endif
	iDateToday = GetDateOfToday();
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
            WknFile.AddData ("Fond","SECU",cSecu.c_str() ) ;
            WknFile.AddData ("Fond","URL",STsURL.c_str() ) ;
        }
    }

    WknFile.open( sWknFileName.c_str()); // Öffnen der WKN file
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
    char c[20];
    KursUndDatum kudHelp;

	std::list<std::string>& vKurse = WknFile.GetVect("Fond","Kurse");
	if (vKurse.size() == 0)
	{
		cout << "No File Opened" << endl;
		DownloadFondData();
		cout << "Speichern : " << SaveFondData() << " Einträge" << endl;
		//itKUDlow = dqKUD.begin();
        //itKUDhigh = dqKUD.begin();
        //itKUDhigh++;
        iKUD = 0;
        iStatus = 0;
		return dqKUD.size();
	}
    std::list<std::string>::iterator itKurs = vKurse.begin();
    stringstream ssKurse;
    ssKurse << *itKurs;
    ssKurse >> c;
    kudHelp.cDatum = (int) atoi(c);
    ssKurse >> c;
    kudHelp.iKurs = atof(c);


	if ((DateOfWkn() < (iDateToday)) || AKTUALISIEREN) /************** Beginn des Aktualisierungsprozess *************/
	{
#ifndef NOOUTPUT
		cout << "Datum des letzen lokalen Fondeintrags: " << kudHelp.cDatum << endl;
		cout << "Aktualisiere ... ";
#endif

		DownloadFondData(kudHelp.cDatum);

#ifndef NOOUTPUT
		cout << "Fertig" << endl;
#endif


		dqKUD.push_back(kudHelp);
		itKurs++;
		while(itKurs!=vKurse.end())
		{
		    ssKurse.clear();
		    ssKurse << *itKurs;
			ssKurse >> c;
			kudHelp.cDatum = (int) atoi(c);
			ssKurse >> c;
			kudHelp.iKurs = atof(c);
			dqKUD.push_back(kudHelp);
			itKurs++;
		}
    #ifndef NOOUTPUT
		cout << "Speichern : " << SaveFondData() << " Einträge" << endl;
	#else
		SaveFondData();
    #endif
	} else {
		dqKUD.push_back(kudHelp);
    #ifndef NOOUTPUT
		cout <<"First: " << kudHelp.cDatum << " "<< kudHelp.iKurs << endl;
    #endif
		itKurs++;
		while(itKurs!=vKurse.end())
		{
            if (!ssKurse.eof()) cout << "Not EOF" << endl;
		    ssKurse.clear();
		    ssKurse << (*itKurs);
		    if (ssKurse.eof()) cout << "EOF" << endl;
			ssKurse >> c;
			kudHelp.cDatum = (int) atoi(c);
			ssKurse >> c;
			kudHelp.iKurs = atof(c);
			//cout << *itKurs<< " "<< kudHelp   ;
			itKurs++;
			dqKUD.push_back(kudHelp);
		}
    #ifndef NOOUTPUT
		cout <<"Last: " << kudHelp.cDatum << " "<< kudHelp.iKurs << endl;
    #endif
	}
    //itKUDlow = dqKUD.begin();
	//itKUDhigh = dqKUD.begin();
	//itKUDhigh++;
	iKUD = 0;
	iStatus = 0;
    return dqKUD.size();
}

int fond::LoadFondAuss()
{

    //if (DateOfWkn() < iDateToday)
    {
        html hSite;
        std::string sUrlHistEig = "http://www.ariva.de";
        sUrlHistEig += STsURL;
        sUrlHistEig += "/historische_ereignisse";
        hSite.open(sUrlHistEig.c_str());

        hSite.cdTotalDir("/html/body/div3/div2/div8/div/table");

        std::vector<HtmlNode*> vhnTR = hSite.hCurrentNode->GetElementsByName("tr");

        for (size_t iElem = 1; iElem < vhnTR.size(); iElem++)
        {
            HtmlNode* hnNode = vhnTR[iElem];

            if (strcmp("Dividende",hnNode->GetElementsByName("td")[1]->vectHtmlChildNode[0]->sName.c_str()) == 0)
            {
                if (hnNode->GetElementsByName("td")[3]->vectHtmlChildNode.size() == 0) continue;

                cout << "Divi am " << hnNode->GetElementsByName("td")[0]->vectHtmlChildNode[0]->sName << " mit "// Datum
                    << hnNode->GetElementsByName("td")[3]->vectHtmlChildNode[0]->sName << " Euro (";


                KursUndDatum kudAuss;
                size_t iPosRev = hnNode->GetElementsByName("td")[3]->vectHtmlChildNode[0]->sName.rfind(',');

                double dAussWert = atof(hnNode->GetElementsByName("td")[3]->vectHtmlChildNode[0]->sName.replace(iPosRev,1,".") .c_str());

                kudAuss.cDatum = StrToDate(hnNode->GetElementsByName("td")[0]->vectHtmlChildNode[0]->sName) - 1;
                double dCurrWert = GetCurrentWert(kudAuss.cDatum);
                cout << dAussWert<<"," << dCurrWert<< ") (";

                kudAuss.iKurs = 1;
                kudAuss.iKurs += dAussWert/GetCurrentWert(kudAuss.cDatum);
                cout << kudAuss.iKurs << ")" << endl;

                if (dCurrWert == 0) continue;

                if (dqAusschuettung.size()>0) {
                    kudAuss.iKurs *= (dqAusschuettung.back().iKurs);
                }

                dqAusschuettung.push_back(kudAuss);
                itAussHigh = dqAusschuettung.begin();
                itAussLow = itAussHigh;
                itAussHigh++;
            }

            else if (strcmp("Split",hnNode->GetElementsByName("td")[1]->vectHtmlChildNode[0]->sName.c_str()) == 0)
            {
                cout << "Split am " << hnNode->GetElementsByName("td")[0]->vectHtmlChildNode[0]->sName << " mit Verhältnis "// Datum
                    << hnNode->GetElementsByName("td")[2]->vectHtmlChildNode[0]->sName << " (";

                KursUndDatum kudAuss;

                std::string sVerh = hnNode->GetElementsByName("td")[2]->vectHtmlChildNode[0]->sName;
                size_t iPos = sVerh.find(":");

                kudAuss.cDatum = StrToDate(hnNode->GetElementsByName("td")[0]->vectHtmlChildNode[0]->sName) - 1;

                //kudAuss.iKurs = 0;
                kudAuss.iKurs = atof(sVerh.substr(iPos+1).c_str())/atof(sVerh.substr(0,iPos).c_str());
                if (dqAusschuettung.size()>0) {
                    kudAuss.iKurs *= (dqAusschuettung.back().iKurs);
                }
                cout << kudAuss.iKurs << ")" << endl;
                dqAusschuettung.push_back(kudAuss);
                itAussHigh = dqAusschuettung.begin();
                itAussLow = itAussHigh;
                itAussHigh++;


            }

        }


    }
    return 0;
}

std::deque<KursUndDatum> fond::DownloadFondAuss()
{
    std::deque<KursUndDatum> dqAuss;
    cSecu = WknFile.GetData("Fond","SECU") ;


    return dqAuss;
}




int fond::SaveFondData()
{
	int iSaveCount = 0;
	//std::fstream oFile(sWknFileName.c_str(),ios::out);		// Speichern der Daten
    VectorStream vsStream(&WknFile.GetVect("Fond","Kurse"),200);
    vsStream.ClearVect();
	std::ostream osSave(&vsStream);
	//osSave.rdbuf(&tvsStream);



	if(!osSave.good()) {
		cout << "fond::SaveFondData--Kann nicht schreiben-" << sWknFileName << endl;
	}else{

		deque<KursUndDatum>::iterator vIt(dqKUD.begin());  // Iterator am Anfang des Vektors
		for (;vIt != dqKUD.end(); vIt++)
		{
			if ((*vIt).cDatum.iDatum > 1)
			osSave << *vIt ;
			osSave.flush();
			iSaveCount++;
		}
		WknFile.AddData("Fond","LastUpdate",iDateToday.GetDateAsChars().c_str());
		WknFile.save(sWknFileName.c_str());
#ifndef LOWOUTPUT
		cout << "geschrieben" << endl;
#endif
	}
	return iSaveCount;
}





int fond::DownloadFondData(Datum cdMax) {

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
        std::string cUrl =  "http://www.ariva.de";
        cUrl += STsURL;
        cUrl += "/historische_kurse";
        hSite.open(cUrl.c_str());
        hSite.cdTotalDir("/html/body/div3/div2/div8/div2/ul2");

        std::vector<HtmlNode*> vhtmlnodeListTR;
        std::vector<HtmlNode*>::iterator itNodeFromList;
        vhtmlnodeListTR = hSite.hCurrentNode->GetElementsByName("li");

        int iAnz = 0;
        std::string sBID2 ;
        std::string sBoerse;

        for (itNodeFromList = vhtmlnodeListTR.begin();
            itNodeFromList != vhtmlnodeListTR.end();
            itNodeFromList++)
        {
            HtmlNode* pNode = *itNodeFromList;
            pNode = pNode->GetElementsByName("a")[0]; //a mit href -> BID
            sBID = pNode->vectHtmlParams[0];

            std::string sNumData;
            if (strcmp(pNode->vectHtmlParams[1].c_str(),"class=\"bold")==0)
                sNumData = pNode->vectHtmlParams[5] ;
            else
                sNumData = pNode->vectHtmlParams[3] ;

            if (iAnz < atoi( sNumData.substr(17).c_str() ))
            {
                size_t iPosBoerseId = sBID.rfind("boerse_id") + 10;
                size_t iPosTueddel = sBID.rfind("\"");
                sBID2 = sBID.substr(iPosBoerseId,iPosTueddel-iPosBoerseId).c_str();
                iAnz = atoi( sNumData.substr(17).c_str());
                pNode = pNode->vectHtmlChildNode[0]; // Name
                sBoerse = pNode->sName;
            }

        }
        int iBID = atoi(sBID2.c_str());
        qInfo() << "Benutze: " << sBoerse << " (" << iBID << ")";

        WknFile.AddData("Fond","BoerseID",sBID2.c_str());
        if (DownloadData(cdMax, iDateToday,iBID) != 0) {iStatus = 2;return -1;}
    } else {
        int iBID = atoi(sBID.c_str());
        if (DownloadData(cdMax, iDateToday,iBID) != 0) {iStatus = 2;return -1;}
    }



	while(cHttp.GetFondData(kudKursUndDatum) == 0)
	{
			if ((cdMax.iDatum >= kudKursUndDatum.cDatum.iDatum) && (cdMax.iDatum != 0))
			{
				break;
			}
			dqKUD.push_back(kudKursUndDatum);
			iNewDataCount++;
	};


	cHttp.Empty();

#ifndef NOOUTPUT
	cout << "Anzahl neuer Einträge: " << iNewDataCount;
	if (iNewDataCount > 0) cout << "aktuellstes Datum: " << dqKUD.at(0).cDatum;
#endif

	return 0;
}


int fond::DownloadData(Datum DStart, Datum DEnd, int iBoerseID,bool bClean_Split, bool bClean_Payout ,bool bClean_Bezug)
{

#ifndef NOOUTPUT
		cout << "Anfangsdatum: " << DStart;
		cout << ", Enddatum: " << DEnd << endl;
#endif

	std::string cUrl = "/quote/historic/historic.csv?secu=";
	cUrl += cSecu;
	cUrl += "&boerse_id=";
	std::ostringstream ssOut;
    ssOut << iBoerseID;
	cUrl += ssOut.str();
	if (DStart.iDatum)
	{
		cUrl += "&min_time=";
		cUrl << DStart;
	} else {
	    cUrl += "&min_time=";
	    Datum dH = StrToDate("01.01.1990");
		cUrl << dH ;
	}
	cUrl += "&max_time=";
	cUrl << DEnd;
	cUrl +="&clean_split=";
	if (bClean_Split) cUrl += "1"; else cUrl += "0";
	cUrl +="&clean_payout=";
	if (bClean_Payout) cUrl += "1"; else cUrl += "0";
	cUrl +="&clean_bezug=";
	if (bClean_Bezug) cUrl += "1"; else cUrl += "0";
	cUrl +="&trenner=%3B&go=Download";

#ifndef LOWOUTPUT
    cout << "Connecting with " << cAddr << endl;
#endif
	if (cHttp.verbinden((char*)cHttp.cAddr.c_str(),80) != 0) return -1;

    if (cHttp.SendGetRequest(cUrl.c_str()) == false) return - 1;



	cHttp.ReplaceEveryThing (" " ,"X");
	cHttp.ReplaceEveryThing (";" ," ");
	cHttp.DeleteUntil("Volumen");
	cHttp.DeleteEveryThing(".");
	cHttp.ReplaceEveryThing(",",".");



	if (cHttp.sHttpData.length() > 0)
	{
		cHttp.ssFondData << cHttp.sHttpData;
	}
	cHttp.sHttpData.erase();

	return 0;
}



double fond::GetCurrentWert(const Datum& cDatum)
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

double fond::GetAussFactor(const Datum& cDat)
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

double fond::GetTotalWert(const Datum &cDatum)
{
	return GetCurrentWert(cDatum) / GetAussFactor(cDatum-1);
}

double fond::GetCurrentAverage(const QDate &cDatum, int iTage)
{
    double dSum = 0;
    for (Datum dDat = cDatum - iTage; dDat < cDatum; dDat++)
        dSum += GetTotalWert(dDat);

    dSum /= iTage;


	return dSum;
}

void fond::DownloadFundamental()
{
    std::string cUrl =  "http://www.ariva.de";
	cUrl += STsURL;
	cUrl += "/bilanz-guv";

    html hSite;
    hSite.open(cUrl.c_str());

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

    std::vector<HtmlNode*> vhnDiv = hSite.hCurrentNode->GetElementsByName("table");
    std::vector<HtmlNode*>::iterator itDiv;
    if (vhnDiv.size() < 6) return;
    hSite.cdDown("table6",false);
    vhnDiv = hSite.hCurrentNode->GetElementsByName("tr");
    if (vhnDiv.size() < 2) return;
    hSite.cdDown("tr2",false);
    vhnDiv = hSite.hCurrentNode->GetElementsByName("td");
    if (vhnDiv.size() < 6) return;
    hSite.cdDown("td6",false);
    if (hSite.hCurrentNode->vectHtmlChildNode.size() == 0) return;
    std::string sPers = hSite.hCurrentNode->vectHtmlChildNode[0]->sName;
    boost::algorithm::erase_all(sPers,".");
    cout << atoi(sPers.c_str()) << endl;

    STiPers =  atoi(sPers.c_str());








}



bool GetSecu(const QString cSearch, QString &sSecu, QString &sURL)
{


	std::string cUrl =  "http://www.ariva.de/search/livesearch.m?searchname=";
	cUrl += cSearch;

    html hSite;
    hSite.open(cUrl.c_str());

    // Secu
    if (hSite.cdTotalDir("/div/input1") == false) return false;
    std::string sHelp =  hSite.hCurrentNode->vectHtmlParams[2];
    sSecu = sHelp.substr(7,sHelp.size() - 8);

    //Url
    if (hSite.cdTotalDir("/div/table/tr/td1/a") == false) return false;
    sHelp =  hSite.hCurrentNode->vectHtmlParams[0];
    sURL = sHelp.substr(6,sHelp.size()-7);



	return true;
}







/** @brief (one liner)
  *
  * (documentation goes here)
  */
 VectorStream::VectorStream(std::list<std::string>* pV,int iBuffersize)
{
    pVect = pV;
    itVect = pVect->begin();
    iBufSize = iBuffersize;
#ifndef NOOUTPUT
    cout << "Opening Vectorstream with Initial " << pV->size() << " Elements" << endl;
#endif
    if (iBuffersize)
    {
            pOutput = new char[iBuffersize];
            setp(pOutput, pOutput + iBuffersize);
            pInput = new char[iBuffersize];
            setg(pInput,pInput,pInput);
    }
    else
    {
            pOutput = NULL;
            pInput = NULL;
            setp(0, 0);
            setg(0, 0, 0);
    }



}

/** @brief (one liner)
  *
  * (documentation goes here)
  */
 VectorStream::~VectorStream()
{
sync();

    if (pOutput)
        delete pOutput;
    if (pInput)
        delete pInput;

}

/** @brief (one liner)
  *
  * (documentation goes here)
  */
int VectorStream::sync()
{

    if (pbase() != pptr())
    {
        DWORD dwCharsToWrite = pptr() - pbase();
        pOutput[dwCharsToWrite] = 0;
        pVect->push_back(pOutput);
        //cout << pReserve;
        setp(pbase(), epptr());
    }
    return 0;
}

/** @brief (one liner)
  *
  * (documentation goes here)
  */
int VectorStream::underflow()
{
    if (!(itVect!=pVect->end())) return EOF;
    int iBytesToCopy = itVect->size();
    if (iBytesToCopy>iBufSize) cout << "Error! Buffersize " << iBufSize << " Bytes To Copy " << iBytesToCopy << endl;
    memcpy(pInput,itVect->c_str(),iBytesToCopy);
    pInput[iBytesToCopy++] = '\n';
    pInput[iBytesToCopy] = 0;
    setg(pInput,pInput,pInput+iBytesToCopy);
    itVect++;
    return 0;
}




/** @brief (one liner)
  *
  * (documentation goes here)
  */
int VectorStream::overflow(int_type)
{
    cout << "Overflow" << endl;
    return 0;
}
