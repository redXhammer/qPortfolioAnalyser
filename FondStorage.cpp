#include "fond.h"
#include "QDebug"

/** @brief (one liner)
  *
  * (documentation goes here)
  */
fond * FondStorage::GetFondClass(const QString &wkn)
{
    fond* pF;
    QMap<QString,fond*>::iterator itFondMap;
    itFondMap = find(wkn);
    if (itFondMap == end())
    {
        pF = AddFond(wkn);
    }
    else{
        pF = itFondMap.value();
    }
    pF->LoadAllData();
    qInfo().noquote() << "Updated Data for" << wkn << pF->STsName;
    return pF;
}

/** @brief (one liner)
  *
  * (documentation goes here)
  */
fond * FondStorage::AddFond(const QString &wkn)
{
    fond* pF = new fond(wkn);
    operator[] (wkn) = pF;
    return pF;
}

/** @brief (one liner)
  *
  * (documentation goes here)
  */
 FondStorage::~FondStorage()
{
    QMap<QString,fond*>::iterator itFondMap;

    while (  size() > 0 )
    {
        itFondMap=begin();
        delete(itFondMap.value());
        erase (itFondMap);
    }
}

/** @brief (one liner)
  *
  * (documentation goes here)
  */
 FondStorage::FondStorage()
{
    iStatus = 1;
}



/*std::istream& operator >> (std::istream& OS, FondStorage& FS)
{
    //qInfo() <<  << endl << endl;
}*/

/** @brief (one liner)
  *
  * (documentation goes here)
  */
bool FondStorage::SyncWithInternet()
{
    QMap<int,QString> mKags = GetKags();
    QMap<int,QString>::iterator itKag;

    for ( itKag=mKags.begin() ; itKag != mKags.end(); itKag++ )
    {

        if (GetKagFonds(itKag.key()))
        {
            qInfo() << " Result: OK for "<< itKag.key() << " => " << itKag.value() << endl;
        }else{
            qInfo() << " Result: Failed for "<< itKag.key() << " => " << itKag.value() << endl;
            getchar();
        }

    }
    return true;
}


QMap<int,QString>  FondStorage::GetKags()
{
    QMap<int,QString> mKags;
    http cHttp;

    cHttp.SetAddr ( "www.finanzen.net" ) ;
    cHttp.SendGetRequest("/fonds/") ;

    cHttp.DeleteUntil("<select name=\"frmFondsKAGNr\">");
    cHttp.DeleteFrom("</select>");
    cHttp.DeleteEveryThing("<option value=\"");
    cHttp.DeleteEveryThing("</option>");
    cHttp.ReplaceEveryThing("\">","\t");

    QTextStream ssHttp(&cHttp.sHttpData);

    while (!ssHttp.atEnd())
    {
        QString sLine = ssHttp.readLine();

        size_t iFindWKN = sLine.indexOf("\t");

        bool bOK;
        int iKagNr = sLine.mid(0,iFindWKN).toInt(&bOK);
        if (!bOK)
        {}
        QString cName = sLine.mid(iFindWKN+1);
        if (iKagNr != 0) mKags[iKagNr] = cName;
    }

    return mKags;
}

/** @brief (one liner)
  *
  * (documentation goes here)
  */
bool FondStorage::GetKagFonds(int iKagNr)
{
    http cHttp;
    int iDownloadedFondCount = 0;

    cHttp.SetAddr ( "www.finanzen.net" ) ;
    QString cAddress = "/fonds/fonds_suchergebnis.asp";
    QString cPostData;
    QTextStream ssOut(&cPostData);
    ssOut << "frmFonds=&frmFondsKAGNr="
            << iKagNr
            << "&frmFondsKategorie=&frmFondsKategorieNr=0&frmFondsAusgabeaufschlag=&frmFondsMindestalter=&frmFonds1JahresPerformance=&inFondsFondsnote=&frmFondsSortierung=0&x=9&y=11";



//http://www.finanzen.net/fonds/fonds_suchergebnis.asp?frmfonds=axa&frmfondssortierung=0&x=23&y=6&intpagenr=2&KAGNr=&inFondsFondsnote=&frmFondsKategorie=&frmFondsKategorieNr=&frmFonds1JahresPerformance=&frmFondsMindestalter=&frmFondsAusgabeaufschlag=

    if (cHttp.SendPostRequest(cAddress,cPostData) == false) return false;
    //cHttp.recv();

    int iFondCount = cHttp.GetKagFondCount();

    qInfo() << "Totaly " << iFondCount << " Fonds" << endl;
    cHttp.ClipHttpCommand("div","class=\"inhalt t1\"");
    cHttp.ClipHttpCommand("table","");


    cHttp.ReplaceEveryThing("<tr>","\n");
    cHttp.DeleteEveryThing("</tr>");
    cHttp.ReplaceEveryThing("</td>","\t");
    cHttp.DeleteEverywereEveryThingBetween("<td",">");

    cHttp.ReplaceCommWithoutNames("a","href","\t");
    cHttp.DeleteEverywereEveryThingBetween("<th","</th>");
    int iMoreThen50 = cHttp.DeleteFrom("Seite:",false);
    cHttp.DeleteEverywereEveryThingBetween("<colgroup>","</colgroup>");
    cHttp.DeleteEveryThing("</a>");
    cHttp.ReplaceEveryThing("\n\t","\n");
#ifndef NOOUTPUT
    qInfo() << cHttp.sHttpData << endl;
#endif
    QTextStream ssHttp(&cHttp.sHttpData);

    while (!ssHttp.atEnd())
    {
        QString sLine = ssHttp.readLine();
        if (sLine.length() != 0)
        {

            size_t iFindWKN = sLine.indexOf("\t");
            size_t iFindWKN2 = sLine.indexOf("\t",iFindWKN+1);

            //iFindWKN = sLine.find("\t",iFindWKN+1);
            //iFindWKN2 = sLine.find("\t",iFindWKN+1);

            QString cUrl = sLine.mid(0,iFindWKN);
            QString cName = sLine.mid(iFindWKN+1,iFindWKN2-iFindWKN-1);
            iFindWKN2 = sLine.indexOf("\t",(iFindWKN = iFindWKN2) + 1);
            QString cWKN = sLine.mid(iFindWKN+1,iFindWKN2-iFindWKN-1);
            iFindWKN2 = sLine.indexOf("\t",(iFindWKN = iFindWKN2) + 1);
            QString cKAG = sLine.mid(iFindWKN+1,iFindWKN2-iFindWKN-1);
#ifndef NOOUTPUT
            qInfo() << cUrl << endl << cName << endl << cWKN << endl << cKAG << endl;
#endif
            fond* pF = AddFond(cWKN);
            pF->STsURL = cUrl;
            pF->STsName = cName;
            pF->STsKag = cKAG;

            iDownloadedFondCount++;
        }
    }
    qInfo() << "Got " << iDownloadedFondCount << " and " << iFondCount << " left." << endl;
    iFondCount -= 50;
    if ((iMoreThen50 == - 1) && (iFondCount > 50)) qInfo() << "FondCountError" << endl;
    if (iMoreThen50 == 0)
    {
        int iPage = 2;
        while (iFondCount > 0)
        {

            cHttp.SetAddr ( "www.finanzen.net" ) ;
            cAddress = "/fonds/fonds_suchergebnis.asp";
            cPostData ="intpagenr=";

            ssOut << iPage++
                << "&frmFonds=&frmFondsKAGNr="
                << iKagNr
                << "&frmFondsKategorie=&frmFondsKategorieNr=0&frmFondsAusgabeaufschlag=&frmFondsMindestalter=&frmFonds1JahresPerformance=&inFondsFondsnote=&frmFondsSortierung=0&x=9&y=11";

            cAddress += "?";
            cAddress += cPostData;

            cHttp.SendGetRequest(cAddress) ;

            cHttp.ClipHttpCommand("div","class=\"inhalt t1\"");
            cHttp.ClipHttpCommand("table","");

            cHttp.ReplaceEveryThing("<tr>","\n");
            cHttp.DeleteEveryThing("</tr>");
            cHttp.ReplaceEveryThing("</td>","\t");
            cHttp.DeleteEverywereEveryThingBetween("<td",">");

            cHttp.ReplaceCommWithoutNames("a","href","\t");
            cHttp.DeleteEverywereEveryThingBetween("<th","</th>");

            //int iMoreThen50 = cHttp.DeleteFrom("Seite:",false);
            cHttp.DeleteFrom("Seite:",false);

            cHttp.DeleteEverywereEveryThingBetween("<colgroup>","</colgroup>");
            cHttp.DeleteEveryThing("</a>");
            cHttp.ReplaceEveryThing("\n\t","\n");
#ifndef NOOUTPUT
            qInfo() << cHttp.sHttpData << endl;
#endif
            QTextStream ssHttp(&cHttp.sHttpData);

            while (!ssHttp.atEnd())
            {
                QString sLine = ssHttp.readLine();
                if (sLine.length() != 0)
                {

                    size_t iFindWKN = sLine.indexOf("\t");
                    size_t iFindWKN2 = sLine.indexOf("\t",iFindWKN+1);

                    QString cUrl = sLine.mid(0,iFindWKN);
                    QString cName = sLine.mid(iFindWKN+1,iFindWKN2-iFindWKN-1);
                    iFindWKN2 = sLine.indexOf("\t",(iFindWKN = iFindWKN2) + 1);
                    QString cWKN = sLine.mid(iFindWKN+1,iFindWKN2-iFindWKN-1);
                    iFindWKN2 = sLine.indexOf("\t",(iFindWKN = iFindWKN2) + 1);
                    QString cKAG = sLine.mid(iFindWKN+1,iFindWKN2-iFindWKN-1);
#ifndef NOOUTPUT
                    qInfo() << cUrl << endl << cName << endl << cWKN << endl << cKAG << endl;
#endif
                    fond* pF = AddFond(cWKN);
                    pF->STsURL = cUrl;
                    pF->STsName = cName;
                    pF->STsKag = cKAG;

                    iDownloadedFondCount++;
                }
            }
            qInfo() << "Got " << iDownloadedFondCount << " and " << iFondCount << " left." << endl;
            iFondCount -= 50;
        }
    }
    return true;
}

/** @brief (one liner)
  *
  * (documentation goes here)
  */
bool FondStorage::UpdateLoadesFiles()
{
  return false;
}

/** @brief (one liner)
  *
  * (documentation goes here)
  */
bool FondStorage::LoadContainerFile(const QString &file)
{
    DataFile dfLoadData;
    if (dfLoadData.open(file) == true)
    {
        ITdblMAP itContainerData = dfLoadData.GetDataItBegin();
        for (;itContainerData != dfLoadData.GetDataItEnd();itContainerData++)
        {
            fond* pF = AddFond(itContainerData.key());
            pF->cSecu = dfLoadData.GetData(itContainerData,"SECU");
            pF->STsURL = dfLoadData.GetData(itContainerData,"URL");
            pF->STsName = dfLoadData.GetData(itContainerData,"NAME");
            //pF->sWkn;
            //pF->sWknFileName;
            pF->STsKag = dfLoadData.GetData(itContainerData,"KAG");
            pF->STsISIN = dfLoadData.GetData(itContainerData,"ISIN");
            //pF->STdAuflageDatum;
            pF->STsKategorie = dfLoadData.GetData(itContainerData,"KATEGORIE");
            pF->STsWaehrung = dfLoadData.GetData(itContainerData,"WÄHRUNG");
            bool bOK;
            pF->STiPers = dfLoadData.GetData(itContainerData,"Pers").toInt(&bOK);
            if (bOK) {}
            /*pF->STsVolumen;
            pF->STdAusgabeaufschlag;
            pF->STdVerwaltungsgebuehr;
            pF->STdDepotbankgebuehr;
            pF->STbSparplanfaehig;
            pF->STdMinimalanlage;
            pF->STbAusschuettend;*/
        }

    }

    return 0;
}

/** @brief (one liner)
  *
  * (documentation goes here)
  */
bool FondStorage::SaveContainerFile(const QString &file)
{
    DataFile dfLoadData;
    QMap<QString,fond*>::iterator itFSList;
    for (itFSList = begin();itFSList != end();itFSList++)
    {
        QString wkn = itFSList.value()->sWkn;
        dfLoadData.AddData (wkn,"SECU",itFSList.value()->cSecu);
        dfLoadData.AddData (wkn,"URL",itFSList.value()->STsURL);
        dfLoadData.AddData (wkn,"NAME",itFSList.value()->STsName);
        dfLoadData.AddData (wkn,"KAG",itFSList.value()->STsKag);
        dfLoadData.AddData (wkn,"Pers",QString::number(itFSList.value()->STiPers));
    }
    return dfLoadData.save(file);
}

int FondStorage::GetFilteredList(QStringList & vList)
{
    QMap<QString,fond*>::iterator itFSList;
    int iCount = 0;
    for (itFSList = begin();itFSList != end();itFSList++)
    {
        if (itFSList.value()->STiPers > 100)
        {
            vList.push_back(itFSList.value()->sWkn);
            iCount++;
        }
    }
    return iCount;
}

