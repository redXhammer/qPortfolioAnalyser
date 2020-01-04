#define PolynomFile
#include "polynom.h"

using namespace std;



polynom::polynom()
{
	return;
}

polynom::~polynom()
{
	return;
}

int polynom::AddElement(const PolynomElement &peElement)
{
	push_back(peElement);
	return 0;
}

double polynom::Calculate(const double &dX)
{
	double iPolynomWert = 0;
	deque<PolynomElement>::iterator itPE;

	for (itPE = begin(); itPE != end(); itPE++)
	{
	iPolynomWert += itPE->coefficient * pow(dX , itPE->exponent );
	}
	return iPolynomWert;
}


polynom polynom::Ableiten()
{
	polynom pPolynomAbl;
	deque<PolynomElement>::iterator itPE;
	PolynomElement peElement;

	for (itPE = begin(); itPE != end(); itPE++)
	{
		peElement.coefficient = itPE->coefficient * itPE->exponent;
		peElement.exponent=  itPE->exponent - 1;
		if (peElement.exponent != -1)
			pPolynomAbl.push_back(peElement);
	}
	return pPolynomAbl;
}


double polynom::CalculateZero(const double & dStart)
{
	double dZero = dStart, dDelta = 1;
	int iLoops = 0;
	polynom pAbl = Ableiten();
	while(fabs(Calculate(dZero)) > 0.000001)
	{
		if ((iLoops++) > 100000)
		{
			cout << "Zeros Error: 10000 loops" << endl;
			break;
		}
		//cout << dZero << endl;
		dDelta = Calculate(dZero) / pAbl.Calculate(dZero);
		//cout << Calculate(dZero) << "/" << pAbl.Calculate(dZero) << endl;
		dZero = dZero - dDelta;

	}
	//cout.precision(10);
	//cout << "Nullstelle: " << dZero  << " in " << iLoops << " loops"<< endl;
	return dZero;
}

/*int polynom::operator = (depot &dDepot)
{
	deque<TransActAnt>::iterator itTAA;
	PolynomElement peElement;
#ifdef _console
	cout << "Size: " << dDepot.size() << endl;
#endif
	//for (itTAA = dDepot.begin();itTAA != dDepot.end(); itTAA++)
	//	cout << itTAA->cDatum << " " << itTAA->iAnteile << endl;

	for (itTAA = dDepot.begin();itTAA != dDepot.end();itTAA++)
	{
		if (itTAA->iType == 1) continue;
		double dOldAnt = 0;
		if (itTAA != dDepot.begin())
		{
			itTAA--;
			dOldAnt = itTAA->iAnteile;
			itTAA++;
		}
		peElement.coefficient = (itTAA->iAnteile - dOldAnt) * dDepot.GetCurrentFondWert(itTAA->cDatum);

		peElement.exponent = iDateToday - itTAA->cDatum;
		push_back(peElement);
	}
	peElement.coefficient = dDepot.GetCurrentDepotWert (iDateToday) * (-1);
	peElement.exponent = 0;
	push_back(peElement);
	return 0;
}*/

std::ostream& operator << (std::ostream& OS, polynom& P)
{
	deque<PolynomElement>::iterator itPE;
	cout.precision(2);
	for (itPE = P.begin(); itPE != P.end(); itPE++)
	{
		cout.setf(ios::fixed);
		cout.precision(2);
		 cout << itPE->coefficient;
		 cout.precision(0);
		 cout << "^" << itPE->exponent << " + ";
	}
	cout << endl;
	cout.precision(2);
	return OS;
}
