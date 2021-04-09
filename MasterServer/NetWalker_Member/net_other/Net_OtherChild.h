#pragma once
#include "Net_Other.h"

//--------------------------------------------------------------------------------------------
class Net_OtherChild : public Net_Other
{

public:
//--------------------------------------------------------------------------------------------
	static bool Init();
	static bool Release();

	virtual void RC_LoadClientData_Account	( int NetID , int KeyID );
	virtual void RC_SaveClientData_Account	( int NetID , int KeyID , int PageID , int Size , const char* Data );

//--------------------------------------------------------------------------------------------
};
