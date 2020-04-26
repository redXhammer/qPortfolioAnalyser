#include "datafile.h"

#include <QDebug>

bool DataFile::open(const char* cFileOpen)
{  /****************** Datei öffnen, laden **************/
#ifndef NOOUTPUT
    cout << "Try to open " << cFileOpen << endl;
#endif
    pFile = fopen(cFileOpen,"r");
    if (pFile == NULL)	{	// Öffnen gescheitert? Laden überspringen
        cout << "It failed to load \"" << cFileOpen << "\"" << endl;
        return false;
    } else {
        // Daten laden
        fseek (pFile , 0 , SEEK_END);
        lSize = ftell (pFile);
        rewind (pFile);
        pbData = new BYTE[lSize+1];
        pbData[lSize] = 0;
        long lResult = fread(pbData,1,lSize,pFile);
        fclose(pFile);
        pFile = NULL;

        assert( lResult > 0);
        lSize = lResult;
        pbData[lSize] = 0;

        BYTE* pbHelp = pbData;
        BYTE* pbEnd = pbData + lSize;
        BYTE* pbDataMarker = 0;
        BYTE* pbValue = 0;
        while (pbHelp < pbEnd)  { // Zeilenweise die Quelldatei durchgehen & zur map hinzufügen

            long lSizeLine = (long) ::strstr ((char*)pbHelp,"\n");
            if (lSizeLine) { // Zeile isolieren
                lSizeLine -= (long)pbHelp;
                pbHelp[lSizeLine] = 0;
            }
            while (pbHelp[0] == ' ') { // führende Leerzeichen entfernen
                int iCopy = 0;
                while (pbHelp[iCopy+1] != 0) { // resursiv verschieben
                    pbHelp[iCopy] = pbHelp[iCopy+1];
                    iCopy++;
                }
                pbHelp[iCopy] = 0;
            }
            int iCopy = strlen((const char*)pbHelp) - 1;
            while (pbHelp[iCopy] == ' ' && iCopy >= 0) { // endende Leerzeichen entfernen
                pbHelp[iCopy] = 0;
                iCopy--;
            }
            if (strlen((const char*)pbHelp) == 0) {  // Abbrechen wenn str = ""
                pbHelp += lSizeLine + 1;
                continue;
            }
            if (pbHelp == (BYTE*)::strstr ((char*)pbHelp,"#")) { // auf DataMarker prüfen, evt setzten und abbrechen
                pbDataMarker = pbHelp + 1;
                pbHelp += lSizeLine + 1;
                continue;
            }
            /*********   Start VectorMode   *********/
            if (char* cBracket = ::strstr((char*)pbHelp,"{")) { // Vector Mode
                *cBracket = 0;
                iCopy = strlen((const char*)pbHelp) - 1;
                while (pbHelp[iCopy] == ' ' && iCopy >= 0) { // endende Leerzeichen entfernen
                    pbHelp[iCopy] = 0;
                    iCopy--;
                }
                QList<QString>& vVect = CreateVect( (char*)pbDataMarker, (char*) pbHelp);
            #ifndef NOOUTPUT
                cout << "To \"" << pbHelp << "\" in #" << pbDataMarker ;
            #endif
                pbHelp += lSizeLine + 1;
                while (pbHelp < pbEnd) {
                    lSizeLine = (long) ::strstr ((char*)pbHelp,"\n");
                    if (lSizeLine) { // Zeile isolieren
                        lSizeLine -= (long)pbHelp;
                        pbHelp[lSizeLine] = 0;
                    }
                    while (pbHelp[0] == ' ') { // führende Leerzeichen entfernen
                        int iCopy = 0;
                        while (pbHelp[iCopy+1] != 0) { // resursiv verschieben
                            pbHelp[iCopy] = pbHelp[iCopy+1];
                            iCopy++;
                        }
                        pbHelp[iCopy] = 0;
                    }
                    while (pbHelp[0] == '\t') { // führende Tabs entfernen
                        int iCopy = 0;
                        while (pbHelp[iCopy+1] != 0) { // resursiv verschieben
                            pbHelp[iCopy] = pbHelp[iCopy+1];
                            iCopy++;
                        }
                        pbHelp[iCopy] = 0;
                    }
                    int iCopy = strlen((const char*)pbHelp) - 1;
                    while (pbHelp[iCopy] == ' ' && iCopy >= 0) { // endende Leerzeichen entfernen
                        pbHelp[iCopy] = 0;
                        iCopy--;
                    }
                    if (strlen((const char*)pbHelp) == 0) {  // Abbrechen wenn str = ""
                        pbHelp += lSizeLine + 1;
                        continue;
                    }
                    if (NULL != ::strstr((char*)pbHelp,"}")) {
                        pbHelp += lSizeLine + 1;
                        break; // End Vector Mode
                    }
                    vVect.push_back((char*)pbHelp);
                    pbHelp += lSizeLine + 1;
                }
            #ifndef NOOUTPUT
                cout << " were " << vVect.size() << " Elements added."  << endl;
            #endif
                continue;
            }
            /*********   End VectorMode   *********/
            pbValue = (BYTE*)::strstr ((char*)pbHelp," ");
            if (pbValue != 0)
            {
                pbValue[0] = 0;
                pbValue++;

                AddData((char*)pbDataMarker,(char*)pbHelp, (char*)pbValue);
            }




            pbHelp += lSizeLine + 1;

        }
    }
    return true;
}

bool DataFile::save(const char* cFileOpen)
{  /****************** Datei öffnen, speichern **************/
    qInfo() << "Try to open " << cFileOpen;
    pFile.setFileName(cFileOpen);
    if (!pFile.open(QIODevice::WriteOnly))	{	// geöffnet ?
        qInfo() << "It failed, not saving";
        return false;
    } else {
        std::set<std::string> sDataTypes;
        std::set<std::string>::iterator itDataTypes;
        QMap<QString , QMap<QString,QString> >        ::iterator itDataMarker;
        QMap<QString,QString>                         ::iterator itKeyMarker;

        QMap<QString , QMap<QString,QList<QString> > >::iterator itDataVect;
        QMap<QString, QList<QString> >                ::iterator itKeyVect;

        for (itDataMarker = mData.begin();itDataMarker!=mData.end();itDataMarker++) {
            sDataTypes.insert ( (*itDataMarker).first);
        }
        for (itDataVect = mVect.begin();itDataVect!=mVect.end();itDataVect++) {
            sDataTypes.insert ( (*itDataVect).first);
        }


        for (itDataTypes = sDataTypes.begin();itDataTypes != sDataTypes.end();itDataTypes++) {  // DataType
            itDataMarker = mData.find(*itDataTypes);
            itDataVect   = mVect.find(*itDataTypes);
            if (!((itDataMarker != mData.end())||(itDataVect != mVect.end()))) continue;

            fputs  ("#",pFile);
            fputs  ((*itDataTypes).c_str(),pFile);
            fputs  ("\n",pFile);



            if (itDataMarker != mData.end()) {
                for (itKeyMarker = (*itDataMarker).second.begin();itKeyMarker != (*itDataMarker).second.end();itKeyMarker++) // Data Key Marker
                {
                    fputs  ((*itKeyMarker).first.c_str(),pFile);
                    fputs  (" ",pFile);
                    fputs  ((*itKeyMarker).second.c_str(),pFile);
                    fputs  ("\n",pFile);
                }
            }

            if (itDataVect != mVect.end()) {
                for (itKeyVect = (*itDataVect).second.begin();itKeyVect != (*itDataVect).second.end();itKeyVect++)  // Vect Key Marker
                {
                    if ((*itKeyVect).second.size()) {  // Vect speichern
                #ifndef NOOUTPUT
                    cout << "Saving " << (*itKeyVect).second.size() << " Elements for "<< (*itKeyVect).first << " in #" << (*itDataTypes) <<endl;
                #endif
                        fputs  ((*itKeyVect).first.c_str(),pFile);
                        fputs  (" {\n",pFile);
                        std::list<std::string>::iterator itVect = (*itKeyVect).second.begin();
                        for (;itVect != (*itKeyVect).second.end();itVect++) {
                            fputs  ((*itVect).c_str(),pFile);
                            fputs  ("\n",pFile);
                        }
                        fputs  ("}\n",pFile);
                    }
                }
            }
        }
    }
    pFile.close();
    return true;
}

bool DataFile::AddData(const char* cDataClass,const  char* cName,const  char* Value)
{
    mData[cDataClass][cName] = Value;
#ifndef NOOUTPUT
    qInfo() << "Adding " << cDataClass << " " << cName << " " << Value << endl;
#endif
    return true;
}

QString DataFile::GetData(const char* cDataClass,const  char* cName)
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

QString DataFile::GetData(ITdblMAP itDataClass, const char* cName)
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

QList<QString> &DataFile::CreateVect(const char* cDataClass,const  char* cName)
{
    return mVect[cDataClass][cName];
}


QList<QString> &DataFile::GetVect(const char* cDataClass, const char* cName)
{
    return mVect[cDataClass][cName];
}


