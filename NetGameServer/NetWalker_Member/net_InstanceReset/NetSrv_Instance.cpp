#include "../NetWakerGSrvInc.h"
#include "NetSrv_Instance.h"
#include "../../mainproc/PartyInfoList/PartyInfoList.h"
//-------------------------------------------------------------------
NetSrv_Instance*    NetSrv_Instance::This         = NULL;

//-------------------------------------------------------------------
bool NetSrv_Instance::_Init()
{
	_Net->RegOnSrvPacketFunction			( EM_PG_Instance_DtoL_ResetTime		, _PG_Instance_DtoL_ResetTime		);
	_Net->RegOnSrvPacketFunction			( EM_PG_Instance_DtoL_PlayInfo		, _PG_Instance_DtoL_PlayInfo		);

	_Net->RegOnSrvPacketFunction			( EM_PG_Instance_LtoL_InstanceExtend				, _PG_Instance_LtoL_InstanceExtend			);
	_Net->RegOnSrvPacketFunction			( EM_PG_Instance_LtoL_InstanceExtendResult			, _PG_Instance_LtoL_InstanceExtendResult	);
	_Net->RegOnSrvPacketFunction			( EM_PG_Instance_LtoL_InstanceRestTimeRequest		, _PG_Instance_LtoL_InstanceRestTimeRequest	);

	_Net->RegOnSrvPacketFunction			( EM_PG_Instance_DtoL_SetWorldVar					, _PG_Instance_DtoL_SetWorldVar				);

	Srv_NetCliReg ( PG_Instance_CtoL_InstanceExtend );
	Srv_NetCliReg ( PG_Instance_CtoL_InstanceRestTimeRequest );

	Srv_NetCliReg ( PG_Instance_CtoL_JoinInstanceResult );

	Srv_NetCliReg ( PG_Instance_CtoL_GetWorldVar );

	Global::Net_ServerList->RegServerLogoutEvent( EM_SERVER_TYPE_CHAT , _OnChatDisconnectEvent );
	Global::Net_ServerList->RegServerLoginEvent( EM_SERVER_TYPE_CHAT , _OnChatConnectEvent );

    return true;
}
//-------------------------------------------------------------------
bool NetSrv_Instance::_Release()
{
    return true;
}

void NetSrv_Instance::_PG_Instance_CtoL_GetWorldVar( int sockid , int size , void* data )
{
	BaseItemObject* Player = BaseItemObject::GetObjBySockID( sockid );
	if( Player == NULL )
		return;

	PG_Instance_CtoL_GetWorldVar* pg = (PG_Instance_CtoL_GetWorldVar*)data;
	This->RC_GetWorldVar( Player, pg->VarName.Begin() );
}
//-------------------------------------------------------------------
void NetSrv_Instance::_PG_Instance_DtoL_SetWorldVar	( int sockid , int size , void* data )
{
	PG_Instance_DtoL_SetWorldVar* pg = (PG_Instance_DtoL_SetWorldVar*)data;
	This->RD_SetWorldVar( pg->VarName.Begin(), pg->VarValue );
}
//-------------------------------------------------------------------
void NetSrv_Instance::_PG_Instance_CtoL_InstanceExtend	( int sockid , int size , void* data )
{
	BaseItemObject* Player = BaseItemObject::GetObjBySockID( sockid );
	if( Player == NULL )
		return;
	PG_Instance_CtoL_InstanceExtend* pg = (PG_Instance_CtoL_InstanceExtend*)data;

	This->RC_InstanceExtend( Player , pg->ZoneID , pg->SavePos , pg->ExtendTime ); 
}
//-------------------------------------------------------------------
void NetSrv_Instance::_PG_Instance_LtoL_InstanceExtend	( int sockid , int size , void* data )
{
	PG_Instance_LtoL_InstanceExtend* pg = (PG_Instance_LtoL_InstanceExtend*)data;

	This->RL_InstanceExtend( sockid , pg->PlayerDBID , pg->ZoneID , pg->LiveTime , pg->ExtendTime , pg->KeyID , pg->SavePos ); 
}
//-------------------------------------------------------------------
void NetSrv_Instance::_PG_Instance_LtoL_InstanceExtendResult( int sockid , int size , void* data )
{
	PG_Instance_LtoL_InstanceExtendResult* pg = (PG_Instance_LtoL_InstanceExtendResult*)data;
	This->RL_InstanceExtendResult( pg->PlayerDBID , pg->ZoneID , pg->ExtendTime , pg->LiveTime , pg->SavePos , pg->KeyID ); 
}

void NetSrv_Instance::_PG_Instance_CtoL_InstanceRestTimeRequest( int sockid , int size , void* data )
{
	BaseItemObject* Player = BaseItemObject::GetObjBySockID( sockid );
	if( Player == NULL )
		return;
	PG_Instance_CtoL_InstanceRestTimeRequest* pg = (PG_Instance_CtoL_InstanceRestTimeRequest*)data;

	This->RC_InstanceRestTimeRequest( Player , pg->ZoneID , pg->SavePos ); 
}
void NetSrv_Instance::_PG_Instance_LtoL_InstanceRestTimeRequest( int sockid , int size , void* data )
{
	PG_Instance_LtoL_InstanceRestTimeRequest* pg = (PG_Instance_LtoL_InstanceRestTimeRequest*)data;
	This->RL_InstanceRestTimeRequest( pg->PlayerDBID , pg->KeyID ); 
}
//-------------------------------------------------------------------
void NetSrv_Instance::_OnChatConnectEvent    ( EM_SERVER_TYPE ServerType, DWORD dwServerLocalID )
{
	This->OnChatConnect( );
}
void NetSrv_Instance::_OnChatDisconnectEvent ( EM_SERVER_TYPE ServerType, DWORD dwServerLocalID )
{
	This->OnChatDisconnect( );
}

void	NetSrv_Instance::_PG_Instance_CtoL_JoinInstanceResult( int sockid , int size , void* data )
{	
	BaseItemObject* Player = BaseItemObject::GetObjBySockID( sockid );
	if( Player == NULL )
		return;
	PG_Instance_CtoL_JoinInstanceResult* pg = (PG_Instance_CtoL_JoinInstanceResult*)data;

	This->RC_JoinInstanceResult( Player , pg->Result ); 
}
void	NetSrv_Instance::_PG_Instance_DtoL_ResetTime	( int sockid , int size , void* data )
{
	PG_Instance_DtoL_ResetTime* pg =(PG_Instance_DtoL_ResetTime*)data;
	This->RD_ResetTime( pg->ResetDay );
}
void	NetSrv_Instance::_PG_Instance_DtoL_PlayInfo		( int sockid , int size , void* data )
{
	PG_Instance_DtoL_PlayInfo* pg =(PG_Instance_DtoL_PlayInfo*)data;
	This->RD_PlayInfo( pg->PartyKey , pg->KeyID , pg->KeyValue );
}


void NetSrv_Instance::SC_ResetTime( int SendToID , int Time, int InstanceSaveID , int ValueKeyID , bool IsLogin )
{
	PG_Instance_LtoC_ResetTime Send;
	Send.Time = Time;
	Send.InstanceSaveID = InstanceSaveID;
	Send.ValueKeyID = ValueKeyID;
	Send.IsLogin = IsLogin;
	Global::SendToCli_ByGUID( SendToID , sizeof(Send) , &Send );
}

void NetSrv_Instance::SC_JoinInstanceRequest( int SendToID )
{
	PG_Instance_LtoC_JoinInstanceRequest Send;
	Global::SendToCli_ByGUID( SendToID , sizeof(Send) , &Send );
}
void NetSrv_Instance::SC_AllPlayer_InstanceReset( int InstanceKeyID )
{
	PG_Instance_LtoC_InstanceReset Send;
	Send.InstanceKey = InstanceKeyID;
	Global::SendToAllPlayer( sizeof(Send) , &Send );
}

void NetSrv_Instance::SC_AllMember_InstanceCreate( int PartyID , int Key , int PlayerDBID )
{
	PartyInfoStruct* Party = PartyInfoListClass::This()->GetPartyInfo( PartyID );	
	if( Party == NULL )
		return;

	PG_Instance_LtoC_InstanceCreate Send;
	Send.ZoneID = RoleDataEx::G_ZoneID;
	Send.Key = Key;
	Send.PlayerDBID = PlayerDBID;

	for( unsigned i = 0 ; i < Party->Member.size() ; i++ )
	{
		Global::SendToCli_ByDBID( Party->Member[i].DBID , sizeof(Send) , &Send );
	}
}

void NetSrv_Instance::SC_FixInstanceExtend( int PlayerDBID , int ZoneID , int ExtendTime , int LiveTime , int KeyID )
{
	PG_Instance_LtoC_FixInstanceExtend Send;
	Send.ZoneID = ZoneID;
	Send.ExtendTime = ExtendTime;
	Send.LiveTime = LiveTime;
	Send.KeyID = KeyID;
	Global::SendToCli_ByDBID( PlayerDBID , sizeof(Send) , &Send );
}

void NetSrv_Instance::SL_InstanceExtend( int ZoneID , int PlayerDBID , int LiveTime , int ExtendTime , int KeyID , int SavePos )
{
	PG_Instance_LtoL_InstanceExtend Send;

	Send.ZoneID = ZoneID;
	Send.ExtendTime = ExtendTime;
	Send.LiveTime = LiveTime;
	Send.PlayerDBID = PlayerDBID;
	Send.KeyID = KeyID;
	Send.SavePos = SavePos;

	Global::SendToLocal( ZoneID , sizeof(Send) , &Send );
}
void NetSrv_Instance::SL_InstanceRestTimeRequest( int ZoneID , int PlayerDBID , int KeyID )
{
	PG_Instance_LtoL_InstanceRestTimeRequest Send;

	Send.PlayerDBID = PlayerDBID;
	Send.KeyID = KeyID;

	Global::SendToLocal( ZoneID , sizeof(Send) , &Send );
}
void NetSrv_Instance::SL_InstanceExtendResult( int SrvSockID , int PlayerDBID , int ZoneID , int ExtendTime , int LiveTime , int SavePos , int KeyID )
{
	PG_Instance_LtoL_InstanceExtendResult Send;
	Send.PlayerDBID = PlayerDBID;
	Send.ZoneID = ZoneID;
	Send.ExtendTime = ExtendTime;
	Send.LiveTime = LiveTime;
	Send.SavePos = SavePos;
	Send.KeyID = KeyID;
	Global::SendToSrvBySockID( SrvSockID , sizeof(Send) , &Send );
}
void NetSrv_Instance::SC_InstanceRestTime( int PlayerDBID , int ZoneID , int KeyID , int LiveTime , int ExtendTime )
{
	PG_Instance_LtoC_InstanceRestTime Send;
	Send.ZoneID = ZoneID;
	Send.ExtendTime = ExtendTime;
	Send.LiveTime = LiveTime;
	Send.KeyID = KeyID;
	Global::SendToCli_ByDBID( PlayerDBID , sizeof(Send) , &Send );
}