#define HttpHeader
#include "http.h"

#define ANALYSINGHTTPRESPONSE

#include "QTextStream"
#include "QDebug"

//#define LOWOUTPUT
//#define _DEBUG
//using namespace std;


http::http()
{
	cAddr = "www.ariva.de";
#ifndef LOWOUTPUT
	//cout << "Creating http with Host: " << cAddr << endl;
#endif
	return;
}

http::http(const char* cAddr2) {
		cAddr = cAddr2;
#ifndef LOWOUTPUT
    //cout << "Creating http with Host: "  << cAddr << endl;
#endif
	return;
}

int http::DeleteFrom(const QString &cSearch, bool bAlwaysDO)
{
    int iReturnVal = 0;
#ifndef LOWOUTPUT
	cout << "Finding " << cSearch << " : " ;
#endif
    int iEndA = sHttpData.indexOf(cSearch);
    if (iEndA == -1)  {
        if (bAlwaysDO == true) {
            qInfo() << "Cannot find " << cSearch << ". Current Data:";

        #if defined (_DEBUG) && defined (_CONSOLE)
            fstream oFile("DeleteFrom.txt",ios::out);
            if(!oFile.good()) {
                qInfo() << "http::recv--Kann nicht schreiben-";
            }
            oFile << sHttpData;
            oFile.close();
        #endif

            getchar();
            return -1;
        }

        qInfo() << "Didnt Found " << cSearch;
        qInfo() << "and bAlwaysDO = False";
        iReturnVal = -1;
	} else {
#ifndef LOWOUTPUT
	cout << iEndA << endl;
	cout << "erasing" << endl;
#endif
    sHttpData.remove(iEndA);  // Add end ??
#ifndef LOWOUTPUT
	cout << "Erased: "<< sHttpData.length() << endl;
#endif
	}
    #if defined (_DEBUG) && defined (_CONSOLE) && defined (ANALYSINGHTTPRESPONSE)
        fstream oFile("ANALYSINGHTTPRESPONSE.txt",ios::out);
        if(!oFile.good()) {
            qInfo() << "http::recv--Kann nicht schreiben-";
        } else {
        oFile << sHttpData;
        oFile.close();
        }
        //getchar();
    #endif
	return iReturnVal;
}


int http::DeleteUntil(const QString &cSearch, bool bAlwaysDO)
{
    int iReturnVal = 0;
#ifndef LOWOUTPUT
	cout << "Finding " << cSearch << " : " ;
#endif
    int iStartA = sHttpData.indexOf(cSearch)  ;
    if (iStartA == -1) {
	     if (bAlwaysDO == true) {
            qInfo() << "Cannot find " << cSearch << ". Current Data:";
#if defined (_DEBUG) && defined (_CONSOLE)
            fstream oFile("DeleteFrom.txt",ios::out);
            if(!oFile.good()) {
                cout << "http::recv--Kann nicht schreiben-" << endl;
            }
            oFile << sHttpData;
            oFile.close();
#endif
            getchar();
            return -1;
	    }
        qInfo() << "Didnt Found " << cSearch;
        qInfo() << "and bAlwaysDO = False";
        iReturnVal = -1;
	} else {
    iStartA += cSearch.length();
#ifndef LOWOUTPUT
	cout << iStartA;
	cout << "erasing" << endl;
#endif
    sHttpData.remove(0,iStartA);
#ifndef LOWOUTPUT
	cout << "Bytes 2: "<< sHttpData.length() << endl;
#endif
	}
    #if defined (_DEBUG) && defined (_CONSOLE) && defined (ANALYSINGHTTPRESPONSE)
        fstream oFile("ANALYSINGHTTPRESPONSE.txt",ios::out);
        if(!oFile.good()) {
            cout << "http::recv--Kann nicht schreiben-" << endl;
        } else {
        oFile << sHttpData;
        oFile.close();
        }
        //getchar();
    #endif
	return iReturnVal;
}


int http::ReplaceEverywereEveryThingBetween(const QString &cSearch1, const QString &cSearch2, const QString &cReplace)
{
	int iFindPosA = -1, iLenA = 0;
    while (0<=(iFindPosA = sHttpData.indexOf(cSearch1)))
	{
        iLenA = sHttpData.indexOf(cSearch2,iFindPosA) - iFindPosA + cSearch2.length();
		sHttpData.replace(iFindPosA,iLenA,cReplace);
	}
    #if defined (_DEBUG) && defined (_CONSOLE) && defined (ANALYSINGHTTPRESPONSE)
        fstream oFile("ANALYSINGHTTPRESPONSE.txt",ios::out);
        if(!oFile.good()) {
            cout << "http::recv--Kann nicht schreiben-" << endl;
        } else {
        oFile << sHttpData;
        oFile.close();
        }
        //getchar();
    #endif
	return 0;
}

int http::DeleteEverywereEveryThingBetween(const QString &cSearch1, const QString &cSearch2)
{
	int iFindPosA = -1, iLenA = 0;
    while (0<=(iFindPosA = sHttpData.indexOf(cSearch1)))
	{
        iLenA = sHttpData.indexOf(cSearch2,iFindPosA) - iFindPosA + cSearch2.length();
		sHttpData.replace(iFindPosA,iLenA,"");
	}
    #if defined (_DEBUG) && defined (_CONSOLE) && defined (ANALYSINGHTTPRESPONSE)
        fstream oFile("ANALYSINGHTTPRESPONSE.txt",ios::out);
        if(!oFile.good()) {
            cout << "http::recv--Kann nicht schreiben-" << endl;
        } else {
        oFile << sHttpData;
        oFile.close();
        }
        //getchar();
    #endif
	return 0;
}

int http::DeleteEveryThing(const QString &cSearch1)
{
	int	iFindPosA = -1;
    while (0<=(iFindPosA = sHttpData.indexOf(cSearch1)))
	{
        sHttpData.replace(iFindPosA,cSearch1.length(),"");
	}
    #if defined (_DEBUG) && defined (_CONSOLE) && defined (ANALYSINGHTTPRESPONSE)
        fstream oFile("ANALYSINGHTTPRESPONSE.txt",ios::out);
        if(!oFile.good()) {
            cout << "http::recv--Kann nicht schreiben-" << endl;
        } else {
        oFile << sHttpData;
        oFile.close();
        }
        //getchar();
    #endif
	return 0;
}

int http::ReplaceEveryThing(const QString &cSearch1, const QString &cReplace)
{
    int	iFindPosA = -1, iLen = cSearch1.length();
    while (0<=(iFindPosA = sHttpData.indexOf(cSearch1)))
	{
		sHttpData.replace(iFindPosA,iLen,cReplace);
	}
    #if defined (_DEBUG) && defined (_CONSOLE) && defined (ANALYSINGHTTPRESPONSE)
        fstream oFile("ANALYSINGHTTPRESPONSE.txt",ios::out);
        if(!oFile.good()) {
            cout << "http::recv--Kann nicht schreiben-" << endl;
        } else {
        oFile << sHttpData;
        oFile.close();
        }
        //getchar();
    #endif
	return 0;
}



int http::ClipHttpCommand (const QString &cCommand, const QString &cName)
{
    if (cName.length()>0){
        QString cCommC = "<";
        cCommC += cCommand;

        int iFoundCommC1 = sHttpData.indexOf(cCommC);
        int iFoundCommC2 = sHttpData.indexOf(">",iFoundCommC1);
        int iFoundCommF = sHttpData.indexOf(cName,iFoundCommC1);
        if (iFoundCommC1 == -1) {
            qInfo() << "Clip failed! Could not find " << cCommC;
            return -1;
        }
        if (iFoundCommC2 == -1) {
            qInfo() << "Clip failed! Could not find " << ">";
            return -1;
        }
        if (iFoundCommF == -1) {
            qInfo() << "Clip failed! Could not find " << cName;
            return -1;
        }

        while (!((iFoundCommC1 < iFoundCommF)&&(iFoundCommF < iFoundCommC2)))
        {
            iFoundCommC1 = sHttpData.indexOf(cCommC,iFoundCommC1+1);
            iFoundCommC2 = sHttpData.indexOf(">",iFoundCommC1);
            iFoundCommF = sHttpData.indexOf(cName,iFoundCommC1);
            if (iFoundCommC1 == -1) {qInfo() << "Clip Loop failed! Could not find " << cCommC; return -1;}
            if (iFoundCommC2 == -1) {qInfo() << "Clip Loop failed! Could not find " << ">"; return -1;}
            if (iFoundCommF == -1) {qInfo() << "Clip Loop failed! Could not find " << cName; return -1;}
        }

        cCommC = "<";
        cCommC += cCommand;

        QString cCommEnd = "</";
        cCommEnd += cCommand;
        cCommEnd += ">";

        int iFoundCommEnd = sHttpData.indexOf(cCommEnd,iFoundCommC2);
        int iCoundNewComm = sHttpData.indexOf(cCommC,iFoundCommC2);
        if (iFoundCommEnd == -1) {qInfo() << "Clip End failed! Could not find " << cCommEnd; return -1;}
        //if (iCoundNewComm == -1) {cout << "Clip End failed! Could not find " << cCommC << endl; return -1;}

        while ((iFoundCommEnd > iCoundNewComm) && (iCoundNewComm != -1))
        {
            iFoundCommEnd = sHttpData.indexOf(cCommEnd,iCoundNewComm+1);
            iCoundNewComm = sHttpData.indexOf(cCommC,iCoundNewComm+1);
            if (iFoundCommEnd == -1) {qInfo() << "Clip End Loop failed! Could not find " << cCommEnd; return -1;}
            //if (iCoundNewComm == -1) {qInfo() << "Clip End Loop failed! Could not find " << cCommC; return -1;}
        }

        sHttpData.remove(iFoundCommEnd); //end
        sHttpData.remove(0,iFoundCommC2+1);
    } else {
        QString cCommC = "<";
        cCommC += cCommand;

        int iFoundCommC1 = sHttpData.indexOf(cCommC);
        int iFoundCommC2 = sHttpData.indexOf(">",iFoundCommC1);

        if (iFoundCommC1 == -1) {qInfo() << "Clip failed! Could not find " << cCommC; return -1;}
        if (iFoundCommC2 == -1) {qInfo() << "Clip failed! Could not find " << ">"; return -1;}


        while (!(iFoundCommC1 < iFoundCommC2))
        {
            iFoundCommC1 = sHttpData.indexOf(cCommC,iFoundCommC1+1);
            iFoundCommC2 = sHttpData.indexOf(">",iFoundCommC1);
            if (iFoundCommC1 == -1) {qInfo() << "Clip Loop failed! Could not find " << cCommC; return -1;}
            if (iFoundCommC2 == -1) {qInfo() << "Clip Loop failed! Could not find " << ">"; return -1;}
        }

        cCommC = "<";
        cCommC += cCommand;

        QString cCommEnd = "</";
        cCommEnd += cCommand;
        cCommEnd += ">";

        int iFoundCommEnd = sHttpData.indexOf(cCommEnd,iFoundCommC2);
        int iCoundNewComm = sHttpData.indexOf(cCommC,iFoundCommC2);
        if (iFoundCommEnd == -1) {qInfo() << "Clip End failed! Could not find " << cCommEnd; return -1;}
        //if (iCoundNewComm == -1) {qInfo() << "Clip End failed! Could not find " << cCommC; return -1;}

        while ((iFoundCommEnd > iCoundNewComm) && (iCoundNewComm != -1))
        {
            iFoundCommEnd = sHttpData.indexOf(cCommEnd,iCoundNewComm+1);
            iCoundNewComm = sHttpData.indexOf(cCommC,iCoundNewComm+1);
            if (iFoundCommEnd == -1) {qInfo() << "Clip End Loop failed! Could not find " << cCommEnd; return -1;}
            //if (iCoundNewComm == -1) {qInfo() << "Clip End Loop failed! Could not find " << cCommC; return -1;}
        }

        sHttpData.remove(iFoundCommEnd); //Add end
        sHttpData.remove(0,iFoundCommC2+1);
    }


    return 0;
}


int http::ReplaceCommWithoutNames (const QString &cCommand, const QString &cName, const QString &cRepl)
{
    if (cName.length()>0){
        QString cCommC = "<";
        cCommC += cCommand;
        QString cFindName = cName;
        cFindName += "=\"";

        int iFoundCommC1 = 0;
        int iFoundCommC2 = 0;
        int iFoundName;
        iFoundCommC1 = sHttpData.indexOf(cCommC,iFoundCommC1+1);
        iFoundCommC2 = sHttpData.indexOf(">",iFoundCommC1);
        while ((iFoundCommC1 != -1 )&&(iFoundCommC2 != -1))
        {
            iFoundName   = sHttpData.indexOf(cFindName,iFoundCommC1);
            if ((iFoundCommC1 < iFoundName)&&(iFoundName < iFoundCommC2))
            {
                iFoundName += cFindName.length();
                int iFindEnd = sHttpData.indexOf("\"",iFoundName);
                sHttpData.replace(iFindEnd,iFoundCommC2+1-iFindEnd,cRepl);
                sHttpData.replace(iFoundCommC1,iFoundName-iFoundCommC1,cRepl);
            }else{
                sHttpData.replace(iFoundCommC1,iFoundCommC2+1-iFoundCommC1,cRepl);
            }
            iFoundCommC1 = sHttpData.indexOf(cCommC,iFoundCommC1+1);
            iFoundCommC2 = sHttpData.indexOf(">",iFoundCommC1);
        }
    }
    return 0;
}


int http::GetKagFondCount()
{
    int iFondCountBegin = sHttpData.indexOf("<h2>") + 4;
    int iFondCountSize = sHttpData.indexOf("</h2>") - iFondCountBegin;
    QString cFondCount = sHttpData.mid(iFondCountBegin,iFondCountSize);
    iFondCountBegin = cFondCount.indexOf(" ");
    cFondCount.remove(iFondCountBegin); //add end
    return cFondCount.toInt();
}



int http::GetFondData(KursUndDatum &kud)
{
    QString cHelp;


    do
    {
        if ( ssFondData.eof() ) return -1;
        getline(ssFondData,cHelp);
    }
    while(cHelp.size() == 0);

    QTextStream ssLine(cHelp);

    ssLine >> kud.cDatum;

    ssLine >> cHelp;
    ssLine >> cHelp;
    ssLine >> cHelp;

    ssLine >> kud.iKurs;

    ssLine >> cHelp;
    //cout << kud.cDatum << " " << kud.iKurs << endl;



    return 0;
}

int http::Empty()
{
		//if ( ssFondData.eof() ) return -1;
			ssFondData.str("");
    return 0;
}


bool http::SendGetRequest(const QString cUrl)
{
    QString request;
	request = "GET ";
	request += cUrl;
	request += " HTTP/1.1\r\n";
	request += "Host: ";
	request += cAddr;
	request += "\r\nConnection: close\r\n";
	request += "User-Agent: Mozilla/5.0 (Windows; U; Windows NT 5.1; de; rv:1.9) Gecko/2008052906 Firefox/3.0\r\n";
	//request += "Accept-Encoding: gzip\r\n";
    request += "Accept-Charset: ISO-8859-1,UTF-8;q=0.7,*;q=0.7\r\n";
    request += "Cache-Control: no-cache\r\n";
    request += "Accept-Language: de,en;q=0.7,en-us;q=0.3\r\n\r\n";






#ifndef LOWOUTPUT
	cout << request << endl;
#endif
	//cout << cUrl << endl;
	if (!SendAll(request.c_str(),request.length()))
    {
        if (0 != verbinden(cAddr.c_str(),80)) return false;
        if (!SendAll(request.c_str(),request.length())) return false;
    }
    recv();
    if (iLastStatusResponse != 200) {
        cout << " Got "<< iLastStatusResponse << " as Status Response" << endl;
        return false;
    }
    return true;
}


bool http::SendPostRequest(const QString cUrl, const QString cPost)
{
    std::string request;
    std::ostringstream ssOut;
	ssOut << "POST "
	 << cUrl
	 << " HTTP/1.1\r\n"
	 << "Host: "
	 << cAddr
	 << "\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: "
	 << strlen(cPost)
	 << "\r\nConnection: close\r\n\r\n" //Connection: close Keep-Alive
	 << cPost;
	request = ssOut.str();
#ifndef LOWOUTPUT
	cout << request << endl;
#endif
	//cout << cUrl << endl;
		if (!SendAll(request.c_str(),request.length()))
    {
        if (0 != verbinden(cAddr.c_str(),80)) return false;
        if (!SendAll(request.c_str(),request.length())) return false;
    }
    recv();
    if (iLastStatusResponse != 200) {cout << " Got "<< iLastStatusResponse << " as Status Response"; return false;}
    return true;
}

/*
POST /wiki/Spezial:Search HTTP/1.1
Host: de.wikipedia.org
Content-Type: application/x-www-form-urlencoded
Content-Length: 24

search=Katzen&go=Artikel

*/







http::~http()
{
#ifndef LOWOUTPUT
	cout << "Deleting http" << endl;
#endif
	return;
}

#define BufSize 1024
int http::recv(QString &sRecvData)
{
	char cBuf[BufSize];
	int iLen = -1;
	int iLenGes  = -1;
	iLen = RecvBuf(cBuf,BufSize);

    sHttpData.clear();
	sHttpData = cBuf;

	DeleteUntil("HTTP/1.1 ");
	DeleteFrom(" ");

	iLastStatusResponse = atoi(sHttpData.c_str());
	sHttpData.clear();

	if (iLen  == -1)
	{
		return -1;

	} else {
		iLenGes = iLen;
		while (iLen)
		{
			sRecvData += cBuf;
			iLen = RecvBuf(cBuf,BufSize);
			if (iLen == -1) {
				return -1;
			}
			//cBuf[iLen] = 0;
			iLenGes += iLen;
		}
	}

#if defined (_DEBUG) && defined (_CONSOLE)
	fstream oFile("Http.txt",ios::out | ios::app);
	if(!oFile.good()) {
	cout << "http::recv--Kann nicht schreiben-" << endl;
	}
	oFile << sRecvData;
	oFile.close();

#endif
	//ofstream oFileHttpData("C:\\Http.txt");
	//oFileHttpData << sRecvData;
	//oFileHttpData.close();
#if defined (_DEBUG) && defined (_CONSOLE)
	if ((int)sRecvData.length() != iLenGes) cout << "Recv Error: sRecvData.length(): " << sRecvData.length() << " und \"iLenGes:\" " << iLenGes << endl;
	else cout << "Download Complete. Bytes: " << iLenGes << endl;;
#endif
	return iLenGes;
}

int http::recv()
{
	char cBuf[100];
	int iLenGes  = -1;
    bool chunked = false;
    const int noSizeGiven = -1;
    int size = noSizeGiven;
    int code = 100; // 100 = Continue
    QString Protokoll;
    QTextStream firstLine; // Die erste Linie ist anders aufgebaut als der Rest

    while(code == 100)
    {
        GetLine(firstLine);
        firstLine >> Protokoll;
        firstLine >> code;
        if(code == 100)
        {
            GetLine(firstLine); // Leere Zeile nach Continue ignorieren
        }
    }
    qInfo() << "Protokoll: " << Protokoll;
    iLastStatusResponse = code;

    if(code != 200)
    {
        firstLine.ignore(); // Leerzeichen nach dem Statuscode ignorieren
        QString msg;
        getline(firstLine, msg);
        qInfo() << "Error #" << code << " - " << msg << endl;
        return 0;
    }

    while(true)
    {
        stringstream sstream;
        GetLine(sstream);
        if(sstream.str() == "\r") // Header zu Ende?
        {
            break;
        }
        string left; // Das was links steht
        sstream >> left;
        sstream.ignore(); // ignoriert Leerzeichen
        if(left == "Content-Length:")
        {
            sstream >> size;
        }
        if(left == "Transfer-Encoding:")
        {
            string transferEncoding;
            sstream >> transferEncoding;
            if(transferEncoding == "chunked")
            {
                chunked = true;
            }
        }
    }

    int recvSize = 0; // Empfangene Bytes insgesamt
    int bytesRecv = -1; // Empfangene Bytes des letzten recv
    int chunkSize = -1;
    int bytesToRecv = -1;


    sHttpData.clear();
    ostringstream fout;

    if(size != noSizeGiven) // Wenn die Größe über Content-length gegeben wurde
    {
//        cout << "0%";
        while(recvSize < size)
        {
            if((bytesRecv = RecvBuf(cBuf,BufSize)) <= 0)
            {
                assert(false);
            }
            recvSize += bytesRecv;
            fout.write(cBuf, bytesRecv);
 //           cout << "\r" << recvSize * 100 / size << "%" << flush; // Mit \r springen wir an den Anfang der Zeile
        }
    }
    else
    {
        if(!chunked)
        {
 //           cout << "Downloading... (Unknown Filesize)" << endl;
            while(bytesRecv != 0) // Wenn recv 0 zurück gibt, wurde die Verbindung beendet
            {
                if((bytesRecv = RecvBuf(cBuf,BufSize)) < 0)
                {
                    assert(false);
                }
                fout.write(cBuf, bytesRecv);
            }
        }
        else
        {
//            cout << "Downloading... (Chunked)" << endl;
            while(true)
            {
                stringstream sstream;
                GetLine(sstream);
                chunkSize = -1;
                sstream >> hex >> chunkSize; // Größe des nächsten Parts einlesen
                if(chunkSize <= 0)
                {
                    break;
                }
//                cout << "Downloading Part (" << chunkSize << " Bytes)... " << endl;
                recvSize = 0; // Vor jeder Schleife wieder auf 0 setzen
                while(recvSize < chunkSize)
                {
                    bytesToRecv = chunkSize - recvSize;
                    if((bytesRecv = ::recv(s, cBuf, bytesToRecv > (int)sizeof(cBuf) ? sizeof(cBuf) : bytesToRecv, 0)) <= 0)
                    {
                        assert(false);
                    }
                    recvSize += bytesRecv;
                    fout.write(cBuf, bytesRecv);
 //                   cout << "\r" << recvSize * 100 / chunkSize << "%" << flush;
                }
//                cout << endl;
                for(int i = 0; i < 2; ++i)
                {
                    char temp;
                    ::recv(s, &temp, 1, 0);
                }
            }
        }
    }
//    cout << endl << "Finished!" << endl;
    sHttpData = fout.str();

    //ReplaceEveryThing(">","<|||\r\n");
    //ReplaceEveryThing("<|||",">");

#if defined (_DEBUG) && defined (_CONSOLE)
	fstream oFile("Http.txt",ios::out);
	if(!oFile.good()) {
	cout << "http::recv--Kann nicht schreiben-" << endl;
	}
	oFile << sHttpData;
	oFile.close();
#endif






#if defined (_DEBUG) && defined (_CONSOLE) && !defined (NOOUTPUT)
	if (sHttpData.length() != iLenGes) cout << "Recv Error: sRecvData.length(): " << sHttpData.length() << " und \"iLenGes:\" " << iLenGes << endl;
	else cout << "Download Complete. Bytes: " << iLenGes << endl;;
#endif
	return iLenGes;
}




