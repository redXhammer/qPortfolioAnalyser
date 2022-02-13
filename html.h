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

class MyTextStream : public QTextStream {
public:
  MyTextStream(FILE *fileHandle)        : QTextStream(fileHandle) {}
  MyTextStream(QString *string)         : QTextStream(string) {}
  MyTextStream(QByteArray *array)       : QTextStream(array) {}
  MyTextStream(const QByteArray &array) : QTextStream(array) {}

  QString readUntil(const QChar &sep, bool putback = false)
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

  void putback(const QChar &chr)
  {
    seek(pos()-1);
    QTextStream::operator<<(chr);
    seek(pos()-1);
  }
private:
  QString backputchars;
};

#endif // HTML_H
