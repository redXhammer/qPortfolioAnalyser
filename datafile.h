#ifndef DATAFILE_H
#define DATAFILE_H

#include <QMap>
#include <QList>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QDate>

#include "kursunddatum.h"


typedef QMap<QString, QMap<QString,QString> >::iterator ITdblMAP;
typedef QMap<QString, QMap<QString,QString> >           DBLmap;
typedef QMap<QString, QString>::iterator                  ITmap;

class MyTextStream : public QTextStream {
public:
  MyTextStream(QIODevice *device)       : QTextStream(device) {}
  MyTextStream(FILE *fileHandle)        : QTextStream(fileHandle) {}
  MyTextStream(QString *string)         : QTextStream(string) {}
  MyTextStream(QByteArray *array)       : QTextStream(array) {}
  MyTextStream(const QByteArray &array) : QTextStream(array) {}

  MyTextStream& operator<< (const QString str);
  MyTextStream& operator>> (QString &str);

  MyTextStream& operator>> (char &c);

  MyTextStream& operator<< (const int i);
  MyTextStream& operator>> (int &i);


  MyTextStream& operator<< (const double f);
  MyTextStream& operator>> (double &f);

  MyTextStream& operator<< (const KursUndDatum &kud);
  MyTextStream& operator>> (KursUndDatum &kud);

  template <class K, class T> MyTextStream& operator<< (const QMap<K, T> &map);
  template <class K, class T> MyTextStream& operator>> (QMap<K, T> &map);

  template <typename T>       MyTextStream& operator<< (const QList<T> &list);
  template <typename T>       MyTextStream& operator>> (QList<T> &list);


  QString readUntil(const QChar &sep, bool putback = false);
  void putback(const QChar &chr);

private:
  QString backputchars;
};

class DataFile
{
private:
// Methoden



// Variablen
QFile pFile;
//BYTE * pbData;
long lSize;

DBLmap mData;
QMap<QString , QMap<QString,QList<KursUndDatum> > > mVect;


public:
// Methoden
    bool open(const QString &);
    bool save(const QString &);
    DataFile() {};
    DataFile(char* cFile){open(cFile);};
    ~DataFile(){};
    bool DropData(){mData.clear();mVect.clear();return true;};
    //bool

// mData zugriff
    bool AddData(const QString& cDataClass, const QString& cName, const QString& Value);
    QString GetData(const QString& cDataClass, const QString& cName);
    QString GetData(ITdblMAP itDataClass, const QString& cName);

// mVect zugriff
    QList<KursUndDatum>& GetVect (const QString &cDataClass, const QString &cName);

    QMap<QString , QMap<QString,QString> >::iterator
        GetDataItBegin(){return mData.begin();};
    QMap<QString , QMap<QString,QString> >::iterator
        GetDataItEnd(){return mData.end();};

// Variablen
};

#endif // DATAFILE_H
