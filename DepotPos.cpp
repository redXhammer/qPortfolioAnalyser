#define DepotFile
#include "depot.h"
#include "polynom.h"
#include "QDebug"

#define NOOUTPUT

QDebug& operator << (QDebug& OS, const TransActAnt& E)
{
    OS << E.cDatum << " Anteile: " << E.iAnteile <<  " Typ: "  << E.iType;
    return OS;
}




DepotPos::DepotPos()
{
    itTAAlow = dqTAA.begin();
    itTAAhigh = dqTAA.begin();
#ifndef NOOUTPUT
    std::cout << "Geht nicht !!" << endl;
#endif

#ifndef NOOUTPUT
    cout << "Adresse: " << this << endl;
#endif
    return;
}

DepotPos::DepotPos(const QString cData)
{
#ifndef NOOUTPUT
    std::cout << "Adresse: " << this << endl;
#endif
    bool bOK;
    iDepotNr = cData.mid(0,2).toInt(&bOK);
    if (!bOK)
    {
        qWarning() << "Did not got Depot Nr from:" << cData;
    }
    sWKN = cData.mid(3);
#ifndef NOOUTPUT
    std::cout << "Creating: Depot: " << iDepotNr << " with WKN: " << sWKN << endl;
#endif
    // TODO (User#1#): GetFond
    pFond = fsAllFonds.GetFondClass(sWKN);
    itTAAlow = dqTAA.begin();
    itTAAhigh = dqTAA.begin();
}

DepotPos::DepotPos(const QString sWKN, int iDepotNr_)
{
#ifndef NOOUTPUT
    std::cout << "Adresse: " << this << endl;
    std::cout << "Creating: Depot: " << iDepotNr_ << " with WKN: " << sWKN << endl;
#endif
    // TODO (User#1#): GetFond
    iDepotNr = iDepotNr_;
    pFond = fsAllFonds.GetFondClass(sWKN);
    itTAAlow = dqTAA.begin();
    itTAAhigh = dqTAA.begin();
}

DepotPos::~DepotPos()
{
    std::cout << "Deleting Depot: " << (long) this << std::endl;
    //if ( pDTtable) delete pDTtable;
}


int DepotPos::AddTransAction(const QDate &cDatum, const double &lEuro)
{
    TransActAnt taaTransActNew;
#ifndef NOOUTPUT
    std::cout << endl << "Adding Transaction on " << cDatum << " with " << lEuro << " Euro." << endl;
    std::cout << "Kurswert: " << pFond->GetCurrentWert(cDatum) / pFond->GetAussFactor(cDatum); << endl;
#endif
    taaTransActNew.cDatum=cDatum;
    if (dqTAA.size() < 1) {
        taaTransActNew.iAnteile = lEuro / pFond->GetCurrentWert(cDatum) * pFond->GetAussFactor(cDatum);
    } else {
        QList<TransActAnt>::iterator itTAA = dqTAA.end();
        itTAA--;
        taaTransActNew.iAnteile = itTAA->iAnteile + lEuro / pFond->GetCurrentWert(cDatum) * pFond->GetAussFactor(cDatum);
    }
    taaTransActNew.iType = 0;
    dqTAA.push_back(taaTransActNew);
    itTAAlow = dqTAA.begin();
    itTAAhigh = dqTAA.begin();
    itTAAhigh++;
#ifndef NOOUTPUT
    std::cout.setf(ios::fixed);
    std::cout.precision(4);
    std::cout << "Anteile: " << pTAA[iCountTAA - 1].iAnteile << " Ergeben einen Wert von: " << GetCurrentDepotWert(cDatum) << " Euro"  << endl;
#endif
    return 0;
}

int DepotPos::AddItem(const QDate &cDatum, const double &lStk)
{
    TransActAnt taaTransActNew;
#ifndef NOOUTPUT
    std::cout << endl << "Adding Transaction on " << cDatum << " with " << lEuro << " Euro." << endl;
    std::cout << "Kurswert: " << pFond->GetCurrentWert(cDatum) / pFond->GetAussFactor(cDatum); << endl;
#endif
    taaTransActNew.cDatum=cDatum;
    if (dqTAA.size() < 1) {
        taaTransActNew.iAnteile = lStk;
    } else {
        QList<TransActAnt>::iterator itTAA = dqTAA.end();
        itTAA--;
        taaTransActNew.iAnteile = itTAA->iAnteile + lStk;
    }
    taaTransActNew.iType = 0;
    dqTAA.push_back(taaTransActNew);
    itTAAlow = dqTAA.begin();
    itTAAhigh = dqTAA.begin();
    itTAAhigh++;
#ifndef NOOUTPUT
    std::cout.setf(ios::fixed);
    std::cout.precision(4);
    std::cout << "Anteile: " << pTAA[iCountTAA - 1].iAnteile << " Ergeben einen Wert von: " << GetCurrentDepotWert(cDatum) << " Euro"  << endl;
#endif
    return 0;
}


int DepotPos::AddGewinn(const QDate &cDatum, double lEuro)
{
    TransActAnt taaTransActNew;
#ifndef NOOUTPUT
    cout << endl << "Adding Transaction on " << cDatum << " with " << lEuro << " Euro." << endl;
    cout << "Kurswert: " << pFond->GetCurrentWert(cDatum) / pFond->GetAussFactor(cDatum)<< endl;
#endif
    taaTransActNew.cDatum=cDatum;
    if (dqTAA.size() < 1) {
        taaTransActNew.iAnteile = lEuro / pFond->GetCurrentWert(cDatum) * pFond->GetAussFactor(cDatum);
    } else {
        QList<TransActAnt>::iterator itTAA = dqTAA.end();
        itTAA--;
        taaTransActNew.iAnteile = itTAA->iAnteile + lEuro / pFond->GetCurrentWert(cDatum) * pFond->GetAussFactor(cDatum);
    }
    taaTransActNew.iType = 1;
    dqTAA.push_back(taaTransActNew);
    itTAAlow = dqTAA.begin();
    itTAAhigh = dqTAA.begin();
    itTAAhigh++;
#ifndef NOOUTPUT
    cout.setf(ios::fixed);
    cout.precision(4);
    cout << "Anteile: " << pTAA[iCountTAA - 1].iAnteile << " Ergeben einen Wert von: " << GetCurrentDepotWert(cDatum) << " Euro"  << endl;
#endif
    return 0;
}


TransActAnt DepotPos::GetCurrentFondAnt(const QDate &cDatum)
{
    if (dqTAA.size() == 0)
    {
        TransActAnt taaOne;
        taaOne.iAnteile = 0;
        taaOne.cDatum = QDate();
        return taaOne;
    }
    if (dqTAA.size() == 1)
    {
        if (itTAAlow->cDatum > cDatum) return 0;
        return *itTAAlow;
    }

    if (dqTAA.begin()->cDatum > cDatum)
        return 0;
    if (dqTAA.rbegin()->cDatum < cDatum)
        return *dqTAA.rbegin();


    while ((*itTAAlow).cDatum >= cDatum)
    {
        itTAAlow--;
        itTAAhigh--;
        if (itTAAhigh == dqTAA.begin())
        {
            itTAAhigh++;
            itTAAlow++;
            break;
        }
    }

    while ((cDatum >= itTAAhigh->cDatum))
    {
        itTAAlow++;
        itTAAhigh++;
        if (itTAAhigh == dqTAA.end())
        {
            itTAAhigh--;
            itTAAlow--;
            break;
        }
    }


    if (cDatum >= itTAAhigh->cDatum)
    {
        //cout << itTAAhigh->cDatum << " Anteile: High " << itTAAhigh->iAnteile << endl;
        return *itTAAhigh;
    }
    //cout << itTAAlow->cDatum << " Anteile: " << itTAAlow->iAnteile << endl;
    return *itTAAlow;
}

double DepotPos::GetCurrentFondWert(const QDate &cDatum)
{
    return pFond->GetTotalWert(cDatum);
}

double DepotPos::GetCurrentDepotWert(const QDate &cDatum)
{
    //cout << endl << "Getting for " << this << " = " <<  sWKN << " Date: " << (Datum)cDatum << " Wert: " << GetCurrentFondWert(cDatum) * GetCurrentFondAnt(cDatum) << endl;
    return GetCurrentFondWert(cDatum) * GetCurrentFondAnt(cDatum).iAnteile;
}





DepotPos* GetDepotClass(const QString &cData) {
    //int iFondFound = -1;


    DepotPos * pdDepot = new DepotPos (cData);
    if (pdDepot == 0) {
#ifndef NOOUTPUT
        cout << "Fehler Depotzuweisung" << endl;
        return 0;
#endif
    }
    dqDepot.push_back(pdDepot) ;
    return pdDepot;
}

void DeleteDepots()
{
    QList<DepotPos*>::iterator itDepots;
    for (itDepots = dqDepot.begin(); itDepots != dqDepot.end(); itDepots++)
    {
        delete *itDepots;
    }
    //DeleteFonds();
}

double DepotPos::GetCurrentTrend(const QDate &DStart, const QDate &DEnd)
{
    double dTrend;
    dTrend = GetCurrentDepotWert(DEnd);
    dTrend -= GetCurrentDepotWert(DStart);
    QList<TransActAnt>::iterator itTAA, itTAA2;
    for (itTAA = dqTAA.begin(); itTAA != dqTAA.end(); itTAA++)
    {
        if (itTAA->iType == 1)
        {
            //cout << "Got Gewinn";
            continue;
        }
        //cout << itTAA->cDatum;
        if (itTAA->cDatum >  DEnd) continue;
        if (itTAA->cDatum <=  DStart) continue;
        {
            if (itTAA == dqTAA.begin())
            {
                dTrend -= (itTAA->iAnteile)*GetCurrentFondWert(itTAA->cDatum);
            }else{
                itTAA2 = itTAA;
                itTAA2--;
                dTrend -= (itTAA->iAnteile - itTAA2->iAnteile)*GetCurrentFondWert(itTAA->cDatum);
            }
            //}else{
            //	cout << "Nicht berücksichtigt: " << itTAA->cDatum << " zwischen " << DStart << " und " << DEnd << endl;
        }
    }
    return dTrend;
}



double DepotPos::GetDepotGrowth(const QDate& cdDatLow, const QDate &cdDatHigh)
{
    polynom pPoly;
    QList<TransActAnt>::iterator itTAA;
    PolynomElement peElement;
    peElement.coefficient = GetCurrentDepotWert (cdDatLow);
    peElement.exponent = cdDatLow.daysTo(cdDatHigh);
    pPoly.push_back(peElement);

    for (itTAA = dqTAA.begin();itTAA != dqTAA.end();itTAA++)
    {
        if(itTAA->iType == 1) continue;
        if(itTAA->cDatum <= cdDatLow) continue;
        if(itTAA->cDatum > cdDatHigh) continue;
        double dOldAnt = 0;
        if (itTAA != dqTAA.begin())
        {
            itTAA--;
            dOldAnt = itTAA->iAnteile;
            itTAA++;
        }
        peElement.coefficient = (itTAA->iAnteile - dOldAnt) * GetCurrentFondWert(itTAA->cDatum);

        peElement.exponent = itTAA->cDatum.daysTo(cdDatHigh);
        pPoly.push_back(peElement);
    }
    peElement.coefficient = GetCurrentDepotWert (cdDatHigh) * (-1);
    peElement.exponent = 0;
    pPoly.push_back(peElement);

    double dZinsTag = pPoly.CalculateZero(1);
    double dZins = pow(dZinsTag,365);

    return (dZins-1) * 100;
}

double DepotPos::GetFondGrowth(const QDate& cdDatLow,const QDate &cdDatHigh)
{
    assert (cdDatLow < cdDatHigh);
    double d1 = GetCurrentFondWert (cdDatHigh);
    double d2 = GetCurrentFondWert (cdDatLow);

    return pow( double(d1 / d2), double(365) / double(cdDatLow.daysTo(cdDatHigh)) ) * 100 - 100;
    //return
}



