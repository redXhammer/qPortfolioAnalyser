

#ifndef FondFileHeader
#define FondFileHeader
#include "html.h"
#include "http.h"

#include <deque>
#include <map>

#include <QDate>

#include "datafile.h"

bool GetSecu(const QString cSearch, QString &sSecu, QString &sURL, QString &sName);

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
  fond(const QString &sWkn);
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

class FondStorage : public QMap<QString,fond*>
{
public:
  FondStorage();
  ~FondStorage();

  fond* AddFond(const QString &wkn);
  fond* GetFondClass(const QString &wkn);

  bool LoadContainerFile(const QString &file);
  bool UpdateLoadesFiles();
  bool SyncWithInternet();
  bool SaveContainerFile(const QString &file);
  int GetFilteredList(QStringList& vList);
  bool GetKagFonds(int iKagNr);
  QMap<int,QString> GetKags();

  int iStatus; // 0 OK, 1 Daten Laden, 2 Fehler

  //friend std::istream& operator >> (std::istream& OS, FondStorage& FS);

private:

protected:

};


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

