#include "html.h"
#include <QDebug>

#include <QByteArray>
#include <QBuffer>
#include <QFile>
#include "http.h"
#include "datafile.h"

/** @brief (one liner)
  *
  * (documentation goes here)
  */
HtmlNode::~HtmlNode()
{
    QVector<HtmlNode*>::iterator itSubNode;
    for(itSubNode=vectHtmlChildNode.begin();itSubNode!=vectHtmlChildNode.end();itSubNode++)
    {
        delete *itSubNode;
    }
    //cout << "Delete " << sName << endl;
}

/** @brief (one liner)
 *
 * (documentation goes here)
 */
HtmlNode::HtmlNode(HtmlNode* hmParent)
{
    phnParentNode = hmParent;
    sName = "Unnamed Node";
    return ;
}

/** @brief (one liner)
 *
 * (documentation goes here)
 */
HtmlNode::HtmlNode()
{
    phnParentNode = 0;
    sName = "Unnamed Node";
    return ;
}

QString HtmlNode::GetCurrentPath()
{
    QString strPath;
    HtmlNode* pCurrentNode = this;

    while (pCurrentNode->phnParentNode != 0)
    {
        QVector<HtmlNode*> vNodesWithName = pCurrentNode->phnParentNode->GetElementsByName(pCurrentNode->sName) ;

        if (vNodesWithName.size() == 1)
        {
            strPath.prepend( "/" + pCurrentNode->sName );
            pCurrentNode = pCurrentNode->phnParentNode;
        }
        else
        {
            int iPos = -2;
            for(int i = 0; i < (int)vNodesWithName.size(); i++)
            {
                if (vNodesWithName[i] == pCurrentNode)
                {
                    iPos = i+1;
                }
            }
            strPath.prepend("/" + pCurrentNode->sName + QString::number(iPos) );
            pCurrentNode = pCurrentNode->phnParentNode;
        }

    }
    return strPath;
}

void HtmlNode::Search(const QString& search)
{
    if (sName.contains(search))
    {
        qInfo() << GetCurrentPath() << " : " << sName;
    }

    foreach (QString param, vectHtmlParams) {
        if (param.contains(search))
        {
            qInfo() << GetCurrentPath() << " : " << param << "(Parameter)";
        }
    }


    for (int i = 0;i < vectHtmlChildNode.size(); i++)
    {
        vectHtmlChildNode[i]->Search(search);
    }
}

void html::Search(const QString &search)
{
    hnEntryNode.Search(search);
}


/** @brief (one liner)
  *
  * (documentation goes here)
  */
int html::open(QString cLocation)
{
    char cBeginNode;
    QFile fbFile;
    QByteArray sBuf;
    HtmlNode *pCurrentParentNode = &hnEntryNode;

    qInfo() << "Opening: " << cLocation;


    if ( cLocation.startsWith("https") )
    {
        QString strHost = cLocation.mid(8); // automate this
        int iStartPath = strHost.indexOf('/');
        QString strPath = strHost.mid(iStartPath);
        strHost = strHost.left(iStartPath);

        http httpVerbindung (strHost);

        if (httpVerbindung.SendGetRequest(strPath) != true)
            return -1;

        sBuf = httpVerbindung.sHttpData.toUtf8();
    }
    else if ( cLocation.startsWith("file") )
    {
        fbFile.setFileName(cLocation.mid(7));
        if (!fbFile.open(QIODevice::ReadOnly)) return -1;
        sBuf = fbFile.readAll();
    }
    else
    {
        qInfo().noquote() << "Error Locationtyp not recognised:" << cLocation;
        return -1;
    }

    MyTextStream IStream(sBuf);

    while (!IStream.atEnd())
    {
        if (IStream.status() != QTextStream::Ok)
        {
          qInfo() << "Bad stream Status" << IStream.status();
          break;
        }

        IStream >> cBeginNode;
        if (cBeginNode == '<')
        {
            QString sNodeContent;
            sNodeContent = IStream.readUntil('>');

            //qInfo().noquote().nospace() << "Processing: >>" << sNodeContent << "<<";

            if (sNodeContent[0] != '/')
            {
                // Create Node
                HtmlNode* phmNewNode = new HtmlNode(pCurrentParentNode);
                if (phmNewNode == NULL)
                {
                    qFatal( "Node Not Contructed");
                    return -1;
                }
                int iEndOfName = sNodeContent.indexOf(' ');
                size_t itStr = 0;
                while ((itStr=sNodeContent.indexOf('\r',itStr)) != std::string::npos)
                {
                    sNodeContent.remove(itStr,1);
                }
                itStr = 0;
                while ((itStr=sNodeContent.indexOf('\n',itStr)) != std::string::npos)
                {
                    sNodeContent.remove(itStr,1);
                }
                phmNewNode->sName = sNodeContent.mid(0,iEndOfName);
                sNodeContent = sNodeContent.mid(iEndOfName+1);

                while (!sNodeContent.isEmpty())
                {
                    iEndOfName = sNodeContent.indexOf(' ');
                    if (iEndOfName == -1)
                    {
                        QString sParam = sNodeContent.trimmed();
                        sParam.remove('\r');
                        sParam.remove('\n');
                        if (sParam.length() > 0)
                            phmNewNode->vectHtmlParams.push_back(sParam);
                        break;
                    }
                    else
                    {
                        QString sParam = sNodeContent.mid(0,iEndOfName).trimmed();
                        sParam.remove('\r');
                        sParam.remove('\n');
                        if (sParam.length() > 0)
                            phmNewNode->vectHtmlParams.push_back(sParam);
                        sNodeContent = sNodeContent.mid(iEndOfName+1);
                    }

                }


                pCurrentParentNode->vectHtmlChildNode.push_back(phmNewNode);
#ifdef VERBOSE_HTML
                qInfo() << "Created new Node. Name: " << phmNewNode->sName
                     << " as Subelement No. " << pCurrentParentNode->vectHtmlChildNode.size()
                     << " in " << pCurrentParentNode->sName
                     << " with " << phmNewNode->vectHtmlParams.size()
                     << " Params.";
#endif
                pCurrentParentNode = pCurrentParentNode->vectHtmlChildNode.back();
            }
            else
            {
                // Close Node
                sNodeContent.remove(0,1);
                int iEndOfName = sNodeContent.indexOf(' ');
                if (iEndOfName != -1)
                    sNodeContent.remove(iEndOfName);

                //qInfo().noquote().nospace() << "Closeing: >>" << sNodeContent << "<<";

                while (pCurrentParentNode->sName != sNodeContent &&
                       pCurrentParentNode->phnParentNode != &hnEntryNode &&
                       pCurrentParentNode != &hnEntryNode)
                {
                    HtmlNode *Parent = pCurrentParentNode->phnParentNode;
                    HtmlNode *Current = pCurrentParentNode;
                    std::vector<HtmlNode*> vectHelp;

                    if (Parent == NULL)
                    {
                        qFatal("Reached Top Node");
                        return -1;
                    }


                    /*std::cout << "Copy "  << Current.vectHtmlChildNode.size() <<  " Elements from "
                    << Current.sName << " to " << Parent.sName
                    << " (" << Parent.vectHtmlChildNode.size() <<")"
                    <<  std::endl;*/
                    int iMax = Current->vectHtmlChildNode.size();

                    for (int i = 0; i < iMax;i++)
                    {
                        //std::cout << Current.vectHtmlChildNode.size() <<"\t" << Current.vectHtmlChildNode[i].sName << endl;
                        vectHelp.push_back(Current->vectHtmlChildNode[i]);
                    }
                    //vectHelp.reserve(Current.vectHtmlChildNode.size());
                    //copy(Current.vectHtmlChildNode.begin(),Current.vectHtmlChildNode.end(),vectHelp.begin());
                    Current->vectHtmlChildNode.clear();
                    for (size_t i = 0; i < vectHelp.size();i++)
                    {
                        Parent->vectHtmlChildNode.push_back(vectHelp[i]);
                    }
                    //copy(vectHelp.begin(),vectHelp.end(),Parent.vectHtmlChildNode.end());

                    //PrintChildNodes(this->hnEntryNode);
                    CheckTree();
                    pCurrentParentNode = Parent;
                }

                pCurrentParentNode = pCurrentParentNode->phnParentNode;
#ifdef VERBOSE_HTML
                qInfo() << "Moving back to " << pCurrentParentNode->sName << ".";
#endif
            }
        }
        else
        {
            QString sNodeContent;
            sNodeContent = IStream.readUntil('<',true);
            sNodeContent.push_front(cBeginNode); // The first char was already read
            sNodeContent = sNodeContent.trimmed();

            if (sNodeContent.length() > 0)
            {
                //qInfo().noquote().nospace() << "Processing Text: >>" << sNodeContent << "<<";
                HtmlNode* phmNewNode = new HtmlNode(pCurrentParentNode);
                phmNewNode->sName = sNodeContent;
                pCurrentParentNode->vectHtmlChildNode.push_back(phmNewNode);
            }
        }
    }

    while (pCurrentParentNode != &hnEntryNode)
    {
        HtmlNode &Parent = *pCurrentParentNode->phnParentNode;
        HtmlNode &Current = *pCurrentParentNode;
        std::vector<HtmlNode*> vectHelp;

        /*std::cout << "Copy "  << Current.vectHtmlChildNode.size() <<  " Elements from "
        << Current.sName << " to " << Parent.sName
        << " (" << Parent.vectHtmlChildNode.size() <<")"
        <<  std::endl<<  std::endl<<  std::endl;*/
        int iMax = Current.vectHtmlChildNode.size();

        for (int i = 0; i < iMax;i++)
        {
            //std::cout << Current.vectHtmlChildNode.size() <<"\t" << Current.vectHtmlChildNode[i].sName << endl;
            vectHelp.push_back(Current.vectHtmlChildNode[i]);
        }
        //vectHelp.reserve(Current.vectHtmlChildNode.size());
        //copy(Current.vectHtmlChildNode.begin(),Current.vectHtmlChildNode.end(),vectHelp.begin());
        Current.vectHtmlChildNode.clear();
        for (size_t i = 0; i < vectHelp.size();i++)
        {
            Parent.vectHtmlChildNode.push_back(vectHelp[i]);
        }
        //copy(vectHelp.begin(),vectHelp.end(),Parent.vectHtmlChildNode.end());

        //PrintChildNodes(this->hnEntryNode);
        CheckTree();
        pCurrentParentNode = &Parent;

    }

    pCurrentParentNode = pCurrentParentNode->phnParentNode;
    hCurrentNode = &hnEntryNode;

    return 0;
}

bool HtmlNode::CheckSubNodes()
{
    //std::cout << "Checking " << sName << " with " << vectHtmlChildNode.size() << " subnodes" << std::endl;
    for (int i = 0;i < vectHtmlChildNode.size(); i++)
    {
        vectHtmlChildNode[i]->phnParentNode = this;

        vectHtmlChildNode[i]->CheckSubNodes();
    }
    return true;
}

bool HtmlNode::List()
{
    for (int i = 0;i < vectHtmlChildNode.size(); i++)
        qInfo().noquote() << "-" << vectHtmlChildNode[i]->sName;

    return true;
}

HtmlNode* HtmlNode::GetSubNode(QString cName)
{
    qInfo() << "Going into " << cName;

    for (int i = 0;i < vectHtmlChildNode.size(); i++)
        if (vectHtmlChildNode[i]->sName.compare(cName) == 0)
            return vectHtmlChildNode[i];
    qInfo()<< "Not Found";
    return 0;
}



bool html::CheckTree()
{
    return hnEntryNode.CheckSubNodes();
}

QVector<HtmlNode*> HtmlNode::GetElementsByName(const QString &cName)
{
    QVector<HtmlNode*> vectElements;
    QVector<HtmlNode*>::iterator itHtmlChildNode = vectHtmlChildNode.begin();

    for (;itHtmlChildNode != vectHtmlChildNode.end();itHtmlChildNode++)
    {
        QString sNodeName = (*itHtmlChildNode)->sName;
        int iRes = sNodeName.compare(cName);
        if (!iRes)
        {
            //std::cout << sNodeName << " added" << std::endl;
            vectElements.push_back(*itHtmlChildNode);
        }

    }
    return vectElements;
}



int html::PrintChildNodes (HtmlNode& vectNode, bool bOptions, int level)
{
    QVector<HtmlNode*>::iterator itHtmlChildNode = vectNode.vectHtmlChildNode.begin();

    for (;itHtmlChildNode!= vectNode.vectHtmlChildNode.end(); itHtmlChildNode++)
    {
        QByteArray buf;
        QTextStream cout(&buf);

        for (int iFor = 0; iFor < level;iFor++) cout << " |";

        QString name = (*itHtmlChildNode)->sName;
        bool bAddDots = false;
        if (name.contains('\n'))
        {
          name = name.left(name.indexOf('\n'));
          name = name.trimmed();
          bAddDots = true;
        }

        if (name.length() > 40)
        {
          name = name.left(40).trimmed();
          bAddDots = true;
        }

        if (bAddDots)
          name += "...";

        cout << "-" << name;
        if (bOptions) {
            QVector<QString>::iterator itParam = (*itHtmlChildNode)->vectHtmlParams.begin();
            for (;itParam != (*itHtmlChildNode)->vectHtmlParams.end();itParam++)
            {
                cout << endl << *itParam;
            }
        }
        cout.flush();
        qInfo().noquote() << buf;

        PrintChildNodes (**itHtmlChildNode,bOptions,level+1);
    }
    return 0;
}




/** @brief (one liner)
  *
  * (documentation goes here)
  */
html::~html()
{
    return;
}

/** @brief (one liner)
 *
 * (documentation goes here)
 */
html::html()
{
    return;
}

bool html::cdUp()
{
            HtmlNode * Helper = hCurrentNode->phnParentNode;
            if (Helper != 0)
            {
                hCurrentNode = Helper;
                qInfo() << "Current Dir: " << hCurrentNode->GetCurrentPath();
                hCurrentNode->List();
                return true;
            }
            else
            {
                qInfo() << "On Top Dir: " << hCurrentNode->GetCurrentPath();
                hCurrentNode->List();
                return false;
            }

}

bool html::cdDown(QString str,bool bList)
{
    QVector<HtmlNode*> vH = hCurrentNode->GetElementsByName(str);
    if (vH.size() == 1)
        hCurrentNode = vH[0];
    else if (vH.size() == 0)
    {
        int iDig = 0;
        int iNumDig = 0;
        int iLenStr = str.size();

        while (iLenStr>iDig++)
            if ( str[iDig].isDigit() ) iNumDig++;

//        qInfo() << "|" << str.substr(iLenStr-iNumDig) << "|,|" << str.substr(0,iLenStr-iNumDig) << "|";

        int iNr = str.mid(iLenStr-iNumDig).toInt() -1;
        vH = hCurrentNode->GetElementsByName(str.mid(0,iLenStr-iNumDig));

        if (vH.size() > 1 && (int)vH.size() > iNr)
            hCurrentNode = vH[iNr];
        else
        {
            qInfo() << "Path: " << hCurrentNode->GetCurrentPath();
            qInfo() << str << " not found";
            return false;
        }
    }
    else
    {
        qInfo() << "Path: " << hCurrentNode->GetCurrentPath() << "has multiple enements:" << str;
        List();
        qInfo() << "Enter number 1 .. " << vH.size();
        int i;
        QTextStream(stdin) >> i;
        hCurrentNode = vH[i-1];
    }

    if (bList)
    {
        qInfo() << "Current Dir: " << hCurrentNode->GetCurrentPath();
        hCurrentNode->List();
    }
    return true;
}

bool html::cdTotalDir(QString sDir)
{
    qInfo() << "cdTotalDir:" << sDir;
    bool bOK = true;
    if (sDir.isEmpty()) return false;
    if(sDir[0] != '/') return false;

    if (sDir.size()== 1)
    {
        hCurrentNode = &hnEntryNode;
        return true;
    }
    hCurrentNode = &hnEntryNode;
    sDir = sDir.mid(1);

    QVector<QString> vSubDirs;

    int fPos;
    while ((fPos = sDir.indexOf('/')) != -1 && bOK)
    {
        //vSubDirs.push_back(sDir.substr(0,fPos));
        bOK = cdDown(sDir.mid(0,fPos),false);

        if (!bOK)
        {
            qInfo() << "could not find " << sDir.mid(0,fPos);
            PrintChildNodes();
            return false;
        }

        sDir.remove(0,fPos+1);
    }
    if (bOK) bOK = cdDown(sDir,false);

    return bOK;
}

void html::List()
{
    hCurrentNode->List();
}

void html::Clear()
{
    hnEntryNode.vectHtmlChildNode.clear();
    hnEntryNode.phnParentNode = NULL;
    hCurrentNode = &hnEntryNode;
}



