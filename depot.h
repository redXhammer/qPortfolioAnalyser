#ifndef DepotFileHeader
#define DepotFileHeader
#include "fond.h"
#include <iomanip>
//#include <process.h>
#include <list>

#define STARTATEARLIEST 0
#define STOPATLAST -1

#define EVERYDAY -1
#define EVERYWEEK -2
#define EVERYMONTH -3
#define EVERYYEAR -4



class TransActAnt
{
public:
	Datum cDatum;
	double iAnteile;
	int iType; // 0 = Einzahlung
			   // 1 = Gewinn
				// 2 = Datum Ungültig
	TransActAnt(const int& A) {cDatum = 0; iAnteile = A; iType = 2;}
	TransActAnt() {cDatum = 0; iAnteile = 0; iType = 2;}

	friend std::ostream& operator << (std::ostream& OS, TransActAnt& E);
};
std::ostream& operator << (std::ostream& OS, TransActAnt& E);

struct DepotTable
{
	Datum cDatum;
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
	int AddTransAction(const Datum&,const double&);
	int AddItem(const Datum &, const double &);
	int AddGewinn(Datum, double);
	TransActAnt GetCurrentFondAnt(const Datum&);
	double GetCurrentFondWert(const Datum&);
	double GetCurrentDepotWert(const Datum&);
	double GetCurrentTrend(const Datum &Start, const Datum &End);
	double GetDepotGrowth(const Datum& cdDatLow,const Datum &cdDatHigh);
	double GetFondGrowth(const Datum& cdDatLow,const Datum &cdDatHigh);
	std::string sWKN;
	int iDepotNr;
	fond* pFond;
	//TransActAnt* pTAA;
	//int iCountTAA;
	std::deque<TransActAnt>::iterator itTAAlow;
	std::deque<TransActAnt>::iterator itTAAhigh;


	//DepotTable* pDTtable;
	//int iDTtableCount;
	//int GetTable(Datum& DStart, Datum &DEnd);
};


class Depot  : public std::list<DepotPos*>
{
protected:
	Datum cdStart, cdEnd; // Für GetDepotSetWert
	double dCurrentMaxDepotSetWert; // Für GetDepotSetWert


public:
	Depot() {
		cDateBegin = -1;
		std::cout << "Creating Deposet " << (long)this<< std::endl;
	}
	~Depot(){std::cout << "Deleating Depotset" << (long)this<< std::endl;}

	Depot(const Depot& A) : std::list<DepotPos*> (A)
	{
	    *this = A;
        std::cout << "Copying Depotset " << (long)this<< std::endl;
	}

	//Depot& operator= (const Depot&);

	Datum cDateBegin;

	Depot(const char*);

	std::string sName;

	bool LoadDepotFile(const char*);
	bool AddDepot(const char* sWKN, int iDepotNr);
	bool AddDepot(DepotPos *);
	void Delete (const int &iAt);
	void ClearDepots();
	bool MakeAllSavingsPlan(double dValPerPeriod, Datum dStart = STARTATEARLIEST, Datum dEnd = STOPATLAST, int iPeriod = EVERYMONTH);

    void MakeAutomaticTransaction(const Datum &, double &, double * pEinzahlung = NULL, double *pAuszahlung = NULL, std::ostream *pOS = NULL);
    void DeleteAllTransactions();

	int GetCount();
	double GetDepotSetWert(const Datum&);

	double CalculateZins(const Datum& ,const Datum &);


	DepotPos* operator [] (const int &);

	//void ShowDepotGesamtWertHeute(const Datum &);
	void ShowDepotGesamtWert(const Datum &);
	double GetCurrentDSTrend(const Datum &, const Datum &);
	int iTableCount;
#ifndef _CONSOLE
	std::deque<wndcre*> dqWindows;
#endif
	//Datum D1, D11, D2, D22;
	/*double	dMaxAnt,dMaxAnt2;
	double	dMaxFWert,dMaxFWert2;
	double	dMaxDWert,dMaxDWert2;*/
	double GetMaxDepotSetWert(Datum& , Datum &);
    bool ResetMaxDates()
    {
        cdEnd = 0;
        cdStart = 0;
        return true;
    }

};

struct ThreadData
{
	Depot* pdDepot;
	char* cFile;
};

/************** Sort Functions   ***********/

bool SortByDepotNr (DepotPos* ,DepotPos*);


DepotPos* GetDepotClass(const char* cData);
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

