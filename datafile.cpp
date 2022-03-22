#include "datafile.h"

#include <QDebug>

//int Level = 0;

MyTextStream& MyTextStream::operator<< (const QString str)
{
    QTextStream::operator<<(str);
    return *this;
}

MyTextStream& MyTextStream::operator>>(QString &str)
{
    str=readUntil('\n').trimmed();
    return *this;
}

MyTextStream& MyTextStream::operator>>(char &c)
{
    QTextStream::operator>>(c);
    return *this;
}

MyTextStream& MyTextStream::operator<<(const double f)
{
    QTextStream::operator<<(f);
    return *this;
}

MyTextStream& MyTextStream::operator>>(double &f)
{
    QTextStream::operator>>(f);
    return *this;
}


MyTextStream& MyTextStream::operator<< (const int i)
{
    QTextStream::operator<<(i);
    return *this;
}

MyTextStream& MyTextStream::operator>> (int &i)
{
    QTextStream::operator>>(i);
    return *this;
}

QString MyTextStream::readUntil(const QChar &sep, bool putback)
{
  QChar ch;
  QString str;
  while (!atEnd()) {
    QTextStream::operator >>(ch);
    if (ch == sep) {
      if (putback)
        seek(pos()-1);
      break;
    }
    str += ch;
  }
  return str;
}

void MyTextStream::putback(const QChar &chr)
{
  seek(pos()-1);
  QTextStream::operator<<(chr);
  seek(pos()-1);
}

MyTextStream& MyTextStream::operator<< (const KursUndDatum &kud)
{
    *this << kud.cDatum.toString("dd.MM.yyyy") << " " << kud.iKurs;
    return *this;
}

MyTextStream& MyTextStream::operator>> (KursUndDatum &kud)
{
    QString date;
    *static_cast<QTextStream*>(this) >> date;
    kud.cDatum = QDate::fromString(date,"dd.MM.yyyy");
    *this >> kud.iKurs;

    return *this;
}

//QTextStream& operator<< (QTextStream& stream, )
template <class K, class T>
MyTextStream& MyTextStream::operator<< (const QMap<K, T> &map)
{
    *this << map.size() << endl;

    typename QMap<K, T>::const_iterator it;
    for (it = map.begin(); it != map.end(); it++)
    {
        *this << it.key() << " " << it.value() << endl;
    }
    return *this;
}

template <class Key, class T>
MyTextStream& MyTextStream::operator>> (QMap<Key, T> &map)
{
    if (this->atEnd())
      return *this;
    int size;
    *this >> size;
    for (int i = 0; i < size; i++)
    {
      Key key;
      T t;
      if (this->atEnd())
        return *this;
      *static_cast<QTextStream*>(this) >> key; //For QString don't read until eol
      if (this->atEnd())
        return *this;
      *this >> t;
      map.insert(key, t);
    }
    return *this;
}

template <typename TT>
MyTextStream& MyTextStream::operator<< (const QList<TT> &list)
{
    *this << list.size() << endl;

    typename QList<TT>::const_iterator it;
    for (it = list.begin(); it != list.end(); it++)
    {
        *this << *it << endl;
    }
    return *this;
}

template <typename T>
MyTextStream& MyTextStream::operator>> (QList<T> &list)
{
    if (this->atEnd())
      return *this;

    int size;
    *this >> size;
    list.clear();
    list.reserve(size);
    for (int i = 0; i < size; i++)
    {
        T t;
        if (this->atEnd())
          return *this;
        *this >> t;
        list.push_back(t);
    }
    return *this;
}

bool DataFile::open(const QString& cFileOpen)
{  /****************** Datei öffnen, laden **************/
#ifndef NOOUTPUT

#endif
    pFile.setFileName(cFileOpen);
    if (!pFile.open(QIODevice::ReadOnly))
    {
        qInfo() << "It failed to load" << cFileOpen;
        return false;
    } else {
        qInfo() << "Opened" << cFileOpen;
        MyTextStream stream(&pFile);
        stream >> mData;
        stream >> mVect;
    }
    pFile.close();
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
        MyTextStream stream(&pFile);
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

QList<KursUndDatum> &DataFile::GetVect(const QString & cDataClass, const QString & cName)
{
    return mVect[cDataClass][cName];
}


