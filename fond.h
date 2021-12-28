

#ifndef FondFileHeader
#define FondFileHeader
#include "html.h"
#include "http.h"

#include <deque>
#include <map>

#include <QDate>

#include "datafile.h"

bool GetSecu(const QString cSearch, QString &sSecu, QString &sURL);

class fond
{
private:
    http cHttp;
	DataFile WknFile;
    QDate DateOfWkn();

    int iStatus;    // 0 OK, 1 Daten Laden, 2 Fehler


//protected:
public:

    QList<KursUndDatum> dqKUD;
    QList<KursUndDatum> dqAusschuettung;

	int iKUD;

    QList<KursUndDatum>::iterator itAussHigh;
    QList<KursUndDatum>::iterator itAussLow;

public:

    fond() : iKUD(0) {}
	fond(const char* sWkn);
	bool LoadAllData();
    int DownloadFondData(QDate cdMax = QDate());
    int DownloadData(QDate, QDate, int iBoerseID = 6,bool bClean_Split = false, bool bClean_Payout = false ,bool bClean_Bezug = false); // 6 Xetra // 8 Fonds
	int LoadFondKurse();
	int LoadFondAuss();
    QList<KursUndDatum> DownloadFondAuss();
	int SaveFondData();
    double GetCurrentWert(const QDate&);
    double GetAussFactor(const QDate&);
    double GetTotalWert(const QDate &);
    double GetCurrentAverage(const QDate &cDatum, int iTage);
	void DownloadFundamental();
    QString GetWknFileName();


	// Daten:

    QString     STsURL;
    QString     STsName;
    QString     sWkn;
    QString     sWknFileName;
    QString     STsKag;
    QString     STsISIN;
    QDate       STdAuflageDatum;
    QString     STsKategorie;
    QString     STsWaehrung;
    QString     STsVolumen;
    double      STdAusgabeaufschlag;
    double      STdVerwaltungsgebuehr;
    double      STdDepotbankgebuehr;
    bool        STbSparplanfaehig;
    double      STdMinimalanlage;
    bool        STbAusschuettend;
    int         STiPers;

    QString cSecu;




};

class FondStorage : public std::map<std::string,fond*>
{
public:
    FondStorage();
    ~FondStorage();

    fond* AddFond(const char* wkn);
    fond* GetFondClass(const char* wkn);

    bool LoadContainerFile(const char* file);
    bool UpdateLoadesFiles();
    bool SyncWithInternet();
    bool SaveContainerFile(const char* file);
    int GetFilteredList(std::vector<std::string> & vList);
    bool GetKagFonds(int iKagNr);
    QMap<int,QString> GetKags();

    int iStatus; // 0 OK, 1 Daten Laden, 2 Fehler

    //friend std::istream& operator >> (std::istream& OS, FondStorage& FS);

private:

protected:

};




class VectorStream : public std::streambuf
{
public:
        VectorStream(std::list<std::string>* pV,int iBuffersize=100);
        virtual ~VectorStream();
        bool ClearVect() {pVect->clear(); return true;}

private:
        char* pOutput;
        char* pInput;
        std::list<std::string>* pVect;
        std::list<std::string>::iterator itVect;
        int iBufSize;
protected:
        int_type overflow(int_type);
        int_type underflow();
        int_type sync();
};



//void* GetFondClass(char*);
//void DeleteFonds() ;


#ifdef FondFile
QDate iDateToday;
//int iCountFonds = 0;
//fond* apFonds[50];
FondStorage fsAllFonds;
#else

extern QDate iDateToday;
//extern int iCountFonds;
//extern fond* apFonds[50];
extern FondStorage fsAllFonds;
#endif // FondFIle

#endif //FondFileHeader

