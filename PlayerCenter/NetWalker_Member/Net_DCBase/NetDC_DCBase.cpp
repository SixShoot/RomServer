#include "PG/PG_DCBase.h"
#include "NetDC_DCBase.h"
//-------------------------------------------------------------------
CNetDC_DCBase*	CNetDC_DCBase::This = NULL;
//-------------------------------------------------------------------
bool CNetDC_DCBase::_Release()
{

	return false;
}
//-------------------------------------------------------------------
bool CNetDC_DCBase::_Init()
{

	_Net->RegOnSrvPacketFunction			( EM_PG_DCBase_XtoRD_SavePlayer						, _PG_DCBase_XtoRD_SavePlayer		    );
	_Net->RegOnSrvPacketFunction			( EM_PG_DBBase_XtoRD_SetPlayerLiveTime				, _PG_DBBase_XtoRD_SetPlayerLiveTime		);
	_Net->RegOnSrvPacketFunction			( EM_PG_DBBase_XtoRD_SetPlayerLiveTime_ByAccount	, _PG_DBBase_XtoRD_SetPlayerLiveTime_ByAccount		);
	_Net->RegOnSrvPacketFunction			( EM_PG_DBBase_XtoRD_CheckRoleDataSize				, _PG_DBBase_XtoRD_CheckRoleDataSize		);


    Global::Net_ServerList->RegServerLoginEvent( EM_SERVER_TYPE_DATACENTER , _OnDataCenterConnectEvent );

	Global::Net_ServerList->RegServerLoginEvent( EM_SERVER_TYPE_MASTER	   , _OnServersConnectEvent );
	Global::Net_ServerList->RegServerLoginEvent( EM_SERVER_TYPE_DATACENTER , _OnServersConnectEvent );
	Global::Net_ServerList->RegServerLoginEvent( EM_SERVER_TYPE_LOCAL	   , _OnServersConnectEvent );
	return true;
}
//-------------------------------------------------------------------

/*
void CNetDC_DCBase::_PG_DBBase_LtoD_SaveRoleDataNotifyResult( int NetID , int Size , void* Data )
{
	PG_DBBase_LtoD_SaveRoleDataNotifyResult* PG = ( PG_DBBase_LtoD_SaveRoleDataNotifyResult* )Data;
	This->RL_SaveRoleDataNotifyResult( PG->Account.Begin() , PG->PlayerDBID , PG->ClientID , PG->MasterSockID , PG->Result ); 
}
*/
void CNetDC_DCBase::_OnDataCenterConnectEvent ( EM_SERVER_TYPE ServerType, DWORD dwServerLocalID )
{
	This->OnDataCenterConnect();
}
void CNetDC_DCBase::_OnServersConnectEvent ( EM_SERVER_TYPE ServerType, DWORD dwServerLocalID )
{
	This->OnServersConnect( ServerType, dwServerLocalID );
}

void CNetDC_DCBase::_PG_DBBase_XtoRD_CheckRoleDataSize	( int NetID , int Size , void* Data )
{
	PG_DBBase_XtoRD_CheckRoleDataSize* PG = ( PG_DBBase_XtoRD_CheckRoleDataSize* )Data;
	This->R_CheckRoleDataSize( NetID , PG->Size ); 
}
void CNetDC_DCBase::_PG_DCBase_XtoRD_SavePlayer          ( int NetID , int Size , void* Data )
{
    PG_DCBase_XtoRD_SavePlayer* PG = ( PG_DCBase_XtoRD_SavePlayer * )Data;
	if( PG->Size != sizeof(PlayerRoleData) )
		return;
	PG->Player.SetLink();
    This->SavePlayer( NetID , PG->SaveTime , PG->DBID , PG->Start , PG->Size , &PG->Player ); 
}

void CNetDC_DCBase::_PG_DBBase_XtoRD_SetPlayerLiveTime   ( int NetID , int Size , void* Data )
{
    PG_DBBase_XtoRD_SetPlayerLiveTime* PG = ( PG_DBBase_XtoRD_SetPlayerLiveTime* )Data;
    This->SetPlayerLiveTime( NetID , PG->DBID , PG->LiveTime );
}
void CNetDC_DCBase::_PG_DBBase_XtoRD_SetPlayerLiveTime_ByAccount   ( int NetID , int Size , void* Data )
{
    PG_DBBase_XtoRD_SetPlayerLiveTime_ByAccount* PG = ( PG_DBBase_XtoRD_SetPlayerLiveTime_ByAccount* )Data;
    This->SetPlayerLiveTime_ByAccount( NetID , PG->Account , PG->LiveTime );
}
//-------------------------------------------------------------------
void CNetDC_DCBase::SavePlayerFailed         ( int SrvNetID , int DBID )
{
    PG_DBBase_RDtoX_SavePlayerError  Send;
    Send.DBID   = DBID;
    Send.Result = EM_SavePlayer_PlayerNotFind;
    _Net->SendToSrv( SrvNetID , sizeof( Send ) , &Send );

}
void CNetDC_DCBase::SetPlayerLiveTimeFailed  ( int SrvNetID , int DBID )
{
    PG_DBBase_RDtoX_SetPlayerLiveTimeError Send;
    Send.DBID = DBID;
    Send.Result = EM_SetPlayerLiveTime_PlayerNotFind;
    _Net->SendToSrv( SrvNetID , sizeof( Send ) , &Send );
}

void CNetDC_DCBase::SetPlayerLiveTimeFailed_ByAccount  ( int SrvNetID , char* Account )
{
    PG_DBBase_RDtoX_SetPlayerLiveTimeError_ByAccount Send;
//    Send.DBID = DBID;
    MyStrcpy( Send.Account , Account , sizeof(Send.Account) );
    Send.Result = EM_SetPlayerLiveTime_PlayerNotFind;
    _Net->SendToSrv( SrvNetID , sizeof( Send ) , &Send );
}
void CNetDC_DCBase::S_CheckRoleDataSizeResult( int SrvNetID , bool Result )
{
	PG_DBBase_RDtoX_CheckRoleDataSizeResult Send;
	Send.Result = Result;
	_Net->SendToSrv( SrvNetID , sizeof( Send ) , &Send );
}

void	CNetDC_DCBase::LogSqlCommand( char* SqlCmd )
{
	PG_DBBase_LtoD_SqlCommand Send;
	Send.Type = EM_SqlCommandType_WorldLogDB;
	Send.ThreadID = rand();
	Send.Data = SqlCmd;
	Send.Size = Send.Data.Size() + 1;
	SendToDBCenter( sizeof( Send ) , &Send );
}
void	CNetDC_DCBase::ImportSqlCommand( char* SqlCmd )
{
	PG_DBBase_LtoD_SqlCommand Send;
	Send.Type = EM_SqlCommandType_ImportDB;
	Send.ThreadID = rand();
	Send.Data = SqlCmd;
	Send.Size = Send.Data.Size() + 1;
	SendToDBCenter( sizeof( Send ) , &Send );
}

void CNetDC_DCBase::SL_CheckRoleLiveTime		( int ZoneID , int DBID  )
{
	PG_DBBase_DtoL_CheckRoleLiveTime Send;
	Send.DBID = DBID;
	SendToLocal( ZoneID , sizeof( Send ) , &Send );
}

void	CNetDC_DCBase::S_CheckRoleStruct( EM_SERVER_TYPE ServerType, DWORD dwServerLocalID )
{
	PG_DBBase_RDtoX_CheckRoleStruct Send;

	ReaderClass<PlayerRoleData>* Reader = RoleDataEx::ReaderBase( );
	map<string,ReadInfoStruct>&  ReadInfo = *( Reader->ReadInfo() );
	map<string,ReadInfoStruct>::iterator Iter;
	
	Send.Count = ReadInfo.size();

	int  i = 0; 
	for( Iter = ReadInfo.begin() ; Iter != ReadInfo.end() ; Iter++ , i++ )
	{		
		strcpy( Send.Info[i].Key , Iter->first.c_str() );
		Send.Info[i].Address = Iter->second.Point;
	}
	//_Net->SendToSrv( ServerID , Send.Size() , &Send );
	Net_ServerList->SendToServer( ServerType,	dwServerLocalID , Send.Size() , &Send );
}