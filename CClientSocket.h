#pragma once
#include <afxsock.h>

class CNGammaMonitorDlg;

class CClientSocket : public CSocket
{
 public:
     CClientSocket(CNGammaMonitorDlg* dlg);
   virtual ~CClientSocket();
   SOCKADDR_IN ClientAddr;
private:
    CNGammaMonitorDlg* m_pMainDlg;

public:
    virtual void OnReceive(int nErrorCode);
    //virtual void OnSend(int nErrorCode);
protected:
};