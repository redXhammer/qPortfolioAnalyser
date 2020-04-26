#ifndef DATAFILE_H
#define DATAFILE_H

#include <QMap>
#include <QList>
#include <QString>
#include <QFile>

typedef QMap<QString , QMap<QString,QString> >::iterator ITdblMAP;
typedef QMap<QString , QMap<QString,QString> >           DBLmap;
typedef QMap<QString,QString>::iterator                  ITmap;

class DataFile
{
private:
// Methoden



// Variablen
QFile pFile;
//BYTE * pbData;
long lSize;

DBLmap mData;
QMap<QString , QMap<QString,QList<QString> > > mVect;


public:
// Methoden
    bool open(const char*);
    bool save(const char*);
    DataFile() {};
    DataFile(char* cFile){open(cFile);};
    ~DataFile(){};
    bool DropData(){mData.clear();mVect.clear();return true;};
    //bool

// mData zugriff
    bool AddData(const char* cDataClass,const  char* cName,const  char* Value);
    QString GetData(const char* cDataClass, const char* cName);
    QString GetData(ITdblMAP itDataClass, const char* cName);

// mVect zugriff
    QList<QString>& CreateVect (const char* cDataClass,const  char* cName);
    QList<QString>& GetVect (const char* cDataClass, const char* cName);

    QMap<QString , QMap<QString,QString> >::iterator
        GetDataItBegin(){return mData.begin();};
    QMap<QString , QMap<QString,QString> >::iterator
        GetDataItEnd(){return mData.end();};

// Variablen

};

#endif // DATAFILE_H
