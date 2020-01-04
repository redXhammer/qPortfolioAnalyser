

#ifndef FondFileHeader
#define FondFileHeader
#include "http.h"
#include <deque>
#include <map>

bool GetSecu(const std::string cSearch, std::string &sSecu, std::string & sURL);

class fond
{



private:


	http cHttp;
	DataFile WknFile;
	Datum DateOfWkn();




    int iStatus;    // 0 OK, 1 Daten Laden, 2 Fehler


//protected:
public:

	std::deque<KursUndDatum> dqKUD;
    std::deque<KursUndDatum> dqAusschuettung;

    //std::deque<KursUndDatum>::iterator itKUDlow;
	//std::deque<KursUndDatum>::iterator itKUDhigh;
	int iKUD;

    std::deque<KursUndDatum>::iterator itAussHigh;
    std::deque<KursUndDatum>::iterator itAussLow;

public:

    fond() : iKUD(0) {}
	fond(const char* sWkn);
	bool LoadAllData();
	int DownloadFondData(Datum cdMax = 0);
	int DownloadData(Datum, Datum, int iBoerseID = 6,bool bClean_Split = false, bool bClean_Payout = false ,bool bClean_Bezug = false); // 6 Xetra // 8 Fonds
	int LoadFondKurse();
	int LoadFondAuss();
	std::deque<KursUndDatum> DownloadFondAuss();
	int SaveFondData();
	double GetCurrentWert(const Datum&);
	double GetAussFactor(const Datum&);
	double GetTotalWert(const Datum &);
	double GetCurrentAverage(const Datum &cDatum, int iTage);
	void DownloadFundamental();
	const char* GetWknFileName();


	// Daten:

	std::string     STsURL;
    std::string     STsName;
    std::string     sWkn;
	std::string     sWknFileName;
	std::string     STsKag;
	std::string     STsISIN;
	Datum           STdAuflageDatum;
	std::string     STsKategorie;
	std::string     STsWaehrung;
	std::string     STsVolumen;
	double         STdAusgabeaufschlag;
	double         STdVerwaltungsgebuehr;
	double         STdDepotbankgebuehr;
	bool           STbSparplanfaehig;
	double         STdMinimalanlage;
	bool           STbAusschuettend;
	int             STiPers;

	std::string cSecu;




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
    std::map<int,std::string> GetKags();

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
Datum iDateToday;
//int iCountFonds = 0;
//fond* apFonds[50];
FondStorage fsAllFonds;
#else

extern Datum iDateToday;
//extern int iCountFonds;
//extern fond* apFonds[50];
extern FondStorage fsAllFonds;
#endif // FondFIle

#endif //FondFileHeader

