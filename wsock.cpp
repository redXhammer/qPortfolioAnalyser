#include "http.h"

#define WStatDisconected  0
#define WStatConected  1

#define LOWOUTPUT

using namespace std;

bool bWsaStarted = false;
long rc;


wsock::wsock()
{
	iStatus = WStatDisconected;
	s = 0;
	return;
}

//======================================================================================

int wsock::verbinden(const char* cAddr, int iPort)
{
    if (iStatus == WStatConected){
#ifndef LOWOUTPUT
		cout << "Still Connected, Closing" << endl;
#endif
		iStatus = WStatDisconected;
#ifdef linux
        close(s); // Verbindung beenden
#else
        closesocket(s); // Windows-Variante
#endif
    }
	bError = true;
#ifndef LOWOUTPUT
	cout << "Creating wsock" << endl;
#endif
#ifndef linux
	WSADATA w;
    if(int result = WSAStartup(MAKEWORD(2,2), &w) != 0)
    {
        cout << "Winsock 2 konnte nicht gestartet werden! Error #" << result << endl;
        return -1;
    }
#endif
	s=socket(AF_INET,SOCK_STREAM,0);
	if(s==-1)
	{
#ifndef linux
		cout <<"Fehler: Der Socket konnte nicht erstellt werden, fehler code:" << WSAGetLastError() << endl;
#else
        cout <<"Fehler: Der Socket konnte nicht erstellt werden!" << endl;
#endif
		return -1;
	}else{
#ifndef LOWOUTPUT
		cout << "Socket erstellt!" << endl;
#endif
	}

	memset(&addr,0,sizeof(sockaddr_in)); // zuerst alles auf 0 setzten
	addr.sin_family=AF_INET;
	addr.sin_port=htons(iPort); // wir verwenden mal port 12345
	addr.sin_addr.s_addr=GetHost(cAddr); // zielrechner ist unser eigener

	rc = connect(s,(sockaddr*)&addr,sizeof(sockaddr));

	if(rc==-1)
	{
#ifndef linux
		cout << "Fehler: connect gescheitert, fehler code:" << WSAGetLastError() << endl;
#else
        cout << "Fehler: connect gescheitert" << endl;
#endif

#ifdef linux
        close(s); // Verbindung beenden
#else
        closesocket(s); // Windows-Variante
#endif

		return -1;
#ifndef LOWOUTPUT
	}else{
		cout << "Verbunden mit " << cAddr;
#endif
	}
	iStatus = WStatConected;
	bError = false;
	return 0;
}
//==============================================================================

ulong wsock::GetHost(const char* cAddr)
{

	ulong uRet;
	unsigned long ip;
	hostent* he;

	if(cAddr==NULL)
	{
		cout << "No Address" << endl;
		return 0;
	}

	ip=inet_addr(cAddr);
	if(ip!=INADDR_NONE)

	{
#ifndef LOWOUTPUT
		cout << "IP" << endl;
#endif
		return ip;
	} else {
#ifndef LOWOUTPUT
		cout << "Url" << endl;
#endif
		he=gethostbyname(cAddr);
		if(he==NULL)
		{
			cout << "he==NULL" << endl;
			return 0;
		}else{

			memcpy(&uRet,he->h_addr_list[0],4);
			return uRet;
		}
	}
}




//===========================================================================


wsock::~wsock()
{
#ifndef LOWOUTPUT
	cout << "Deleting wsock" << endl;
#endif
	iStatus = WStatDisconected;
	if (s)
#ifdef linux
    close(s); // Verbindung beenden
#else
    closesocket(s); // Windows-Variante
#endif
	return;
}

//===========================================================================

bool wsock::SendAll(const char* buf, const int size)
{
    if (iStatus == WStatConected) {
		int bytesSent = 0; // Anzahl Bytes die wir bereits vom Buffer gesendet haben
		int iBS = 0;
		do
		{
			if ((iBS = send(s, buf + bytesSent, size - bytesSent, 0)) > 0)
				bytesSent += iBS;
			else
			{
#ifndef linux
				cout << "Sent Failed" << WSAGetLastError() << endl;
#else
                cout << "Sent Failed" << endl;
#endif
				iStatus = WStatDisconected;
				return false;
			}
		} while(bytesSent < size);
	} else {
#ifndef NOOUTPUT
		cout << "Not Connected:" << endl;
#endif
		return false;
	}
	return true;
}

//===========================================================================

int wsock::RecvBuf(char* bData, int len)
{
    if (iStatus == WStatConected) {
		len--;
		int iSent = recv(s, bData, len, 0);
		/*if (iSent == WSAEWOULDBLOCK)
		{
			cout << "Empty" << WSAGetLastError() << " : " << WSAEWOULDBLOCK << endl;
			bData[iSent] = 0;
			return 0;
		}
		else if (iSent > 0)*/
		if (iSent > 0)
		{
			bData[iSent] = 0;
			return iSent;
		}
		else if (iSent == 0)
		{
#ifndef LOWOUTPUT
			cout << "Connection Close" << endl;
#endif
#ifdef linux
            close(s); // Verbindung beenden
#else
            closesocket(s); // Windows-Variante
#endif
            iStatus = WStatDisconected;
			bData[iSent] = 0;
			return iSent;
		}
		else if (iSent < 0)
		{
#ifndef linux
			cout << "Error in Sent. Code : " << WSAGetLastError() << " : " << WSAEWOULDBLOCK << endl;
#else
            cout << "Error in Sent." << endl;
#endif
			return -1;
		}
		return -1;
	} else {
#ifndef NOOUTPUT
		cout << "Not Connected" << endl;
#endif
		return -1;
	}
}

//===========================================================================

int wsock::RecvBuf(string &sData) {
#define BLOCKSIZE 1000
	char bData[BLOCKSIZE];
	int iNewLen, iGesLen = 0;

	while ((iNewLen = RecvBuf((char *)bData, BLOCKSIZE)) != 0)
    {
        sData += bData;
        iGesLen += iNewLen;
    }
	return iGesLen;

}
