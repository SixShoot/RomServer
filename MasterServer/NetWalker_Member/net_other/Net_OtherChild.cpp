#include "Net_OtherChild.h"	
#include "StackWalker/StackWalker.h"
#include "FileVersion/FileVersion.h"
#include "../net_dcbase/Net_DCBaseChild.h"
//-------------------------------------------------------------------
bool    Net_OtherChild::Init()
{
	Net_Other::_Init();

	if( This != NULL)
		return false;

	This = NEW( Net_OtherChild );

	return true;
}

//--------------------------------------------------------------------------------------
bool    Net_OtherChild::Release()
{
	if( This == NULL )
		return false;

	delete This;
	This = NULL;
	
	Net_Other::_Release();
	return true;

}
//--------------------------------------------------------------------------------------
void Net_OtherChild::RC_LoadClientData_Account	( int NetID , int KeyID )
{
	GSrv_CliInfoStruct* CliNetInfo = _Net->GetCliNetInfo( NetID );
	if( CliNetInfo == NULL )
		return;
	SD_LoadClientData_Account(  NetID , CliNetInfo->ProxyID , CliNetInfo->Account.c_str() , KeyID );
}
void Net_OtherChild::RC_SaveClientData_Account	( int NetID , int KeyID , int PageID , int Size , const char* Data )
{
	GSrv_CliInfoStruct* CliNetInfo = _Net->GetCliNetInfo( NetID );
	if( CliNetInfo == NULL )
		return;

	if( Size > 1024 || Size <= 0)
	{
		SC_SaveClientDataResult_Account( NetID ,  KeyID , false );
		return;
	}

	char SqlCmd[0x1000];
	if( PageID == 0 )
	{
		sprintf( SqlCmd , "DELETE Table_ClientData_Account WHERE Account='%s' and KeyID=%d" 
			, CliNetInfo->Account.c_str()
			, KeyID );

		Net_DCBase::SqlCommand( NetID , SqlCmd );
	}


	string CmdBinStr = StringToSqlX( Data , Size , false );
	sprintf( SqlCmd , "INSERT Table_ClientData_Account(Account,KeyID,PageID,Size,Data) VALUES('%s',%d,%d,%d,cast(%s as binary(1024)))" 
		, CliNetInfo->Account.c_str()
		, KeyID
		, PageID 
		, Size 
		, CmdBinStr.c_str() );
	Net_DCBase::SqlCommand( NetID , SqlCmd );

	SC_SaveClientDataResult_Account( NetID ,  KeyID , true );
}