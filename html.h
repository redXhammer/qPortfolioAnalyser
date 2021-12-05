#ifndef HTML_H
#define HTML_H

#include <QVector>
#include <QString>

class HtmlNode
{
public:
    HtmlNode(HtmlNode*);
    HtmlNode();
    ~HtmlNode();


    HtmlNode *phnParentNode;
    QVector<HtmlNode*> vectHtmlChildNode;
    QString sName;
    QVector<QString> vectHtmlParams;

    QVector<HtmlNode*> GetElementsByName(QString);
    bool CheckSubNodes();
    bool List();
    HtmlNode* GetSubNode(QString);
    QString GetCurrentPath();
};

class html //: public http
{
    public:
    html();
    ~html();

    int open(QString);

    HtmlNode hnEntryNode;
    //HtmlNode *pCurrentParentNode;
    HtmlNode * hCurrentNode;
    bool CheckTree();

    bool cdUp();
    bool cdDown(QString,bool bList = true);
    bool cdTotalDir(QString);
    void List();
    void Clear();
    int PrintChildNodes (HtmlNode& vectNode, bool bOptions);
    int PrintChildNodes (bool bOptions = false){return PrintChildNodes(*hCurrentNode, bOptions);};


};

#endif // HTML_H
