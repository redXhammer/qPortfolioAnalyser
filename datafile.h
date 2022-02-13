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
