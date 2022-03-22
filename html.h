#ifndef HTML_H
#define HTML_H

#include <QVector>
#include <QString>
#include <QTextStream>

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

    QVector<HtmlNode*> GetElementsByName(const QString &);
    bool CheckSubNodes();
    bool List();
    HtmlNode* GetSubNode(QString);
    QString GetCurrentPath();
    void Search(const QString& search);
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
    void Search(const QString& search);
    int PrintChildNodes (HtmlNode& vectNode, bool bOptions, int level);
    int PrintChildNodes (bool bOptions = false){return PrintChildNodes(*hCurrentNode, bOptions, 0);};


};

#endif // HTML_H
