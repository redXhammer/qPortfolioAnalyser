#include "html.h"
#include <QDebug>

#include <QByteArray>

#include <QFile>

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
            strPath = "/" + pCurrentNode->sName + strPath;
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
            strPath = "/" + pCurrentNode->sName + iPos  + strPath;
            pCurrentNode = pCurrentNode->phnParentNode;
        }

    }
    return strPath;
}


/** @brief (one liner)
  *
  * (documentation goes here)
  */
int html::open(QString cLocation)
{
    //using boost::algorithm::trim;
    //using boost::algorithm::erase_all;

    char cBeginNode;
    QFile fbFile;
    std::stringbuf sbString;
    QByteArray sBuf;
    HtmlNode *pCurrentParentNode = &hnEntryNode;

    qInfo() << "Opening: " << cLocation;

    size_t abc = cLocation.compare("http");
    if ( abc == 0)
    {
        //http://www.onvista.de/aktien/suche-vergleich/profi.html
//        QString strHost = cLocation.mid(7);
//        int iStartPath = strHost.indexOf('/');
//        QString strPath = strHost.mid(iStartPath);
//        strHost = strHost.left(iStartPath);

//        http httpVerbindung (strHost);

//        if (httpVerbindung.SendGetRequest(strPath) != true)
//            return -1;

//        sbString.sputn(httpVerbindung.sHttpData.c_str(),httpVerbindung.sHttpData.length());


//        sBuf = &sbString;
        sBuf = QByteArray(); //TODO: Correct this
    }
    else if ((abc = cLocation.compare("file")) == 0)
    {
        fbFile.setFileName(cLocation.mid(7));
        if (!fbFile.open(QIODevice::ReadOnly)) return -1;
        sBuf = fbFile.readAll();
    }
    else
    {
        qInfo() << "Error Locationtyp not recognised";
        return -1;
    }

    QTextStream IStream(sBuf);

    while (!IStream.atEnd())
    {
        IStream >> cBeginNode;
        if (cBeginNode == '<')
        {
            IStream.get();
            QString sNodeContent;
            getline(IStream,sNodeContent,'>');

            qInfo() << "Processing: " << sNodeContent << endl;

            if (sNodeContent[0] != '/')
            {
                // Create Node
                HtmlNode* phmNewNode = new HtmlNode(pCurrentParentNode);
                if (phmNewNode == NULL)
                {
                    qFatal( "Node Not Contructed");
                    return -1;
                }
                int iEndOfName = sNodeContent.find(' ');
                size_t itStr = 0;
                while ((itStr=sNodeContent.find_first_of('\r',itStr)) != std::string::npos)
                {
                    sNodeContent.erase(itStr,1);
                }
                itStr = 0;
                while ((itStr=sNodeContent.find_first_of('\n',itStr)) != std::string::npos)
                {
                    sNodeContent.erase(itStr,1);
                }
                phmNewNode->sName = sNodeContent.substr(0,iEndOfName);
                sNodeContent = sNodeContent.substr(iEndOfName+1);

                while (!sNodeContent.empty())
                {
                    iEndOfName = sNodeContent.find(' ');
                    if (iEndOfName == -1)
                    {
                        std::string sParam = sNodeContent;
                        trim(sParam);
                        erase_all(sParam, "\r");
                        erase_all(sParam, "\n");
                        if (sParam.length() > 0)
                            phmNewNode->vectHtmlParams.push_back(sParam);
                        break;
                    }
                    else
                    {
                        std::string sParam = sNodeContent.substr(0,iEndOfName);
                        trim(sParam);
                        erase_all(sParam, "\r");
                        erase_all(sParam, "\n");
                        if (sParam.length() > 0)
                            phmNewNode->vectHtmlParams.push_back(sParam);
                        sNodeContent = sNodeContent.substr(iEndOfName+1);
                    }

                }


                pCurrentParentNode->vectHtmlChildNode.push_back(phmNewNode);
                cout << "Created new Node. Name: " << phmNewNode->sName << " as Subelement No. " << pCurrentParentNode->vectHtmlChildNode.size() << " in " << pCurrentParentNode->sName << " with " << phmNewNode->vectHtmlParams.size() << " Params." << endl;
                pCurrentParentNode = pCurrentParentNode->vectHtmlChildNode.back();
            }
            else
            {
                // Close Node
                sNodeContent.erase(0,1);
                int iEndOfName = sNodeContent.find(' ');
                if (iEndOfName != -1)
                    sNodeContent.erase(iEndOfName);

                std::cout << "Closeing " << sNodeContent << " : " << std::endl ;
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

                //pCurrentParentNode = pCurrentParentNode->phnParentNode;
                cout << "Moving back to " << pCurrentParentNode->sName << "." << endl;

            }


        }
        else
        {

            std::string sNodeContent;
            getline(IStream,sNodeContent,'<');
            IStream.putback('<');
            //cout << "Processing Text: " << sNodeContent << endl;
            size_t itStr = 0;
            while ((itStr=sNodeContent.find_first_of('\r',itStr)) != std::string::npos)
            {
                sNodeContent.erase(itStr,1);
            }
            itStr = 0;
            while ((itStr=sNodeContent.find_first_of('\n',itStr)) != std::string::npos)
            {
                sNodeContent.erase(itStr,1);
            }

            while (sNodeContent.length() > 0)
            {
                if (sNodeContent[0] == ' ') sNodeContent.erase(0,1);
                else break;
            }
            if (sNodeContent.length() > 0)
            {

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
    for (size_t i = 0;i < vectHtmlChildNode.size(); i++)
    {
        vectHtmlChildNode[i]->phnParentNode = this;

        vectHtmlChildNode[i]->CheckSubNodes();
    }
    return true;
}

bool HtmlNode::List()
{
    for (size_t i = 0;i < vectHtmlChildNode.size(); i++)
        std::cout << vectHtmlChildNode[i]->sName << std::endl;

    return true;
}

HtmlNode* HtmlNode::GetSubNode(QString cName)
{
    qInfo() << "Going into " << cName;

    for (size_t i = 0;i < vectHtmlChildNode.size(); i++)
        if (strcmp(vectHtmlChildNode[i]->sName.c_str(),cName) == 0)
            return vectHtmlChildNode[i];
    qInfo()<< "Not Found";
    return 0;

}



bool html::CheckTree()
{
    return hnEntryNode.CheckSubNodes();
}

std::vector<HtmlNode*> HtmlNode::GetElementsByName(const char* cName)
{
    std::vector<HtmlNode*> vectElements;
    std::vector<HtmlNode*>::iterator itHtmlChildNode = vectHtmlChildNode.begin();

    for (;itHtmlChildNode != vectHtmlChildNode.end();itHtmlChildNode++)
    {
        std::string sNodeName = (*itHtmlChildNode)->sName;
        int iRes = strcmp(sNodeName.c_str(), cName);
        if (!iRes)
        {
            //std::cout << sNodeName << " added" << std::endl;
            vectElements.push_back(*itHtmlChildNode);
        }

    }
    return vectElements;
}



int html::PrintChildNodes (HtmlNode& vectNode, bool bOptions)
{
    static int iLayer = -1;
    iLayer++;
    std::vector<HtmlNode*>::iterator itHtmlChildNode = vectNode.vectHtmlChildNode.begin();

    for (;itHtmlChildNode!= vectNode.vectHtmlChildNode.end(); itHtmlChildNode++)
    {
        for (int iFor = 0; iFor < iLayer;iFor++) cout << " |";
        cout << "-";
        cout << (*itHtmlChildNode)->sName;
        if (bOptions) {
            std::vector<std::string>::iterator itParam = (*itHtmlChildNode)->vectHtmlParams.begin();
            for (;itParam != (*itHtmlChildNode)->vectHtmlParams.end();itParam++)
            {
                std::string sStr = *itParam;
                cout << '\n' << sStr;
            }
        }
        cout << endl;
        PrintChildNodes (**itHtmlChildNode,bOptions);

    }
    iLayer--;
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
                cout << "Current Dir: " << hCurrentNode->GetCurrentPath() << endl;
                hCurrentNode->List();
                return true;
            }
            else
            {
                cout << "On Top Dir: " << hCurrentNode->GetCurrentPath() << endl;
                hCurrentNode->List();
                return false;
            }

}

bool html::cdDown(std::string str,bool bList)
{

    std::vector<HtmlNode*> vH = hCurrentNode->GetElementsByName(str.c_str());
    if (vH.size() == 1)
        hCurrentNode = vH[0];
    else if (vH.size() == 0)
    {
        int iDig = 0;
        int iNumDig = 0;
        int iLenStr = str.size();
        const char * cName = str.c_str();

        while (iLenStr>iDig++)
            if ( isdigit(cName[iDig]) ) iNumDig++;

//        cout << "|" << str.substr(iLenStr-iNumDig) << "|,|" << str.substr(0,iLenStr-iNumDig) << "|" << endl;

        int iNr = atoi(str.substr(iLenStr-iNumDig).c_str()) -1;
        vH = hCurrentNode->GetElementsByName(str.substr(0,iLenStr-iNumDig).c_str());

        if (vH.size() > 1 && (int)vH.size() > iNr)
            hCurrentNode = vH[iNr];
        else
        {
            std::cout << "Path: " << hCurrentNode->GetCurrentPath() << std::endl;
            std::cout << str << " not found" << std::endl;
            return false;
        }
    }
    else
    {
        std::cout << "Path: " << hCurrentNode->GetCurrentPath() << std::endl;
        List();
        std::cout << "Enter number 1 .. " << vH.size() << std::endl;
        int i;
        std::cin >> i;
        hCurrentNode = vH[i-1];

    }

    if (bList)
    {
        cout << "Current Dir: " << hCurrentNode->GetCurrentPath() << endl;
        hCurrentNode->List();
    }
    return true;
}

bool html::cdTotalDir(std::string sDir)
{
    bool bOK = true;
    if (sDir.empty()) return false;
    if(sDir[0] != '/') return false;

    if (sDir.size()== 1)
    {
        hCurrentNode = &hnEntryNode;
        return true;
    }
    hCurrentNode = &hnEntryNode;
    sDir = sDir.substr(1);

    std::vector<std::string> vSubDirs;

    size_t fPos;
    while ((fPos = sDir.find('/')) != std::string::npos && bOK)
    {
        //vSubDirs.push_back(sDir.substr(0,fPos));
        bOK = cdDown(sDir.substr(0,fPos),false);

        if (!bOK)
        {
            cout << "could not find " << sDir.substr(0,fPos) << endl;
            PrintChildNodes(*hCurrentNode,false);
            return false;
        }

        sDir.erase(0,fPos+1);
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

