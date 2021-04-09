#pragma once

/****************************************************************************************************
Soap client helper class for C++
Usage: 
	1. Inherit a class from this base class
	2. Call Initial in constructor, like this:
		Initial("http://localhost/AuthService/AuthService.asmx?wsdl", "AuthService", "");
	3. Wrap the webservice by writing proxy function for each web method
	4. Call Invoke to execute the function, processing the input params and return value
		be careful, the order of parameters should be reversed in the parameter array while callin Invoke
	5. Then use this class to call webservice, like this:
		AuthServiceClient	service;
		bool ret = service.IsAuthorized(strCode);

*******************************************************************************************************/

//////////////////////////////////////////////////////////////////////////
// Last Update : 2011/04/26 - by Slime
//////////////////////////////////////////////////////////////////////////

// SOAP Toolkit 3.0 Installer :
// \\fs1\專案資料\●專案開發程式區●\soapsdk.exe

//import soapsdk
//#import "mssoap30.dll" \
//	exclude("IStream", "IErrorInfo", "ISequentialStream", "_LARGE_INTEGER", \
//	"_ULARGE_INTEGER", "tagSTATSTG", "_FILETIME")
//using namespace MSSOAPLib30;

#include "mssoap30.h"
#pragma comment(lib, "mssoap30.lib")

#include "comutil.h"
#if _DEBUG
#pragma comment(lib, "comsuppwd.lib")
#else
#pragma comment(lib, "comsuppw.lib")
#endif

///////////////////////////////////////////////////////////////////////////////////////
// Base class for calling a webservice using soap
class SoapClientBase
{
protected:
	ISoapClient	*m_pSoapClient;
	char*		m_pError;
	HRESULT		m_hr;
    bool        m_bReady;

public:
	SoapClientBase(void)
	{
		m_pSoapClient = NULL;
		m_pError = NULL;
		m_hr = 0;
        m_bReady = false;
	}

	virtual ~SoapClientBase(void)
	{
		Reset();
	}

    inline bool IsReady() { return m_bReady; }

    inline void ExceptionProcess() { m_pSoapClient = NULL; }

	///////////////////////////////////////////////////////////////////
	// Check error message issued by last call(if any)
	char* GetLastError()
	{
		return m_pError;
	}

	////////////////////////////////////////////////////////////////
	// Initial SoapClient object
	bool Initial(char* szWSDLFile, char* szService, char* szPort)
	{
		Reset();

        m_hr = ::CoInitialize(NULL);

		//create soapclient object
		m_hr = ::CoCreateInstance(__uuidof(SoapClient30), NULL, CLSCTX_INPROC_SERVER, __uuidof(ISoapClient), (LPVOID *)&m_pSoapClient);
		if (m_pSoapClient==NULL)
		{
			IssueError("Create soap client object fail");
			return false;
		}

		//init soap client
		_variant_t	varWSDL	= szWSDLFile;
		_variant_t	varWSML = "";
		_bstr_t bstrService	= szService;
		_bstr_t bstrPort	= szPort;
		_bstr_t bstrNS		= "";

		m_hr = m_pSoapClient->MSSoapInit2(varWSDL, varWSML, bstrService, bstrPort, bstrNS);
		if (FAILED(m_hr))
		{
			IssueError("Error calling MSSoapInit2");
			return false;
		}

        m_bReady = true;
		return true;
	}


protected:
	///////////////////////////////////////////////////////////////////
	// Record an error message issued by this class or inherited
	void IssueError(char* szError)
	{
		if (m_pError)
        {
			delete [] m_pError;
        }

		m_pError = new char[strlen(szError)+1];
		strcpy(m_pError, szError);
	}

	////////////////////////////////////////////////////////////////////
	// Release interface, free memory, clean everything
	void Reset()
	{
		if (m_pSoapClient)
		{
			m_pSoapClient->Release();
			m_pSoapClient = NULL;
		}
		if (m_pError)
		{
			delete [] m_pError;
			m_pError = NULL;
		}
		m_hr = S_OK;

        m_bReady = false;

        ::CoUninitialize();
	}

protected:
	/////////////////////////////////////////////////////////////////////////////////////////
	// the following code is copied from ATL code(CComDispatchDriver), and modified

	HRESULT GetIDOfName(LPCOLESTR lpsz, DISPID* pdispid)
	{
		return m_pSoapClient->GetIDsOfNames(IID_NULL, (LPOLESTR*)&lpsz, 1, LOCALE_USER_DEFAULT, pdispid);
	}

	// Invoke a method by DISPID with N parameters
	HRESULT Invoke(DISPID dispid, VARIANT* pvarParams, int nParams, VARIANT* pvarRet = NULL)
	{
		DISPPARAMS dispparams = { pvarParams, NULL, nParams, 0};
		return m_pSoapClient->Invoke(dispid, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &dispparams, pvarRet, NULL, NULL);
	}

	// Invoke a method by name with Nparameters
	HRESULT Invoke(LPCOLESTR lpszName, VARIANT* pvarParams, int nParams, VARIANT* pvarRet = NULL)
	{
		HRESULT hr;
		DISPID dispid;

		hr = GetIDOfName(lpszName, &dispid);
		if (SUCCEEDED(hr))
        {
			hr = Invoke(dispid, pvarParams, nParams, pvarRet);
        }

		return hr;
	}
};
