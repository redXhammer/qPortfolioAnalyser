#include "http.h"

#include "QDebug"

#include <openssl/ssl.h>
#include <openssl/err.h>

#define WStatDisconected  0
#define WStatConected  1

//#define LOWOUTPUT

using namespace std;

bool bWsaStarted = false;
long rc;

void log_ssl()
{
    int err;
    while ((err = ERR_get_error()))
    {
        char *str = ERR_error_string(err, 0);
        if (!str)
            return;

        qWarning() << str;
    }
}


wsock::wsock()
{
  iStatus = WStatDisconected;
  sock = 0;
  ssl = nullptr;
  return;
}

//===========================================================================

wsock::~wsock()
{
#ifndef LOWOUTPUT
  qInfo() << "Deleting wsock";
#endif
  close();
}

void wsock::close()
{
  qInfo() << "close" ;
  iStatus = WStatDisconected;
  if (sock)
#ifdef linux
    ::close(sock); // Verbindung beenden
#else
    closesocket(sock); // Windows-Variante
#endif
  if (ssl)
    SSL_free(ssl);

  sock = 0;
  ssl = nullptr;

    //SSL_CTX_free(ctx);
  return;
}

//======================================================================================

int wsock::verbinden(const QString &cAddr, int iPort)
{
  int s;
  if (iStatus == WStatConected){
#ifndef LOWOUTPUT
    qInfo() << "Still Connected, Closing";
#endif
    close();
  }
  bError = true;
#ifndef LOWOUTPUT
  qInfo() << "Creating wsock";
#endif
#ifndef linux
  WSADATA w;
  if(int result = WSAStartup(MAKEWORD(2,2), &w) != 0)
  {
    qInfo() << "Winsock 2 konnte nicht gestartet werden! Error #" << result;
    return -1;
  }
#endif
  s=socket(AF_INET,SOCK_STREAM,0);
  if(s==-1)
  {
#ifndef linux
    qInfo() <<"Fehler: Der Socket konnte nicht erstellt werden, fehler code:" << WSAGetLastError();
#else
    qInfo() <<"Fehler: Der Socket konnte nicht erstellt werden!";
#endif
    return -1;
  }else{
#ifndef LOWOUTPUT
    qInfo() << "Socket erstellt!";
#endif
  }

#ifndef linux
  SOCKADDR_IN addr;
#else
  sockaddr_in addr;
#endif

  memset(&addr,0,sizeof(sockaddr_in)); // zuerst alles auf 0 setzten
  addr.sin_family=AF_INET;
  addr.sin_port=htons(iPort);
  addr.sin_addr.s_addr=GetHost(cAddr);

  rc = connect(s,(sockaddr*)&addr,sizeof(sockaddr));

  if(rc==-1)
  {
#ifndef linux
    qInfo() << "Fehler: connect gescheitert, fehler code:" << WSAGetLastError();
#else
    qInfo() << "Fehler: connect gescheitert";
#endif

    close();
    return -1;
#ifndef LOWOUTPUT
  }else{
    qInfo() << "Verbunden mit " << cAddr;
#endif
  }

  const SSL_METHOD *meth = TLS_client_method() ; //TLSv1_2_client_method();
  SSL_CTX *ctx = SSL_CTX_new (meth);
  ssl = SSL_new (ctx);
  if (!ssl) {
    qWarning("Error creating SSL.\n");
    log_ssl();
    return -1;
  }

  SSL_set_fd(ssl, s);
  int err = SSL_connect(ssl);
  if (err <= 0) {
    qWarning("Error creating SSL connection.  err=%x", err);
    log_ssl();
    return -1;
  }
  qInfo() << "SSL connection using" << SSL_get_cipher (ssl);


  iStatus = WStatConected;
  bError = false;
  return 0;
}
//==============================================================================

ulong wsock::GetHost(const QString &cAddr)
{

  ulong uRet;
  unsigned long ip;
  hostent* he;

  if(cAddr.isEmpty())
  {
    qWarning() << "No Address";
    return 0;
  }

  ip=inet_addr(cAddr.toUtf8().data());
  if(ip!=INADDR_NONE)

  {
#ifndef LOWOUTPUT
    qInfo() << "IP";
#endif
    return ip;
  } else {
#ifndef LOWOUTPUT
    qInfo() << "Url";
#endif
    he=gethostbyname(cAddr.toUtf8().data());
    if(he==NULL)
    {
      qWarning() << "he==NULL" << cAddr;
      return 0;
    }else{

      memcpy(&uRet,he->h_addr_list[0],4);
      return uRet;
    }
  }
}

//===========================================================================

void wsock::SSL_error(int error)
{
    int err = SSL_get_error(ssl, error);
    switch (err) {
    case SSL_ERROR_WANT_WRITE:
      qWarning() << "SSL_ERROR_WANT_WRITE";
      break;
    case SSL_ERROR_WANT_READ:
      qWarning() << "SSL_ERROR_WANT_READ";
      break;
    case SSL_ERROR_ZERO_RETURN:
    case SSL_ERROR_SYSCALL:
    case SSL_ERROR_SSL:
    default:
      qWarning() << "SSL_ERROR_ other";
      break;
    }
}

bool wsock::SendAll(const QByteArray &buf)
{
  if (iStatus == WStatConected) {
    int bytesSent = 0; // Anzahl Bytes die wir bereits vom Buffer gesendet haben
    int iBS = 0;
    int size = buf.size();
    do
    {
      if ((iBS = SSL_write(ssl, buf.data() + bytesSent, size - bytesSent)) > 0)
        bytesSent += iBS;
      else
      {
        SSL_error(iBS);

#ifndef linux
        qWarning() << "Sent Failed" << WSAGetLastError();
#else
        qWarning() << "Sent Failed" << endl;
#endif
        iStatus = WStatDisconected;
        return false;
      }
    } while(bytesSent < size);
  } else {
#ifndef NOOUTPUT
    qWarning() << "Not Connected:";
#endif
    return false;
  }
  return true;
}

//===========================================================================

int wsock::RecvPart(QByteArray &buf, int maxSize)
{
  buf.resize(maxSize);
  if (iStatus == WStatConected)
  {
    int iBytesRecv = SSL_read(ssl, buf.data(), maxSize);
    if (iBytesRecv > 0)
    {
//#ifndef LOWOUTPUT
      //qInfo() << "iBytesRecv" << iBytesRecv;
//#endif
      buf.resize(iBytesRecv);
      return iBytesRecv;
    }
    else if (iBytesRecv == 0)
    {
#ifndef LOWOUTPUT
      qWarning() << "Connection Close";
#endif

      close();
      buf.clear();
      return iBytesRecv;
    }
    else if (iBytesRecv < 0)
    {
#ifndef linux
        qWarning() << "Error in Recv. Code : " << WSAGetLastError() << " : " << WSAEWOULDBLOCK;
#else
        //qWarning() << "Error in Recv.";
        SSL_error(iBytesRecv);
#endif
      return -1;
    }
    return -1;
  } else {
#ifndef NOOUTPUT
    qWarning() << "Not Connected";
#endif
    return -1;
  }
}

//===========================================================================

bool wsock::SendAll(const QString &buf)
{
  return SendAll(buf.toUtf8());
}

int wsock::RecvAll(QByteArray &buf)
{
  #define BLOCKSIZE 10000

  int iNewLen, iGesLen = 0;
  QByteArray data;
  buf.clear();

  while ((iNewLen = RecvPart(data, BLOCKSIZE)) != 0)
  {
    buf += data;
    iGesLen += iNewLen;
  }
  return iGesLen;
}

int wsock::RecvAll(QString &sData)
{
  QByteArray data;
  RecvAll(data);
  sData = QString::fromUtf8(data);
  return sData.length();
}

bool wsock::GetLine(QTextStream& line, bool skipEmpty)
{
    if (iStatus != WStatConected)
        return false;
    int res;
    char c;
    int count = 0;
    while( (res = SSL_read(ssl, &c, 1)) == 1)
    {
        if(c == '\r')
            continue;

        if(c == '\n')
        {
            if (count == 0 && skipEmpty)
                continue; //If empty read next line

            //qInfo().noquote() << "wsock::GetLine():" << *line.string();
            return true;
        }
        count++;
        line << c;
    }
    SSL_error(res);
    return false;
}
