

#include "depot.h"
#include "qdebug.h"
#include "polynom.h"

#include "QFile"
#include "QString"
#include "QByteArray"

#define NOOUTPUT

#define MAXINT32 0x7FFFFFFF

TransActAnt::TransActAnt() :
    cDatum(),iAnteile(0), iType(2)
{}

TransActAnt::TransActAnt(const int& A) :
    cDatum(),iAnteile(A), iType(2)
{}


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


Depot::Depot() :
    cDateBegin()
{
    qInfo() << "Creating Deposet " << (long)this;
}

Depot::Depot(const Depot& A) :
    QList<DepotPos*> (A)
{
    *this = A;
    qInfo() << "Copying Depotset " << (long long)this;
}

Depot::Depot(const char* cFile) :
    cDateBegin()
{
    qInfo() << "Creating Deposet " << (long long)this << "from" << cFile;
    LoadDepotFile(cFile);
    return;
}

Depot::~Depot()
{
    qInfo() << "Deleating Depotset" << (long long)this;
}


bool Depot::LoadDepotFile (const char * cFile)
{

    QFile fsDepotFile(cFile);
    //QString sDepotData,sDepotData2;
    DepotPos* pcDepot;
    cDateBegin = QDate();

    if (!fsDepotFile.open(QIODevice::ReadOnly /* | QIODevice::Text */ ))
    {
        qWarning() << "DepotFile" << cFile << "could not being opend Opened";
        return false;
    }

    while(!fsDepotFile.atEnd())
    {
        QByteArray sDepotData = fsDepotFile.readLine();

        if (sDepotData.size() > 3) {
            if (sDepotData[2] == ':'){
                pcDepot = GetDepotClass(sDepotData);
                push_back(pcDepot);

            } else {
                int iType = 0;
                if (sDepotData[0] == '*')
                {
                    iType = 1;
                    sDepotData.remove(0,1);
                }

                size_t iPos;
                if ((iPos = sDepotData.lastIndexOf("St")) != -1)
                {
                    iType = 3;
                    sDepotData.remove(iPos,MAXINT32);
                }

                QByteArray sDepotData2 = sDepotData;
                sDepotData2.remove(0,8);
                sDepotData.remove(9,MAXINT32);
                if(pcDepot != 0)
                {
                    QDate dT = QDate::fromString(sDepotData,QString("dd.MM.yy"));
                    bool bOK;
                    double val = sDepotData2.toFloat(&bOK);
                    if (!bOK)
                    {
                        qWarning() << "Did not correctly process line";
                        continue;
                    }
                    if ((!cDateBegin.isValid()) || (cDateBegin > dT))
                    {
                        cDateBegin = dT;
                    }

                    if (iType==1) { // Wiederanlage (keinGrundbestand)
                        pcDepot->AddGewinn(dT,val);
                    }else if (iType==0){ // Neuanlage (Grundbestand)
                        pcDepot->AddTransAction(dT,val);
                    }else if (iType==3){ // Neuanlage (Stück)
                        pcDepot->AddItem(dT,val);
                    }
                } else {
                    qInfo() << "No Depot Defined" ;
                }
            }
        }
    }
#if 0 //ndef _CONSOLE
    QList<wndcre*>::iterator itHwnd;
    for (itHwnd = dqWindows.begin(); itHwnd != dqWindows.end() ;itHwnd++)
    {
        (*itHwnd)->ReloadData();
        cout << "Redrawing: " << (*itHwnd)->UpdateWindow() << endl;

    }
#endif
    //sName = cFile;
    return true;
}

DepotPos* Depot::operator [] (const int &iAt)
{
    QList<DepotPos*>::iterator itList = begin();
    for (;itList != end();itList++)
    {
        if ((*itList)->iDepotNr == iAt) return *itList;
    }
    return (DepotPos*)-1;
}

void Depot::Delete (const int &iAt)
{
    QList<DepotPos*>::iterator itList = begin();
    for (;itList != end();itList++)
    {
        if ((*itList)->iDepotNr == iAt)
        {
            erase(itList);
            return;
        }
    }
}

QString Depot::ShowDepotGesamtWert(const QDate &cDateOfW) {
    //double dWert7, dWertGes7 = 0;
    double dWert31, dWertGes31 = 0;
    double dWertStart, dWertGesStart = 0;
    double dWertGesammt = 0;

    QString strText;
    QTextStream streamText(&strText,QIODevice::WriteOnly);

    streamText << "Entwicklung vom " << cDateBegin.toString() << " bis zum " << cDateOfW.toString();

    streamText << '|' << qSetFieldWidth( 10 ) << "DepotPos" <<'|' << qSetFieldWidth( 10 ) << "Anteile" <<'|' << qSetFieldWidth( 10 ) << "Wert"  <<'|' <<
                  qSetFieldWidth( 10 ) << "+- Monat"  <<'|' << "+- Gesamt"  <<'|' << " Zins " << '|' ;
    QList<DepotPos*>::const_iterator idDepot;
    for (idDepot = begin(); idDepot != end();idDepot++)
    {
        dWertGesammt += (*idDepot)->GetCurrentDepotWert(cDateOfW);
        streamText << '|' << qSetFieldWidth( 10 ) << (*idDepot)->iDepotNr << '|';
        streamText << fixed;
        streamText << qSetRealNumberPrecision(4);
        streamText << qSetFieldWidth( 10 ) << (*idDepot)->GetCurrentFondAnt(cDateOfW).iAnteile <<'|';
        streamText << fixed;
        streamText << qSetRealNumberPrecision(2);
        streamText << qSetFieldWidth( 10 ) <<  (*idDepot)->GetCurrentDepotWert(cDateOfW) <<'|' ;
        dWert31 = (*idDepot)->GetCurrentTrend(cDateOfW.addDays(-31), cDateOfW);
        dWertGes31 += dWert31;
        streamText << qSetFieldWidth( 10 ) <<  dWert31 <<'|' ;
        dWertStart = (*idDepot)->GetCurrentTrend(cDateBegin,cDateOfW);
        dWertGesStart += dWertStart;
        streamText << qSetFieldWidth( 10 ) <<  dWertStart <<'|' ;
        streamText << qSetFieldWidth( 10 ) << (*idDepot)->GetDepotGrowth(cDateBegin,cDateOfW)  <<'|' ;
        streamText << qSetFieldWidth( 10 ) << (*idDepot)->GetFondGrowth (cDateBegin,cDateOfW)  <<'|' ;

    }
    streamText <<   qSetFieldWidth( 33 ) << dWertGesammt <<   /*qSetFieldWidth( 11 ) << dWertGes7 <<*/   qSetFieldWidth( 11 ) << dWertGes31  <<    qSetFieldWidth( 11 ) <<  dWertGesStart << qSetFieldWidth( 11 ) <<  CalculateZins(cDateBegin,cDateOfW) ;
    return strText;
}



double Depot::GetDepotSetWert(const QDate &cDat)
{
    //int iDat;
    double dMaxDWert = 0;
    QList<DepotPos*>::const_iterator itDepot = begin();
    for (; itDepot != end();itDepot++)
    {
        dMaxDWert += (*itDepot)->GetCurrentDepotWert(cDat);

    }
    return dMaxDWert;
}




double Depot::CalculateZins(const QDate& cdDatLow,const QDate &cdDatHigh)
{
    /* Berechne: GesammtZins

        */
    if (size() < 1) return 0;

    polynom pPolyGes;
    QList<DepotPos*>::iterator itDepot;


    for (itDepot = begin(); itDepot != end(); itDepot ++)
    {
        QList<TransActAnt>::iterator itTAA;
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

            peElement.exponent = itTAA->cDatum.daysTo(cdDatHigh); //cdDatHigh - itTAA->cDatum;
            pPolyGes.push_back(peElement);
        }
        peElement.coefficient = (*itDepot)->GetCurrentDepotWert (cdDatHigh) * (-1);
        peElement.exponent = 0;
        pPolyGes.push_back(peElement);
        peElement.coefficient = (*itDepot)->GetCurrentDepotWert (cdDatLow);
        peElement.exponent = cdDatLow.daysTo(cdDatHigh);
        pPolyGes.push_back(peElement);

    }
    double dZinsTag = pPolyGes.CalculateZero(1);
    double dZins = pow(dZinsTag,365);
    return (dZins-1) * 100;
}

double Depot::GetMaxDepotSetWert(QDate& cdDatLow, QDate& cdDatHigh)
{
    if ((cdDatLow == cdStart) && (cdEnd == cdDatHigh))
    {
        return dCurrentMaxDepotSetWert;
    }

    cdStart = cdDatLow; cdEnd = cdDatHigh;

    QDate cHelp = cdDatLow;
    double dMaxDSWert = 0;
    double dCurentDSWert;

    for (; cHelp <= cdDatHigh ;cHelp=cHelp.addDays(1))
    {
        dCurentDSWert = GetDepotSetWert(cHelp);
        if (dMaxDSWert < dCurentDSWert) dMaxDSWert = dCurentDSWert;
    }
    dCurrentMaxDepotSetWert = dMaxDSWert;
    return dMaxDSWert;
}

bool Depot::ResetMaxDates()
{
    cdEnd = QDate();
    cdStart = QDate();
    return true;
}


void Depot::ClearDepots()
{
    clear();
}

double Depot::GetCurrentDSTrend(const QDate &DStart, const QDate &DEnd)
{
    double dTrend = 0;
    QList<DepotPos*>::const_iterator itDepot = begin();
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
bool Depot::MakeAllSavingsPlan(double dValPerPeriod, QDate dStart , QDate dEnd, int iPeriod)
{
    if (size() <= 0) return false;
    dValPerPeriod /= size();
    QList<DepotPos*>::iterator itDepot;
    if (dStart.isNull())
        for (itDepot = begin();itDepot != end();itDepot++)
            if (dStart < (*itDepot)->pFond->dqKUD.back().cDatum)
                dStart = (*itDepot)->pFond->dqKUD.back().cDatum;


    if (iPeriod < 0)
    {
        switch (iPeriod)
        {
        case EVERYDAY:
            qWarning() << "Not Implemented Yet" << endl;
            break;
        case EVERYWEEK:
            qWarning() << "Not Implemented Yet" << endl;
            break;
        case EVERYMONTH:
            while (dStart.day() != 1){ dStart = dStart.addDays(1);}
            break;
        case EVERYYEAR:
            qWarning() << "Not Implemented Yet" << endl;
            break;
        }
        qInfo() << "Setting StartDate to " << dStart;

        if (dEnd.isNull()) dEnd = QDate::currentDate();
        cDateBegin = dStart;
        for (;dStart < dEnd; dStart = dStart.addDays(30))
        {
            for (itDepot = begin();itDepot != end();itDepot++)
                (*itDepot)->AddTransAction(dStart,dValPerPeriod);

        }


    }

    return true;
}

void Depot::MakeAutomaticTransaction(const QDate & dDate, double &dBargeld, double * pEinzahlung, double *pAuszahlung, QTextStream * pOS )
{
    /*if (pOS) *pOS << "Transaktionen am " << dDate
            << ")\tBargeld, Beginn:" << dBargeld
            << "\tWP: " << GetDepotSetWert(dDate)
            << endl << "Verkaufsorders:" << endl;


    QList<DepotPos*>::iterator itDeotPos;
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
    if (pOS) *pOS << "Transaktionen am " << dDate.toString()
                  << ")\tBargeld, Beginn:" << dBargeld
                  << "\tWP: " << GetDepotSetWert(dDate)
                  << endl << "Kaufsorders:" << endl;

    QList<DepotPos*>::iterator itDeotPos;
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
    QList<DepotPos*>::iterator itDepotPos;

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
