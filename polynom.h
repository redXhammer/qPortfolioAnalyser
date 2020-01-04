#ifndef PolynomFileHeader
#define PolynomFileHeader

#include <math.h>
#include <deque>
#include <iostream>

#include "depot.h"


struct PolynomElement
{
	double coefficient;
	double exponent;
};

class polynom : public std::deque<PolynomElement>
{
public:
	polynom();
	~polynom();
	int AddElement(const PolynomElement&);
	double Calculate(const double&);

	double CalculateZero(const double &);

	//int operator = (depot&);


	polynom Ableiten();

//	friend std::ostream& operator << (std::ostream& OS, polynom& P);

};

std::ostream& operator << (std::ostream& OS, polynom& P);

#ifdef PolynomFile

#else // PolynomFile

#endif // PolynomFile
#endif // PolynomFileHeader

