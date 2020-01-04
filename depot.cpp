

#include "depot.h"
#include "polynom.h"

#define NOOUTPUT
using std::cout;
using std::endl;

/*Depot::Depot()
{
	return;
}

Depot::~Depot()
{
	return;
}*/

/** @brief (one liner)
  *
  * (documentation goes here)
  */
/*Depot & Depot::operator=(const Depot&)
{
    copy(Depot.begin(),Depot.end(), begin());

}*/




Depot::Depot(const char* cFile)
{
	/*static ThreadData tdThreadData;
	static HANDLE hThread = 0;
	tdThreadData.cFile = (char*)cFile;
	tdThreadData.pdDepot = (Depot*)this;

	std::cout << "creating thread " << _beginthread(InitDepots, 0, &tdThreadData) << std::endl;
	return;*/
	LoadDepotFile(cFile);
	return;
}

/*void InitDepots (void* pParam)
{
	if (pParam == 0) {
		std::cout << "Keine Klasse angegeben! Ende" << std::endl;
		return;
	}
	ThreadData *ptdTD = (ThreadData *)pParam;

	ptdTD->pdDepot->LoadDepotFile(ptdTD->cFile);

	return;
}*/


bool Depot::LoadDepotFile (const char * cFile)
{

	std::fstream fsDepotFile;
	std::string sDepotData,sDepotData2;
	DepotPos* pcDepot;


	//iDCDepotCountMax = 0;

	fsDepotFile.open(cFile , std::ios::in); // "F:\\fonddaten\\depot.dpt"
	if (fsDepotFile.fail())
	{
		std::cout << "No DepotFile Opened" << std::endl;

		return false; // -1;
	}
    cDateBegin = -1;
	while(!fsDepotFile.eof())
	{
		std::getline(fsDepotFile,sDepotData);
		if (sDepotData.size() > 3) {
			if (sDepotData.c_str()[2] == ':'){
				pcDepot = GetDepotClass(sDepotData.c_str());
				push_back(pcDepot);
				//iDCDepotCountMax++;
			} else {
				int iType = 0;
				if (sDepotData.c_str()[0] == '*')
				{
					iType = 1;
					sDepotData.erase(0,1);
				}
				size_t iPos;
				if ((iPos = sDepotData.rfind("St")) != std::string::npos)
				{
				    iType = 3;
				    sDepotData.erase(iPos);
				}
				sDepotData2 = sDepotData;
				sDepotData2.erase(0,8);
				sDepotData.erase(9);
				if(pcDepot != 0)
				{
				    Datum dT = StrToDate(sDepotData);
				    if ((cDateBegin == -1) || (cDateBegin > dT))
				    {
                        cDateBegin = dT;
				    }
					if (iType==1) { // Wiederanlage (keinGrundbestand)
						pcDepot->AddGewinn(dT,atof(sDepotData2.c_str()));
					}else if (iType==0){ // Neuanlage (Grundbestand)
						pcDepot->AddTransAction(dT,atof(sDepotData2.c_str()));
					}else if (iType==3){ // Neuanlage (Stück)
						pcDepot->AddItem(dT,atof(sDepotData2.c_str()));
					}
				} else {
					std::cout << "No Depot Defined" << std::endl;
				}
			}
		}
	}
#ifndef _CONSOLE
	std::deque<wndcre*>::iterator itHwnd;
	for (itHwnd = dqWindows.begin(); itHwnd != dqWindows.end() ;itHwnd++)
	{
	    (*itHwnd)->ReloadData();
		cout << "Redrawing: " << (*itHwnd)->UpdateWindow() << endl;

	}
#endif
    sName = cFile;
	return true;
}

DepotPos* Depot::operator [] (const int &iAt)
{
    std::list<DepotPos*>::iterator itList = begin();
    for (;itList != end();itList++)
    {
        if ((*itList)->iDepotNr == iAt) return *itList;
    }
    return (DepotPos*)-1;
}

void Depot::Delete (const int &iAt)
{
    std::list<DepotPos*>::iterator itList = begin();
    for (;itList != end();itList++)
    {
        if ((*itList)->iDepotNr == iAt)
        {
            erase(itList);
            return;
        }
    }
}

void Depot::ShowDepotGesamtWert(const Datum &cDateOfW) {
	//double dWert7, dWertGes7 = 0;
	double dWert31, dWertGes31 = 0;
	double dWertStart, dWertGesStart = 0;
	double dWertGesammt = 0;

	std::cout << "Entwicklung vom " << cDateBegin << " bis zum " << (Datum)cDateOfW << std::endl;

	std::cout << std::endl<< '|' << std::setw( 10 ) << "DepotPos" <<'|' << std::setw( 10 ) << "Anteile" <<'|' << std::setw( 10 ) << "Wert"  <<'|' <<
		/*std::setw( 10 ) << "+- Woche"  <<'|' <<*/ std::setw( 10 ) << "+- Monat"  <<'|' << "+- Gesamt"  <<'|' << " Zins " << '|' << std::endl;
	std::list<DepotPos*>::const_iterator idDepot;
	for (idDepot = begin(); idDepot != end();idDepot++)
	{
		dWertGesammt += (*idDepot)->GetCurrentDepotWert(cDateOfW);
		std::cout << '|' << std::setw( 10 ) << (*idDepot)->iDepotNr << '|';
		std::cout.setf(std::ios::fixed);
		std::cout.precision(4);
		std::cout << std::setw( 10 ) << (*idDepot)->GetCurrentFondAnt(cDateOfW).iAnteile <<'|';
		std::cout.setf(std::ios::fixed);
		std::cout.precision(2);
		std::cout << std::setw( 10 ) <<  (*idDepot)->GetCurrentDepotWert(cDateOfW) <<'|' ;
		/*dWert7 = (*idDepot)->GetCurrentTrend(cDateOfW.iDatum - 7,cDateOfW);
		dWertGes7 += dWert7;
		std::cout << std::setw( 10 ) <<  dWert7 <<'|' ;*/
		dWert31 = (*idDepot)->GetCurrentTrend(cDateOfW.iDatum - 31,cDateOfW);
		dWertGes31 += dWert31;
		std::cout << std::setw( 10 ) <<  dWert31 <<'|' ;
		dWertStart = (*idDepot)->GetCurrentTrend(cDateBegin,cDateOfW);
		dWertGesStart += dWertStart;
		std::cout << std::setw( 10 ) <<  dWertStart <<'|' ;
		std::cout << std::setw( 10 ) << (*idDepot)->GetDepotGrowth(cDateBegin,cDateOfW)  <<'|' ;
		std::cout << std::setw( 10 ) << (*idDepot)->GetFondGrowth (cDateBegin,cDateOfW)  <<'|' << std::endl;

	}
	std::cout <<   std::setw( 33 ) << dWertGesammt <<   /*std::setw( 11 ) << dWertGes7 <<*/   std::setw( 11 ) << dWertGes31  <<    std::setw( 11 ) <<  dWertGesStart << std::setw( 11 ) <<  CalculateZins(cDateBegin,cDateOfW) << std::endl;
	return;
}



double Depot::GetDepotSetWert(const Datum &cDat)
{
	//int iDat;
	double dMaxDWert = 0;
	std::list<DepotPos*>::const_iterator itDepot = begin();
	for (; itDepot != end();itDepot++)
	{
		dMaxDWert += (*itDepot)->GetCurrentDepotWert(cDat);

	}
	return dMaxDWert;
}




double Depot::CalculateZins(const Datum& cdDatLow,const Datum &cdDatHigh)
{
/* Berechne: GesammtZins

	*/
	if (size() < 1) return 0;

	polynom pPolyGes;
	std::list<DepotPos*>::iterator itDepot;


	for (itDepot = begin(); itDepot != end(); itDepot ++)
	{
		std::deque<TransActAnt>::iterator itTAA;
		PolynomElement peElement;
#ifdef _console
		cout << "Size: " << dDepot.size() << endl;
#endif
		//for (itTAA = dDepot.begin();itTAA != dDepot.end(); itTAA++)
		//	cout << itTAA->cDatum << " " << itTAA->iAnteile << endl;

		for (itTAA = (*itDepot)->dqTAA.begin();itTAA != (*itDepot)->dqTAA.end();itTAA++)
		{
			if ((itTAA->iType == 1)||(itTAA->cDatum <= cdDatLow)||(itTAA->cDatum >= cdDatHigh)) continue;
			double dOldAnt = 0;
			if (itTAA != (*itDepot)->dqTAA.begin())
			{
				itTAA--;
				dOldAnt = itTAA->iAnteile;
				itTAA++;
			}
			peElement.coefficient = (itTAA->iAnteile - dOldAnt) * (*itDepot)->GetCurrentFondWert(itTAA->cDatum);

			peElement.exponent = cdDatHigh - itTAA->cDatum;
			pPolyGes.push_back(peElement);
		}
		peElement.coefficient = (*itDepot)->GetCurrentDepotWert (cdDatHigh) * (-1);
		peElement.exponent = 0;
		pPolyGes.push_back(peElement);
		peElement.coefficient = (*itDepot)->GetCurrentDepotWert (cdDatLow);
		peElement.exponent = cdDatHigh-cdDatLow;
		pPolyGes.push_back(peElement);

	}
	double dZinsTag = pPolyGes.CalculateZero(1);
	double dZins = pow(dZinsTag,365);
	return (dZins-1) * 100;
}

double Depot::GetMaxDepotSetWert(Datum& cdDatLow, Datum &cdDatHigh)
{
	if ((cdDatLow == cdStart) && (cdEnd == cdDatHigh))
	{
		return dCurrentMaxDepotSetWert;
	}

	cdStart = cdDatLow; cdEnd = cdDatHigh;

	Datum cHelp = cdDatLow;
	double dMaxDSWert = 0;
	double dCurentDSWert;

	for (; cHelp <= cdDatHigh ;cHelp++)
	{
		dCurentDSWert = GetDepotSetWert(cHelp);
		if (dMaxDSWert < dCurentDSWert) dMaxDSWert = dCurentDSWert;
	}
	dCurrentMaxDepotSetWert = dMaxDSWert;
	return dMaxDSWert;
}


void Depot::ClearDepots()
{clear();}

double Depot::GetCurrentDSTrend(const Datum &DStart, const Datum &DEnd)
{
	double dTrend = 0;
	std::list<DepotPos*>::const_iterator itDepot = begin();
	for (; itDepot != end();itDepot++)
	{
		dTrend += (*itDepot)->GetCurrentTrend(DStart,DEnd);
	}
	return dTrend;
}


bool Depot::AddDepot (const char* sWKN, int iDepotNr)
{
    DepotPos * pdDepot = new DepotPos (sWKN,iDepotNr);
    push_back(pdDepot);
    return true;
}

bool Depot::AddDepot (DepotPos *pdAdd)
{
	push_back(pdAdd);
	return true;
}

/** @brief (one liner)
  *
  * (documentation goes here)
  */
bool Depot::MakeAllSavingsPlan(double dValPerPeriod, Datum dStart , Datum dEnd, int iPeriod)
{
    if (size() <= 0) return false;
    dValPerPeriod /= size();
    std::list<DepotPos*>::iterator itDepot;
    if (dStart == STARTATEARLIEST)
        for (itDepot = begin();itDepot != end();itDepot++)
            if (dStart < (*itDepot)->pFond->dqKUD.back().cDatum)
                dStart = (*itDepot)->pFond->dqKUD.back().cDatum;

    cout << dStart << " and setting StartDate to ";
    if (iPeriod < 0)
    {
        switch (iPeriod)
        {
        case EVERYDAY:
            cout << "Not Implemented Yet" << endl;
            break;
        case EVERYWEEK:
            cout << "Not Implemented Yet" << endl;
            break;
        case EVERYMONTH:
            while (dStart.Day() != 1){ dStart++;}
            break;
        case EVERYYEAR:
            cout << "Not Implemented Yet" << endl;
            break;
        }
        cout << dStart << endl;

        if (dEnd == STOPATLAST) dEnd = iDateToday;
        cDateBegin = dStart;
        for (;dStart < dEnd; dStart+=30)
        {
            for (itDepot = begin();itDepot != end();itDepot++)
                (*itDepot)->AddTransAction(dStart,dValPerPeriod);

        }


    }

    return true;
}

void Depot::MakeAutomaticTransaction(const Datum & dDate, double &dBargeld, double * pEinzahlung, double *pAuszahlung, std::ostream * pOS )
{
    /*if (pOS) *pOS << "Transaktionen am " << dDate
            << ")\tBargeld, Beginn:" << dBargeld
            << "\tWP: " << GetDepotSetWert(dDate)
            << endl << "Verkaufsorders:" << endl;


    std::list<DepotPos*>::iterator itDeotPos;
    for (itDeotPos = begin(); itDeotPos != end(); itDeotPos++)
    {
        DepotPos * pDepPos = *itDeotPos;
        double dKursZuAverage = pDepPos->GetCurrentFondWert(dDate) / pDepPos->pFond->GetCurrentAverage (dDate,100);

        if (dKursZuAverage > 1.05 && pDepPos->GetCurrentDepotWert(dDate)>0)
        {
            double dVerkauf = pDepPos->GetCurrentDepotWert(dDate)*0.2;
            dBargeld += dVerkauf;
            if (pAuszahlung)
                *pAuszahlung += dVerkauf;
            pDepPos->AddTransAction(dDate,-dVerkauf);
            if (pOS) *pOS << "[" << pDepPos->iDepotNr << "]"
                << " von " << dVerkauf
                << "\t(" << (dKursZuAverage-1)*100
                << endl;
        }
    }

    if (pOS) *pOS << "Bargeld, Nach Verkauf:" << dBargeld  << ", Kaufsorders:" << endl;
    double dKaufGes = 0;
    for (itDeotPos = begin(); itDeotPos != end(); itDeotPos++)
    {
        DepotPos * pDepPos = *itDeotPos;
        double dKursZuAverage = pDepPos->GetCurrentFondWert(dDate) / pDepPos->pFond->GetCurrentAverage (dDate,100);

        if (dKursZuAverage < 1)
        {
            if (dKursZuAverage < 0.8)
                dKaufGes += 1.;
            else if (dKursZuAverage < 0.85)
                dKaufGes += 0.5;
            else if (dKursZuAverage < 0.9)
                dKaufGes += 0.4;
            else if (dKursZuAverage < 1)
                dKaufGes += 0.2;
        }
    }
    if (dKaufGes < 1. )dKaufGes = 1;

    double dBargeldKauf = dBargeld;

    for (itDeotPos = begin(); itDeotPos != end(); itDeotPos++)
    {
        DepotPos * pDepPos = *itDeotPos;
        double dKursZuAverage = pDepPos->GetCurrentFondWert(dDate) / pDepPos->pFond->GetCurrentAverage (dDate,100);

        if (dKursZuAverage < 1)
        {
            double dKauf;
            if (dKursZuAverage < 0.8)
                dKauf = dBargeldKauf/dKaufGes;
            else if (dKursZuAverage < 0.85)
                dKauf = dBargeldKauf/dKaufGes*0.5;
            else if (dKursZuAverage < 0.9)
                dKauf = dBargeldKauf/dKaufGes*0.4;
            else if (dKursZuAverage < 1)
                dKauf = dBargeldKauf/dKaufGes*0.2;


            dBargeld -= dKauf;
            if (pEinzahlung)
                *pEinzahlung += dKauf;
            pDepPos->AddTransAction(dDate,dKauf);
            if (pOS) *pOS << "[" << pDepPos->iDepotNr << "]"
                << " von " << dKauf
                << "\t(" << (dKursZuAverage-1)*100
                << endl;
        }
    }

    if (pOS) *pOS << ")\tBargeld, Nach Kauf:" << dBargeld << endl;
    */
    if (pOS) *pOS << "Transaktionen am " << dDate
            << ")\tBargeld, Beginn:" << dBargeld
            << "\tWP: " << GetDepotSetWert(dDate)
            << endl << "Kaufsorders:" << endl;

    std::list<DepotPos*>::iterator itDeotPos;
    double dBar = dBargeld;
    for (itDeotPos = begin(); itDeotPos != end(); itDeotPos++)
    {
        DepotPos * pDepPos = *itDeotPos;
        if (pDepPos->GetCurrentFondWert(dDate) == 0) continue;
        double dKauf = dBar/size();
        pDepPos->AddTransAction(dDate,dKauf);
        dBargeld -= dKauf;
        if (pOS) *pOS << "[" << pDepPos->iDepotNr << "]"
                << " von " << dKauf
                << endl;
    }
    if (pOS) *pOS << ")\tBargeld, Nach Kauf:" << dBargeld << endl;
}

void Depot::DeleteAllTransactions()
{
    std::list<DepotPos*>::iterator itDepotPos;

    for (itDepotPos = begin(); itDepotPos!= end(); itDepotPos++)
    {
        (*itDepotPos)->dqTAA.clear();
    }



}




bool SortByDepotNr(DepotPos* pdA, DepotPos*pdB)
{
    if (pdA->iDepotNr < pdB->iDepotNr ) return true;
    return false;
}
