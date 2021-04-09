
#include "NetDC_DepartmentStore_WebServiceAccess.h"

//////////////////////////////////////////////////////////////////////////

WebServiceAccess::WebServiceAccess(const char* ServiceWSDL, const char* ServiceName, const char* ServicePort)
{
    // keep connection data.
    m_strWSDL.assign(ServiceWSDL);
    m_strName.assign(ServiceName);
    m_strPort.assign(ServicePort);

    // we must catch exception when initialize, if connection failed, user will know that.
    try
    {
        //Initial("http://localhost:8064/WebService_ImportCheck.asmx?wsdl", "WebService_ImportCheck", "");
        Initial(const_cast<char*>(ServiceWSDL), const_cast<char*>(ServiceName), const_cast<char*>(ServicePort));
    }
    catch (...)
    {
        ExceptionProcess();
        printf("> SimpleSoapClient() Exception!\n");
    }
}

WebServiceAccess::~WebServiceAccess()
{
}

//////////////////////////////////////////////////////////////////////////

void WebServiceAccess::Call_ImportAccountMoneyCheck(bool* returnValue,
                                                    int OrderID, int MoneyAccount, int MoneyBonus,
                                                    int WorldID, const char* AccountID)
{
    if (!m_bReady)
    {
        *returnValue = false;
        return;
    }

    HRESULT hr = S_FALSE;
    _variant_t varResult;
    _variant_t varParamArray[5];    // 參數必須逆序排列(堆疊)

    varResult = false;
    varParamArray[4] = OrderID;
    varParamArray[3] = MoneyAccount;
    varParamArray[2] = MoneyBonus;
    varParamArray[1] = WorldID;
    varParamArray[0] = AccountID;

    hr = Invoke(L"Check_ImportAccountMoney", varParamArray, 5, &varResult);
    if (hr == S_OK)
    {
        if (varResult == _variant_t(false))
        {
            *returnValue = false;
        }
        else
        {
            *returnValue = true;
        }
    }
    else
    {
        *returnValue = false;
    }
}
