#ifndef DATA_CENTER_DEPARTMENT_STORE_WEB_SERVICE_ACCESS_H
#define DATA_CENTER_DEPARTMENT_STORE_WEB_SERVICE_ACCESS_H

//////////////////////////////////////////////////////////////////////////
// Last Update - by slime 2011/04/12
//////////////////////////////////////////////////////////////////////////

#include "SoapClientBase.h"

#include <string>

class WebServiceAccess : SoapClientBase
{
public:
    WebServiceAccess(const char* ServiceWSDL, const char* ServiceName, const char* ServicePort);
    virtual ~WebServiceAccess();

    //////////////////////////////////////////////////////////////////////////

    // 呼叫遠端 Web Service 檢查是否可以匯入金額.
    // (內容尚未確認，以下為測試用)
    void Call_ImportAccountMoneyCheck(bool* returnValue,
                                      int OrderID, int MoneyAccount, int MoneyBonus,
                                      int WorldID, const char* AccountID);

protected:

private:
    std::string     m_strWSDL;  // 遠端 Web Service 提供 WSDL 描述的位置.
    std::string     m_strName;  // 遠端 Web Service 的名稱.
    std::string     m_strPort;  // 遠端 Web Service 的連線埠(預設可空白)

};

#endif // DATA_CENTER_DEPARTMENT_STORE_WEB_SERVICE_ACCESS_H