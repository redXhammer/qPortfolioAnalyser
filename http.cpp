#define HttpHeader
#include "http.h"

#define ANALYSINGHTTPRESPONSE

#include "QTextStream"
#include "QDebug"
#include "QFile"

//#define LOWOUTPUT
//#define _DEBUG
//using namespace std;


http::http()
{
    ssFondData.setString(&strFondData);
    cAddr = "www.ariva.de";
#ifndef LOWOUTPUT
    //qInfo() << "Creating http with Host: " << cAddr << endl;
#endif
    return;
}

http::http(const QString &cAddr2) {
		cAddr = cAddr2;
#ifndef LOWOUTPUT
    //qInfo() << "Creating http with Host: "  << cAddr << endl;
#endif
	return;
}

int http::DeleteFrom(const QString &cSearch, bool bAlwaysDO)
{
    int iReturnVal = 0;
#ifndef LOWOUTPUT
  qInfo() << "Finding " << cSearch << " : " ;
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
  qInfo() << iEndA << endl;
  qInfo() << "erasing" << endl;
#endif
    sHttpData.remove(iEndA);  // Add end ??
#ifndef LOWOUTPUT
  qInfo() << "Erased: "<< sHttpData.length() << endl;
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
  qInfo() << "Finding " << cSearch << " : " ;
#endif
    int iStartA = sHttpData.indexOf(cSearch)  ;
    if (iStartA == -1) {
	     if (bAlwaysDO == true) {
            qInfo() << "Cannot find " << cSearch << ". Current Data:";
#if defined (_DEBUG) && defined (_CONSOLE)
            fstream oFile("DeleteFrom.txt",ios::out);
            if(!oFile.good()) {
                qInfo() << "http::recv--Kann nicht schreiben-" << endl;
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
  qInfo() << iStartA << "erasing" << endl;
#endif
    sHttpData.remove(0,iStartA);
#ifndef LOWOUTPUT
  qInfo() << "Bytes 2: "<< sHttpData.length() << endl;
#endif
	}
    #if defined (_DEBUG) && defined (_CONSOLE) && defined (ANALYSINGHTTPRESPONSE)
        fstream oFile("ANALYSINGHTTPRESPONSE.txt",ios::out);
        if(!oFile.good()) {
            qInfo() << "http::recv--Kann nicht schreiben-" << endl;
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
            qInfo() << "http::recv--Kann nicht schreiben-" << endl;
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
            qInfo() << "http::recv--Kann nicht schreiben-" << endl;
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
            qInfo() << "http::recv--Kann nicht schreiben-" << endl;
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
            qInfo() << "http::recv--Kann nicht schreiben-" << endl;
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
        //if (iCoundNewComm == -1) {qInfo() << "Clip End failed! Could not find " << cCommC << endl; return -1;}

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
        if ( ssFondData.atEnd() ) return -1;
        cHelp = ssFondData.readLine();
    }
    while(cHelp.size() == 0);

    QTextStream ssLine(&cHelp);

    QString date;
    ssLine >> date;
    kud.cDatum = QDate::fromString(date);

    ssLine >> cHelp;
    ssLine >> cHelp;
    ssLine >> cHelp;

    ssLine >> kud.iKurs;

    ssLine >> cHelp;

    return 0;
}

int http::Empty()
{
    strFondData.clear();
    //ssFondData.setString(&strFondData);
    return 0;
}


bool http::SendGetRequest(const QString url)
{
  cUrl = url;
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
  qInfo() << request << endl;
#endif
  //qInfo() << cUrl << endl;
  if (!SendAll(request))
    {
        if (0 != verbinden(cAddr,443)) return false;
        if (!SendAll(request)) return false;
    }
    recv();
    if (iLastStatusResponse != 200) {
        qInfo() << " Got "<< iLastStatusResponse << " as Status Response";
        return false;
    }
    return true;
}


bool http::SendPostRequest(const QString url, const QString cPost)
{
  cUrl = url;
  QString request;
  QTextStream ssOut(&request);
  ssOut << "POST "
        << cUrl
        << " HTTP/1.1\r\n"
        << "Host: "
        << cAddr
        << "\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: "
        << cPost.length()
        << "\r\nConnection: close\r\n\r\n" //Connection: close Keep-Alive
        << cPost;

#ifndef LOWOUTPUT
  qInfo() << request;
#endif
  //qInfo() << cUrl << endl;
  if (!SendAll(request))
  {
    if (0 != verbinden(cAddr,443)) return false;
    if (!SendAll(request)) return false;
  }
  recv();
  if (iLastStatusResponse != 200)
  {
    qWarning() << " Got "<< iLastStatusResponse << " as Status Response";
    return false;
  }
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
  qInfo() << "Deleting http" << endl;
#endif
	return;
}

#define BufSize 1024
int http::recv(QString &sRecvData)
{
    QByteArray cBuf;
    int iLen = -1;
    int iLenGes  = -1;
    iLen = RecvAll(cBuf);

    sHttpData.clear();
    sHttpData = cBuf;

    DeleteUntil("HTTP/1.1 ");
    DeleteFrom(" ");

    bool bOK;
    iLastStatusResponse = sHttpData.toInt(&bOK);
    sHttpData.clear();

    if (iLen  == -1)
    {
        return -1;
    }
    else
    {
        iLenGes = iLen;
        while (iLen)
        {
            sRecvData += cBuf;
            iLen = RecvAll(cBuf);
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
        qInfo() << "http::recv--Kann nicht schreiben-" << endl;
    }
    oFile << sRecvData;
    oFile.close();

#endif
    //ofstream oFileHttpData("C:\\Http.txt");
    //oFileHttpData << sRecvData;
    //oFileHttpData.close();
#if defined (_DEBUG) && defined (_CONSOLE)
    if ((int)sRecvData.length() != iLenGes) qInfo() << "Recv Error: sRecvData.length(): " << sRecvData.length() << " und \"iLenGes:\" " << iLenGes << endl;
    else qInfo() << "Download Complete. Bytes: " << iLenGes << endl;;
#endif
    return iLenGes;
}

int http::recv()
{
    int iLenGes  = -1;
    bool chunked = false;
    const int noSizeGiven = -1;
    int size = noSizeGiven;
    int code = 100; // 100 = Continue
    QString Protokoll;
    QString firstLineData;
    QTextStream firstLine(&firstLineData); // Die erste Linie ist anders aufgebaut als der Rest

    while(code == 100)
    {
        GetLine(firstLine);
        firstLine >> Protokoll;
        firstLine >> code;
        //if(code == 100)
        //{
        //    GetLine(firstLine); // Leere Zeile nach Continue ignorieren
        //}
    }
    qInfo() << "Protokoll: " << Protokoll;
    iLastStatusResponse = code;

    if(code != 200)
    {
        //firstLine.trim(); // Leerzeichen nach dem Statuscode ignorieren
        QString msg;
        firstLine >> msg;
        qInfo() << "Error #" << code << " - " << msg << endl;
        return 0;
    }

    while(true)
    {
        QString sstreamData;
        QTextStream sstream(&sstreamData);
        GetLine(sstream);
        QString left; // Das was links steht
        sstream >> left;

        if(left.isEmpty()) // Header zu Ende?
        {
            break;
        }

        sstream.skipWhiteSpace(); // ignoriert Leerzeichen
        if(left == "Content-Length:")
        {
            sstream >> size;
        }
        if(left == "Transfer-Encoding:")
        {
            QString transferEncoding;
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

    QByteArray http_data;

    if(size != noSizeGiven) // Wenn die Größe über Content-length gegeben wurde
    {
        while(recvSize < size)
        {
            QByteArray data_shunk;
            if((bytesRecv = RecvAll(data_shunk)) <= 0)
            {
                assert(false);
            }
            recvSize += bytesRecv;
            http_data += data_shunk;
        }

        qInfo() << "Downloading... " << recvSize << "of" << size;
    }
    else
    {
        if(!chunked)
        {
            qInfo() << "Downloading... (Unknown Filesize)" << endl;
            while(bytesRecv != 0) // Wenn recv 0 zurück gibt, wurde die Verbindung beendet
            {
                QByteArray data_shunk;
                if((bytesRecv = RecvAll(data_shunk)) <= 0)
                {
                    assert(false);
                }
                http_data += data_shunk;
            }
        }
        else
        {
            qInfo() << "Downloading... (Chunked)" << endl;
            while(true)
            {
                bool bOK;
                QString sstreamData;
                QTextStream sstream(&sstreamData);

                int i = 0;
                while (!GetLine(sstream,true))
                {
                    if(i++>10)
                    {
                        qWarning() << "Got emplty line,quit";
                        return -1;
                    }
                    qWarning() << "Got emplty line";
                }

                chunkSize = sstreamData.toInt(&bOK,16); // Größe des nächsten Parts einlesen
                if (!bOK)
                {
                    qWarning() << "Failed reading chunk size:" << sstreamData;
                    break;
                }

                if(chunkSize <= 0)
                {
                    qInfo() << "Finished:" << chunkSize;
                    break;
                }
                qInfo() << "Downloading Part (" << chunkSize << " Bytes)... ";
                recvSize = 0; // Vor jeder Schleife wieder auf 0 setzen
                while(recvSize < chunkSize)
                {
                    QByteArray data_shunk;
                    bytesToRecv = chunkSize - recvSize;

                    bytesRecv = RecvPart(data_shunk,bytesToRecv);
                    if (bytesRecv < 0)
                    {
                      qWarning() << "Error reading from socket";
                      qInfo() << http_data;
                      return -1;
                    }

                    recvSize += bytesRecv;
                    http_data += data_shunk;
//                   qInfo() << "\r" << recvSize * 100 / chunkSize << "%" << flush;
                }
//                qInfo() << endl;
//                for(int i = 0; i < 2; ++i)
//                {
//                    char temp;
//                    ::recv(s, &temp, 1, 0); //Nessesary?????
//                }
            }
        }
    }
//    qInfo() << endl << "Finished!" << endl;
    sHttpData = QString::fromUtf8(http_data);



    //ReplaceEveryThing(">","<|||\r\n");
    //ReplaceEveryThing("<|||",">");

//#if defined (_DEBUG) && defined (_CONSOLE)
    qInfo() << "Writing response to:" << cUrl;


    QFile oFile(cUrl.remove('/').remove('\\'));
    if(!oFile.open(QIODevice::WriteOnly)) {
      qInfo() << "http::recv--Kann nicht schreiben-" << endl;
    }
    oFile.write(sHttpData.toUtf8());
    oFile.close();
//#endif

#if defined (_DEBUG) && defined (_CONSOLE) && !defined (NOOUTPUT)
  if (sHttpData.length() != iLenGes) qInfo() << "Recv Error: sRecvData.length(): " << sHttpData.length() << " und \"iLenGes:\" " << iLenGes << endl;
  else qInfo() << "Download Complete. Bytes: " << iLenGes << endl;;
#endif
	return iLenGes;
}




