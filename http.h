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


#include "QDate"
#include "QTextStream"

#ifndef _CONSOLE
#include "..\FondAnzeiger\StatusWindow.h"
//#define cout Cout
#endif // _CONSOLE

#ifdef linux
#define BYTE unsigned char
#endif
//using namespace std;

class KursUndDatum
{
public:
    double iKurs;
    QDate cDatum;
    //friend std::ostream& operator << (std::ostream& OS, KursUndDatum& E);
    //friend std::istream& operator >> (std::istream& OS, KursUndDatum& KUD);
    KursUndDatum& operator = (const KursUndDatum&);
};

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
	int verbinden(const char* cAddr, int iPort);
    void GetLine(QTextStream& line){
	    for(char c; recv(s, &c, 1, 0) > 0; line << c)
            if(c == '\n')
                return;
        assert(false);
    }

	bool SendAll(const char*  buf, const int size) ;
	int RecvBuf(char* bData, int len);
    int RecvBuf(QString &sData);
};


class http : public wsock
{
private:
    //wsock cWsock;

public:
    QString cAddr;
    std::stringstream ssFondData;


	//std::string cSecu;
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



class HtmlNode
{
public:
    HtmlNode(HtmlNode*);
    HtmlNode();
    ~HtmlNode();


    HtmlNode *phnParentNode;
    std::vector<HtmlNode*> vectHtmlChildNode;
    std::string sName;
    std::vector<std::string> vectHtmlParams;

    std::vector<HtmlNode*> GetElementsByName(const char*);
    bool CheckSubNodes();
    bool List();
    HtmlNode* GetSubNode(const char*);
    std::string GetCurrentPath();


};

class html //: public http
{
    public:
    html();
    ~html();

    int open(const char*);

    HtmlNode hnEntryNode;
    //HtmlNode *pCurrentParentNode;
    HtmlNode * hCurrentNode;
    bool CheckTree();

    bool cdUp();
    bool cdDown(std::string,bool bList = true);
    bool cdTotalDir(std::string);
    void List();
    void Clear();
    int PrintChildNodes (HtmlNode& vectNode, bool bOptions);
    int PrintChildNodes (bool bOptions = false){return PrintChildNodes(*hCurrentNode, bOptions);};


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

typedef std::map<std::string , std::map<std::string,std::string> >::iterator ITdblMAP;
typedef std::map<std::string , std::map<std::string,std::string> >           DBLmap;
typedef std::map<std::string,std::string>::iterator                          ITmap;

class DataFile
{
private:
// Methoden



// Variablen
FILE * pFile;
BYTE * pbData;
long lSize;

DBLmap mData;
std::map<std::string , std::map<std::string,std::list<std::string> > > mVect;


public:
// Methoden
	bool open(const char*);
    bool save(const char*);
	DataFile() {pFile = NULL;};
	DataFile(char* cFile){open(cFile);};
	~DataFile(){if(pFile) {fclose(pFile);pFile=NULL;}};
	bool DropData(){mData.clear();mVect.clear();return true;};
    //bool

// mData zugriff
	bool AddData(const char* cDataClass,const  char* cName,const  char* Value);
	std::string GetData(const char* cDataClass, const char* cName);
	std::string GetData(ITdblMAP itDataClass, const char* cName);

// mVect zugriff
    std::list<std::string>& CreateVect (const char* cDataClass,const  char* cName);
    std::list<std::string>& GetVect (const char* cDataClass, const char* cName);

    //bool AddVectElement (char* cDataClass, char* cName, char* Value);
    //bool GetVectElement (char* cDataClass, char* cName, char* Value);

    std::map<std::string , std::map<std::string,std::string> >::iterator
        GetDataItBegin(){return mData.begin();};
    std::map<std::string , std::map<std::string,std::string> >::iterator
        GetDataItEnd(){return mData.end();};



// Variablen

};

/*template <class Ta>
const Ta& oma (const Ta &a, const Ta &b)
{
return a < b ? b : a;
}*/

//std::string& operator << (std::string& OS, double& d);
std::string& operator << (std::string& OS, const double d);

Datum StrToDate(std::string cDate);
Datum MakedateofDMJ(int Day, int Month,int Year);
int GetDateOfToday();
int GetCountOfDaysPerYear(int iYear);
int GetCountOfDaysUntil(int iYear);
int GetCountOfDaysPerMonth(int iYear,int iMonth);
bool GetSchaltYear(int iYear);

#ifdef HttpHeader

#else
//extern std::string request;
#endif


#endif  //httpFileHeader
