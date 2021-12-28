#include "datafile.h"

#include <QDebug>

QTextStream& operator<< (QTextStream& stream, const KursUndDatum &kud)
{
    stream << kud.cDatum.toString() << kud.iKurs;
    return stream;
}

QTextStream& operator>> (QTextStream& stream, KursUndDatum &kud)
{
    QString date;
    stream >> date >> kud.iKurs;
    kud.cDatum = QDate::fromString(date);
    return stream;
}

//QTextStream& operator<< (QTextStream& stream, )
template <class KK, class TT>
QTextStream& operator<< (QTextStream& stream, const QMap<KK, TT> &map)
{
    stream << map.size();

    typename QMap<KK, TT>::const_iterator it;
    for (it = map.begin(); it != map.end(); it++)
    {
        stream << it.key() << it.value() << Qt::endl;
    }
    return stream;
}

template <class Key, class T>
QTextStream& operator>> (QTextStream& stream, QMap<Key, T> &map)
{
    int size;
    stream >> size;
    for (int i = 0; i < size; i++)
    {
        Key key;
        T t;
        stream >> key >> t;
        map.insert(key, t);
    }
    return stream;
}

template <typename TT>
QTextStream& operator<< (QTextStream& stream, const QList<TT> &list)
{
    stream << list.size();

    typename QList<TT>::const_iterator it;
    for (it = list.begin(); it != list.end(); it++)
    {
        stream << *it;
    }
    return stream;
}

template <typename T>
QTextStream& operator>> (QTextStream& stream, QList<T> &list)
{
    int size;
    stream >> size;
    list.clear();
    list.reserve(size);
    for (int i = 0; i < size; i++)
    {
        T t;
        stream >> t;
        list.push_back(t);
    }
    return stream;
}

bool DataFile::open(const QString& cFileOpen)
{  /****************** Datei öffnen, laden **************/
#ifndef NOOUTPUT
    qInfo() << "Try to open " << cFileOpen;
#endif
    pFile.setFileName(cFileOpen);
    if (!pFile.open(QIODevice::ReadOnly))	{	// Öffnen gescheitert? Laden überspringen
        qInfo() << "It failed to load \"" << cFileOpen << "\"";
        return false;
    } else {
        QTextStream stream(&pFile);
        stream >> mData;
        stream >> mVect;
    }
    return true;
}

bool DataFile::save(const QString &cFileOpen)
{  /****************** Datei öffnen, speichern **************/
    qInfo() << "Try to open " << cFileOpen;
    pFile.setFileName(cFileOpen);
    if (!pFile.open(QIODevice::WriteOnly))	{	// geöffnet ?
        qInfo() << "It failed, not saving";
        return false;
    } else {
        QTextStream stream(&pFile);
        stream << mData;
        stream << mVect;
    }
    pFile.close();
    return true;
}

bool DataFile::AddData(const QString& cDataClass, const QString& cName, const QString& Value)
{
    mData[cDataClass][cName] = Value;
#ifndef NOOUTPUT
    qInfo() << "Adding " << cDataClass << " " << cName << " " << Value;
#endif
    return true;
}

QString DataFile::GetData(const QString &cDataClass, const QString &cName)
{
    ITdblMAP itDataMarker = mData.find(cDataClass);
    if (itDataMarker != mData.end())
    {
        ITmap itKeyMarker = itDataMarker.value().find(cName);
        if (itKeyMarker != itDataMarker.value().end())
        {
            return itKeyMarker.value();
        }
    }
    return "";
}

QString DataFile::GetData(ITdblMAP itDataClass, const QString &cName)
{
    if (itDataClass != mData.end())
    {
        ITmap itKeyMarker = itDataClass.value().find(QString(cName));
        if (itKeyMarker != itDataClass.value().end())
        {
            return itKeyMarker.value();
        }
    }
    return "";
}

QList<KursUndDatum> &DataFile::CreateVect(const char* cDataClass,const  char* cName)
{
    return mVect[cDataClass][cName];
}


QList<KursUndDatum> &DataFile::GetVect(const char* cDataClass, const char* cName)
{
    return mVect[cDataClass][cName];
}


