#ifndef httpFileHeader
#define httpFileHeader


#define LOWOUTPUT
#define NOOUTPUT




#ifdef linux
#include <sys/socket.h> // socket(), connect()
#include <sys/types.h>
#include <arpa/inet.h> // sockaddr_in
#include <netdb.h> // gethostbyname(), hostent
#include <errno.h> // errno
#include <unistd.h>
#else
#include <winsock2.h>
#endif

#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include <assert.h>
#include <fstream>
#include <stdexcept>

#include <math.h>


#include <QDate>
#include <QTextStream>

#include "kursunddatum.h"

#ifndef _CONSOLE
//#include "..\FondAnzeiger\StatusWindow.h"
//#define cout Cout
#endif // _CONSOLE

#ifdef linux
#define BYTE unsigned char
#endif
//using namespace std;

class wsock
{
private:
public:
#ifndef linux
  SOCKET s;
  SOCKADDR_IN addr;
#else
  int s;
  sockaddr_in addr;
#endif

  bool bError;
  int iStatus;


  wsock();
  ~wsock();
  int verbinden(const QString &cAddr, int iPort);
  void GetLine(QTextStream& line){
    for(char c; recv(s, &c, 1, 0) > 0; line << c)
      if(c == '\n')
        return;
    assert(false);
  }

  bool SendAll(const QByteArray &buf) ;
  bool SendAll(const QString &buf) ;
  int RecvAll(QByteArray &buf);
  int RecvPart(QByteArray &buf, int maxSize);

  //int RecvAll(QString &sData);
};


class http : public wsock
{
public:
  QString cAddr;
  QTextStream ssFondData;
  QString strFondData;


  //QString cSecu;
  QString sHttpData;
  int iLastStatusResponse;

  bool SendGetRequest(const QString cUrl);
  bool SendPostRequest(const QString cUrl, const QString cPost);

  QDate GetAuflageDatum(const QString cUrl);
  http();
  http(const char* cAddr);
  void SetAddr(const char* Addr){cAddr = Addr;return;}
  const QString GetAddr(){return cAddr;}

  int GetFondData(KursUndDatum &kud) ;

  int GetKagFondCount();

  int Empty() ;


  int DeleteUntil(const QString &, bool bAlwaysDO = true);
  int DeleteFrom(const QString &, bool bAlwaysDO = true);
  int DeleteEverywereEveryThingBetween(const QString &, const QString &);
  int ReplaceEverywereEveryThingBetween(const QString & , const QString & , const QString & );
  int DeleteEveryThing(const QString &);
  int ReplaceEveryThing(const QString & , const QString & );

  int ClipHttpCommand (const QString &cCommand, const QString &cName);
  int ReplaceCommWithoutNames (const QString &cCommand, const QString &cName, const QString &cRepl);


  ~http();
  int recv(QString &);
  int recv();
};


#if defined (_CONSOLE)

int MainHtml();
//int PrintChildNodes (HtmlNode&,bool = false);
#endif

struct DataElement
{
	BYTE* pPtr;
	size_t sSizeX;
	size_t sSizeY;
};


/*template <class Ta>
const Ta& oma (const Ta &a, const Ta &b)
{
return a < b ? b : a;
}*/

//QString& operator << (QString& OS, double& d);
QString& operator << (QString& OS, const double d);

QDate StrToDate(QString cDate);
//QDate MakedateofDMJ(int Day, int Month,int Year);
int GetDateOfToday();
int GetCountOfDaysPerYear(int iYear);
int GetCountOfDaysUntil(int iYear);
int GetCountOfDaysPerMonth(int iYear,int iMonth);
bool GetSchaltYear(int iYear);

#ifdef HttpHeader

#else
//extern QString request;
#endif


#endif  //httpFileHeader
