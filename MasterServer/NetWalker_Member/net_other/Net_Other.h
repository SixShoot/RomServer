
#pragma once

#include "../../MainProc/Global.h"
#include "../Net_ServerList/Net_ServerList_Child.h"
#include "PG/PG_Other.h"

class Net_Other : public Global
{
protected:
	//-------------------------------------------------------------------
	static Net_Other*	This;

	static bool				_Init();
	static bool				_Release();

	static void	_PG_Other_CtoM_LoadClientData_Account	( int NetID , int Size , void* Data );
	static void	_PG_Other_CtoM_SaveClientData_Account	( int NetID , int Size , void* Data );
public:
    //-------------------------------------------------------------------	
	virtual void RC_LoadClientData_Account	( int NetID , int KeyID ) = 0;
	virtual void RC_SaveClientData_Account	( int NetID , int KeyID , int PageID , int Size , const char* Data ) = 0;

	static  void SD_LoadClientData_Account		( int SockID , int ProxyID , const char* Account , int KeyID );
	static  void SC_SaveClientDataResult_Account( int NetID , int KeyID , bool Result );

    //-------------------------------------------------------------------	
};
