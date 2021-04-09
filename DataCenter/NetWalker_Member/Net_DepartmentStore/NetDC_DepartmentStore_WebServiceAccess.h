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

    // �I�s���� Web Service �ˬd�O�_�i�H�פJ���B.
    // (���e�|���T�{�A�H�U�����ե�)
    void Call_ImportAccountMoneyCheck(bool* returnValue,
                                      int OrderID, int MoneyAccount, int MoneyBonus,
                                      int WorldID, const char* AccountID);

protected:

private:
    std::string     m_strWSDL;  // ���� Web Service ���� WSDL �y�z����m.
    std::string     m_strName;  // ���� Web Service ���W��.
    std::string     m_strPort;  // ���� Web Service ���s�u��(�w�]�i�ť�)

};

#endif // DATA_CENTER_DEPARTMENT_STORE_WEB_SERVICE_ACCESS_H