#include "fond.h"

using std::cout;
using std::endl;

/** @brief (one liner)
  *
  * (documentation goes here)
  */
fond * FondStorage::GetFondClass(const char* wkn)
{
    fond* pF;
    std::map<std::string,fond*>::iterator itFondMap;
    itFondMap = find(wkn);
    if (itFondMap == end())
    {
        cout << "Cannot find " << wkn << endl;
        pF = AddFond(wkn);
        pF->LoadAllData();
        return pF;
    }
    else{
        pF = itFondMap->second;
        cout << "Loading Data for " << pF->STsName << endl;
        pF->LoadAllData();
        return pF;
    }


}

/** @brief (one liner)
  *
  * (documentation goes here)
  */
fond * FondStorage::AddFond(const char* wkn)
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

    std::map<std::string,fond*>::iterator itFondMap;

    while (  size() > 0 )
    {
        itFondMap=begin();
        delete(itFondMap->second);
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


    //cout <<  << endl << endl;

}*/

/** @brief (one liner)
  *
  * (documentation goes here)
  */
bool FondStorage::SyncWithInternet()
{
    std::map<int,std::string> mKags = GetKags();
    std::map<int,std::string>::iterator itKag;

    for ( itKag=mKags.begin() ; itKag != mKags.end(); itKag++ )
    {

        if (GetKagFonds((*itKag).first))
        {

            cout << " Result: OK for "<< (*itKag).first << " => " << (*itKag).second << endl;

        }else{
            cout << " Result: Failed for "<< (*itKag).first << " => " << (*itKag).second << endl;
            getchar();
        }

    }
    //GetKagFonds(484);
    //GetKagFonds(491);

    return true;
}


std::map<int,std::string> FondStorage::GetKags()
{
    std::map<int,std::string> mKags;
    http cHttp;

    cHttp.SetAddr ( "www.finanzen.net" ) ;
    cHttp.SendGetRequest("/fonds/") ;


    cHttp.DeleteUntil("<select name=\"frmFondsKAGNr\">");
    cHttp.DeleteFrom("</select>");
    cHttp.DeleteEveryThing("<option value=\"");
    cHttp.DeleteEveryThing("</option>");
    cHttp.ReplaceEveryThing("\">","\t");

    std::stringstream ssHttp;
    ssHttp .str( cHttp.sHttpData);

    while (!ssHttp.eof())
    {
        std::string sLine;
        getline(ssHttp,sLine);

        size_t iFindWKN = sLine.find("\t");


        int iKagNr = atoi(sLine.substr(0,iFindWKN).c_str());
        std::string cName = sLine.substr(iFindWKN+1);
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
    std::string cAddress = "/fonds/fonds_suchergebnis.asp";
    std::ostringstream ssOut;
    ssOut << "frmFonds=&frmFondsKAGNr="
            << iKagNr
            << "&frmFondsKategorie=&frmFondsKategorieNr=0&frmFondsAusgabeaufschlag=&frmFondsMindestalter=&frmFonds1JahresPerformance=&inFondsFondsnote=&frmFondsSortierung=0&x=9&y=11";
    std::string cPostData = ssOut.str();



//http://www.finanzen.net/fonds/fonds_suchergebnis.asp?frmfonds=axa&frmfondssortierung=0&x=23&y=6&intpagenr=2&KAGNr=&inFondsFondsnote=&frmFondsKategorie=&frmFondsKategorieNr=&frmFonds1JahresPerformance=&frmFondsMindestalter=&frmFondsAusgabeaufschlag=

    if (cHttp.SendPostRequest(cAddress.c_str(),cPostData.c_str()) == false) return false;
    //cHttp.recv();

    int iFondCount = cHttp.GetKagFondCount();

    cout << "Totaly " << iFondCount << " Fonds" << endl;
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
    cout << cHttp.sHttpData << endl;
#endif
    std::stringstream ssHttp;
    ssHttp .str( cHttp.sHttpData);

    while (!ssHttp.eof())
    {
        std::string sLine;
        getline(ssHttp,sLine);
        if (sLine.length() != 0)
        {

            size_t iFindWKN = sLine.find("\t");
            size_t iFindWKN2 = sLine.find("\t",iFindWKN+1);

            //iFindWKN = sLine.find("\t",iFindWKN+1);
            //iFindWKN2 = sLine.find("\t",iFindWKN+1);

            std::string cUrl = sLine.substr(0,iFindWKN);
            std::string cName = sLine.substr(iFindWKN+1,iFindWKN2-iFindWKN-1);
            iFindWKN2 = sLine.find("\t",(iFindWKN = iFindWKN2) + 1);
            std::string cWKN = sLine.substr(iFindWKN+1,iFindWKN2-iFindWKN-1);
            iFindWKN2 = sLine.find("\t",(iFindWKN = iFindWKN2) + 1);
            std::string cKAG = sLine.substr(iFindWKN+1,iFindWKN2-iFindWKN-1);
#ifndef NOOUTPUT
            cout << cUrl << endl << cName << endl << cWKN << endl << cKAG << endl;
#endif
            fond* pF = AddFond(cWKN.c_str());
            pF->STsURL = cUrl.c_str();
            pF->STsName = cName.c_str();
            pF->STsKag = cKAG.c_str();

            iDownloadedFondCount++;
        }
    }
    cout << "Got " << iDownloadedFondCount << " and " << iFondCount << " left." << endl;
    iFondCount -= 50;
    if ((iMoreThen50 == - 1) && (iFondCount > 50)) cout << "FondCountError" << endl;
    if (iMoreThen50 == 0)
    {
        int iPage = 2;
        while (iFondCount > 0)
        {

            cHttp.SetAddr ( "www.finanzen.net" ) ;
            cAddress = "/fonds/fonds_suchergebnis.asp";
            cPostData ="intpagenr=";
            ssOut.str("");
            ssOut << iPage++
                << "&frmFonds=&frmFondsKAGNr="
                << iKagNr
                << "&frmFondsKategorie=&frmFondsKategorieNr=0&frmFondsAusgabeaufschlag=&frmFondsMindestalter=&frmFonds1JahresPerformance=&inFondsFondsnote=&frmFondsSortierung=0&x=9&y=11";

            cPostData += ssOut.str();

            cAddress += "?";
            cAddress += cPostData;

            cHttp.SendGetRequest(cAddress.c_str()) ;
            //cHttp.recv();


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
            cout << cHttp.sHttpData << endl;
#endif
            std::stringstream ssHttp;
            ssHttp .str( cHttp.sHttpData);

            while (!ssHttp.eof())
            {
                std::string sLine;
                getline(ssHttp,sLine);
                if (sLine.length() != 0)
                {

                    size_t iFindWKN = sLine.find("\t");
                    size_t iFindWKN2 = sLine.find("\t",iFindWKN+1);

                    std::string cUrl = sLine.substr(0,iFindWKN);
                    std::string cName = sLine.substr(iFindWKN+1,iFindWKN2-iFindWKN-1);
                    iFindWKN2 = sLine.find("\t",(iFindWKN = iFindWKN2) + 1);
                    std::string cWKN = sLine.substr(iFindWKN+1,iFindWKN2-iFindWKN-1);
                    iFindWKN2 = sLine.find("\t",(iFindWKN = iFindWKN2) + 1);
                    std::string cKAG = sLine.substr(iFindWKN+1,iFindWKN2-iFindWKN-1);
#ifndef NOOUTPUT
                    cout << cUrl << endl << cName << endl << cWKN << endl << cKAG << endl;
#endif
                    fond* pF = AddFond(cWKN.c_str());
                    pF->STsURL = cUrl.c_str();
                    pF->STsName = cName.c_str();
                    pF->STsKag = cKAG.c_str();

                    iDownloadedFondCount++;
                }
            }
            cout << "Got " << iDownloadedFondCount << " and " << iFondCount << " left." << endl;
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
bool FondStorage::LoadContainerFile(const char* file)
{
    DataFile dfLoadData;
    if (dfLoadData.open(file) == true)
    {
        ITdblMAP itContainerData = dfLoadData.GetDataItBegin();
        for (;itContainerData != dfLoadData.GetDataItEnd();itContainerData++)
        {
            fond* pF = AddFond(itContainerData->first.c_str());
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
            pF->STiPers = atoi(dfLoadData.GetData(itContainerData,"Pers").c_str());
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
bool FondStorage::SaveContainerFile(const char* file)
{
    DataFile dfLoadData;
    std::map<std::string,fond*>::iterator itFSList;
    for (itFSList = begin();itFSList != end();itFSList++)
    {
        dfLoadData.AddData ((itFSList->second)->sWkn.c_str(),"SECU",(itFSList->second)->cSecu.c_str());
        dfLoadData.AddData ((itFSList->second)->sWkn.c_str(),"URL",(itFSList->second)->STsURL.c_str());
        dfLoadData.AddData ((itFSList->second)->sWkn.c_str(),"NAME",(itFSList->second)->STsName.c_str());
        dfLoadData.AddData ((itFSList->second)->sWkn.c_str(),"KAG",(itFSList->second)->STsKag.c_str());
        std::stringstream sstring;
        sstring << (itFSList->second)->STiPers;
        dfLoadData.AddData ((itFSList->second)->sWkn.c_str(),"Pers",sstring.str().c_str());
    }
    return dfLoadData.save(file);
}

int FondStorage::GetFilteredList(std::vector<std::string> & vList)
{
    std::map<std::string,fond*>::iterator itFSList;
    int iCount = 0;
    for (itFSList = begin();itFSList != end();itFSList++)
    {
        if ((itFSList->second)->STiPers > 100)
        {
            vList.push_back((itFSList->second)->sWkn);
            iCount++;
        }

    }
    return iCount;
}

