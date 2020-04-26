#ifndef DepotFileHeader
#define DepotFileHeader
#include "fond.h"
#include <iomanip>
//#include <process.h>
#include <list>
#include <QDate>
#include "QString"

#define STARTATEARLIEST 0
#define STOPATLAST -1

#define EVERYDAY -1
#define EVERYWEEK -2
#define EVERYMONTH -3
#define EVERYYEAR -4



class TransActAnt
{
public:
    TransActAnt();
    TransActAnt(const int& A);
    //friend std::ostream& operator << (std::ostream& OS, TransActAnt& E);
private:
    QDate cDatum;
    double iAnteile;
    int iType; // 0 = Einzahlung
               // 1 = Gewinn
               // 2 = Datum Ungültig
};
//std::ostream& operator << (std::ostream& OS, TransActAnt& E);

struct DepotTable
{
    QDate cDatum;
	double dAnteile;
	double dFondWert;
};


class DepotPos
{
	friend class Depot;
protected:
	std::deque<TransActAnt> dqTAA;
public:
	DepotPos();
	DepotPos(const char*);
	DepotPos(const char* sWKN, int iDepotNr);
	~DepotPos();
    int AddTransAction(const QDate&,const double&);
    int AddItem(const QDate &, const double &);
    int AddGewinn(QDate, double);
    TransActAnt GetCurrentFondAnt(const QDate&);
    double GetCurrentFondWert(const QDate&);
    double GetCurrentDepotWert(const QDate&);
    double GetCurrentTrend(const QDate &Start, const QDate &End);
    double GetDepotGrowth(const QDate& cdDatLow,const QDate &cdDatHigh);
    double GetFondGrowth(const QDate& cdDatLow,const QDate &cdDatHigh);


private:
    QString sWKN;
	int iDepotNr;
	fond* pFond;

	std::deque<TransActAnt>::iterator itTAAlow;
	std::deque<TransActAnt>::iterator itTAAhigh;

};


class Depot  : public std::list<DepotPos*>
{
private:
    QDate cdStart;
    QDate cdEnd;
    double dCurrentMaxDepotSetWert;


public:
    Depot();
    Depot(const char*);
    Depot(const Depot& A);
    ~Depot();


    QDate cDateBegin;



    //QString sName;

	bool LoadDepotFile(const char*);
	bool AddDepot(const char* sWKN, int iDepotNr);
	bool AddDepot(DepotPos *);
	void Delete (const int &iAt);
	void ClearDepots();
    bool MakeAllSavingsPlan(double dValPerPeriod, QDate dStart = QDate(), QDate dEnd = QDate(), int iPeriod = EVERYMONTH);

    void MakeAutomaticTransaction(const QDate &, double &, double * pEinzahlung = NULL, double *pAuszahlung = NULL, std::ostream *pOS = NULL);
    void DeleteAllTransactions();

	int GetCount();
    double GetDepotSetWert(const QDate&);

    double CalculateZins(const QDate& ,const QDate &);


	DepotPos* operator [] (const int &);

	//void ShowDepotGesamtWertHeute(const Datum &);
    QString ShowDepotGesamtWert(const QDate &);
    double GetCurrentDSTrend(const QDate &, const QDate &);
	int iTableCount;

    double GetMaxDepotSetWert(QDate& , QDate &);
    bool ResetMaxDates();

};

/************** Sort Functions   ***********/
bool SortByDepotNr (DepotPos* ,DepotPos*);


DepotPos* GetDepotClass(const QByteArray &cData);
void DeleteDepots() ;
void InitDepots (void* pParam);


#ifdef DepotFile
//DepotPos* apDepot[50];
std::deque<DepotPos*> dqDepot;
//int iCountDepot;
#else
//extern DepotPos* apDepot[50];
//extern std::deque<DepotPos> dqDepot;
//extern int iCountDepot;
#endif






#endif

