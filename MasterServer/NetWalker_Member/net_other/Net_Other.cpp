#include "Net_Other.h"
#include "PG/PG_Other.h"

//-------------------------------------------------------------------
Net_Other*	Net_Other::This = NULL;
//-------------------------------------------------------------------
bool Net_Other::_Release()
{
	return false;
}
//-------------------------------------------------------------------
bool Net_Other::_Init()
{
	Server_Reg_Client_Packet( PG_Other_CtoM_LoadClientData_Account );
	Server_Reg_Client_Packet( PG_Other_CtoM_SaveClientData_Account );
	return true;
}
//-------------------------------------------------------------------
void	Net_Other::_PG_Other_CtoM_LoadClientData_Account	( int NetID , int Size , void* Data )
{
	PG_Other_CtoM_LoadClientData_Account* PG = (PG_Other_CtoM_LoadClientData_Account*)Data;
	This->RC_LoadClientData_Account( NetID , PG->KeyID );
}
void	Net_Other::_PG_Other_CtoM_SaveClientData_Account	( int NetID , int Size , void* Data )
{
	PG_Other_CtoM_SaveClientData_Account* PG = (PG_Other_CtoM_SaveClientData_Account*)Data;
	This->RC_SaveClientData_Account( NetID , PG->KeyID , PG->PageID , PG->Size , PG->Data );
}

void Net_Other::SD_LoadClientData_Account		( int SockID , int ProxyID , const char* Account , int KeyID )
{
	PG_Other_MtoD_LoadClientData_Account Send;
	Send.KeyID = KeyID;
	Send.Account = Account;
	SendToDBCenter( sizeof(Send) , &Send ); 
}

void Net_Other::SC_SaveClientDataResult_Account	( int NetID , int KeyID , bool Result )
{
	PG_Other_MtoC_SaveClientDataResult_Account Send;
	Send.KeyID = KeyID;
	Send.Result = Result;
	SendToCli( NetID , sizeof( Send ) , &Send );
}
