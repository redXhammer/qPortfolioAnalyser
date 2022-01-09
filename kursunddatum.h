#ifndef KURSUNDDATUM_H
#define KURSUNDDATUM_H

#include <QDate>

class KursUndDatum
{
public:
  KursUndDatum();
  KursUndDatum& operator = (const KursUndDatum&);

//private:
  double iKurs;
  QDate cDatum;
  //friend std::ostream& operator << (std::ostream& OS, KursUndDatum& E);
  //friend std::istream& operator >> (std::istream& OS, KursUndDatum& KUD);

};

#endif // KURSUNDDATUM_H
