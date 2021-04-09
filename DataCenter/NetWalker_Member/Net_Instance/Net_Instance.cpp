#include "Net_Instance.h"	
#include "../net_serverlist/Net_ServerList_Child.h"

Net_Instance*	Net_Instance::This		= NULL;

//-------------------------------------------------------------------
Net_Instance::Net_Instance(void)
{
}
//-------------------------------------------------------------------
Net_Instance::~Net_Instance(void)
{
}
//-------------------------------------------------------------------
bool Net_Instance::_Release()
{
	

	return true;
}
//-------------------------------------------------------------------
bool Net_Instance::_Init()
{
	Net_ServerList->RegServerReadyEvent( EM_SERVER_TYPE_LOCAL , _OnLocalSrvConnectEvent );

	_Net->RegOnSrvPacketFunction( EM_PG_Instance_LtoD_SetWorldVar	, _PG_Instance_LtoD_SetWorldVar	);
	return true;
}
//-------------------------------------------------------------------
void Net_Instance::_OnLocalSrvConnectEvent( EM_SERVER_TYPE ServerType, DWORD SrvID )
{
	This->OnZoneSrvConnect( ServerType, SrvID );
}
//-------------------------------------------------------------------
void Net_Instance::_PG_Instance_LtoD_SetWorldVar	( int ServerID , int Size , void* Data )
{
	PG_Instance_LtoD_SetWorldVar* PG = ( PG_Instance_LtoD_SetWorldVar* )Data;
	This->RL_SetWorldVar( ServerID , PG->VarName.Begin(), PG->VarValue );
}
//-------------------------------------------------------------------
/*
void Net_Instance::_PG_Instance_LtoD_GetWorldVar	( int ServerID , int Size , void* Data )
{
	PG_Instance_LtoD_GetWorldVar* PG = ( PG_Instance_LtoD_GetWorldVar* )Data;
	This->RL_GetWorldVar( ServerID, PG->CliDBID, PG->VarName );
}
*/
//-------------------------------------------------------------------
void Net_Instance::SL_ResetTime( int ZoneID , WeekDayStruct& ResetDay )
{
	PG_Instance_DtoL_ResetTime Send;
	Send.ResetDay = ResetDay;
	SendToLocal( ZoneID , sizeof(Send) , &Send );
}
void Net_Instance::SL_PlayInfo( int ZoneID , int PartyKey , int KeyID , int KeyValue )
{
	PG_Instance_DtoL_PlayInfo Send;
	Send.PartyKey = PartyKey;
	Send.KeyValue = KeyValue;
	Send.KeyID = KeyID;
	SendToLocal( ZoneID , sizeof(Send) , &Send );
}

void Net_Instance::SL_WorldVar( int ZoneID , const char* pszVarName, int iVarValue )
{
	PG_Instance_DtoL_SetWorldVar	Packet;

	Packet.VarName	= pszVarName;
	Packet.VarValue	= iVarValue;

	if( ZoneID != 0 )
	{
		SendToLocal( ZoneID, sizeof(PG_Instance_DtoL_SetWorldVar), &Packet );
	}
	else
	{
		Global::SendToAllLocal( sizeof(PG_Instance_DtoL_SetWorldVar), &Packet );
	}

}