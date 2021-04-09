#include "NetChat_ChannelBase.h"
#include "../../MainProc/Global.h"
#include "../Net_ServerList/Net_ServerList_Child.h"
//-------------------------------------------------------------------
NetSrv_ChannelBase*    NetSrv_ChannelBase::This         = NULL;
//-------------------------------------------------------------------
bool NetSrv_ChannelBase::_Init()
{

    _Net->RegOnCliConnectFunction       ( _OnCliConnect );
    _Net->RegOnCliDisconnectFunction    ( _OnCliDisconnect );

	_Net->RegOnSrvConnectFunction		( _OnSrvConnect );


    _Net->RegOnSrvPacketFunction	( EM_PG_ChannelBase_toChat_DataTransmitToGSrv_Group     , _PG_ChannelBase_toChat_DataTransmitToGSrv_Group );
    _Net->RegOnSrvPacketFunction	( EM_PG_ChannelBase_toChat_DataTransmitToCli_Group      , _PG_ChannelBase_toChat_DataTransmitToCli_Group  );
    _Net->RegOnSrvPacketFunction	( EM_PG_ChannelBase_LtoChat_ChangeRoleInfo              , _PG_ChannelBase_LtoChat_ChangeRoleInfo		  );
    _Net->RegOnSrvPacketFunction	( EM_PG_ChannelBase_toChat_DataTransmitToSrv            , _PG_ChannelBase_toChat_DataTransmitToSrv        );
    _Net->RegOnSrvPacketFunction	( EM_PG_ChannelBase_toChat_DataTransmitToCli            , _PG_ChannelBase_toChat_DataTransmitToCli        );


	_Net->RegOnSrvPacketFunction	( EM_PG_ChannelBase_LtoChat_CreateChannel               , _PG_ChannelBase_LtoChat_CreateChannel          );
	_Net->RegOnSrvPacketFunction	( EM_PG_ChannelBase_LtoChat_AddUser                     , _PG_ChannelBase_LtoChat_AddUser                );
	_Net->RegOnSrvPacketFunction	( EM_PG_ChannelBase_LtoChat_DelUser                     , _PG_ChannelBase_LtoChat_DelUser                );

    _Net->RegOnSrvPacketFunction	( EM_PG_ChannelBase_LtoChat_SetDSysTime					, _PG_ChannelBase_LtoChat_SetDSysTime        );

	Srv_NetCliReg( PG_ChannelBase_CtoChat_DataTransmitToCli_Group	);
	Srv_NetCliReg( PG_ChannelBase_CtoChat_ChannelPlayerList			);
	Srv_NetCliReg( PG_ChannelBase_CtoChat_JoinPlayerDisabled		);
	Srv_NetCliReg( PG_ChannelBase_CtoChat_KickPlayer				);
	Srv_NetCliReg( PG_ChannelBase_CtoChat_SetManager				);

	Srv_NetCliReg( PG_ChannelBase_CtoChat_ZoneChannelID				);

	_Net->RegOnCliPacketFunction	( EM_PG_ChannelBase_CtoChat_DataTransmitToCli_Name     , _PG_ChannelBase_CtoChat_DataTransmitToCli_Name 
									, sizeof(PG_ChannelBase_CtoChat_DataTransmitToCli_Name) - 0x100 , sizeof(PG_ChannelBase_CtoChat_DataTransmitToCli_Name)  );		

	Global::Net_ServerList->RegServerLoginEvent( EM_SERVER_TYPE_LOCAL , _OnLocalSrvConnect );
    return true;
}
//-------------------------------------------------------------------
bool NetSrv_ChannelBase::_Release()
{
    return true;
}
//-------------------------------------------------------------------
void NetSrv_ChannelBase::_PG_ChannelBase_ChattoL_SetChannelOwner( int Sockid , int Size , void* Data )
{
	PG_ChannelBase_ChattoL_SetChannelOwner*   PG = (PG_ChannelBase_ChattoL_SetChannelOwner*)Data;
	This->SetChannelOwner( PG->Type , PG->ChannelID , PG->PlayerDBID );
}
void NetSrv_ChannelBase::_OnLocalSrvConnect( EM_SERVER_TYPE ServerType , DWORD ZoneID )
{
	This->OnLocalSrvConnect( ZoneID );
}
void NetSrv_ChannelBase::_OnSrvConnect( int SrvSockID , string SrvName )
{
	This->OnSrvConnect( SrvSockID );
}

void NetSrv_ChannelBase::_OnCliConnect( int ID , string Account )
{
    This->CliConnect( ID , (char*)Account.c_str() );
}
//-------------------------------------------------------------------
void NetSrv_ChannelBase::_OnCliDisconnect( int CliID )
{
    This->CliDisconnect( CliID );
}
//-------------------------------------------------------------------
//通知 Client 連上 Chat
void NetSrv_ChannelBase::ClientConnect		( int SockID )
{
	PG_ChannelBase_ChattoC_ClientConnectToChat Send;
	SendToCli( SockID , sizeof( Send ) , &Send );
}
void NetSrv_ChannelBase::SC_PlayerNotFind	( int SockID , const char* Name )
{
	PG_ChannelBase_ChattoC_PlayerNotFind Send;
	Send.Name = Name;
	SendToCli( SockID , sizeof( Send ) , &Send );
}
//-------------------------------------------------------------------
void NetSrv_ChannelBase::_PG_ChannelBase_LtoChat_SetDSysTime	( int Sockid , int Size , void* Data )
{
	PG_ChannelBase_LtoChat_SetDSysTime*   PG = (PG_ChannelBase_LtoChat_SetDSysTime*)Data;
	This->SetDSysTime( PG->Time );
}
//-------------------------------------------------------------------
void NetSrv_ChannelBase::_PG_ChannelBase_CtoChat_DataTransmitToCli_Group ( int Sockid , int Size , void* Data )
{
	PG_ChannelBase_CtoChat_DataTransmitToCli_Group*   PG = (PG_ChannelBase_CtoChat_DataTransmitToCli_Group*)Data;
	BaseItemObject* Sender = BaseItemObject::GetObjBySockID( Sockid );

	if( Sender == NULL )
		return;

	//暫時不攔 Cli的封包資料
	switch( PG->Type )
	{
	case EM_GroupObjectType_SysChannel: 
		break;
	case EM_GroupObjectType_PlayerChannel:
		break;
	case EM_GroupObjectType_Party:
		break;
	case EM_GroupObjectType_Guild:
		break;

	}
	GroupObjectClass* Group = GroupObjectClass::GetObj( (GroupObjectTypeEnum)PG->Type , PG->ChannelID );
	if( Group == NULL )
		return;


	if( Group->CheckInGroup( Sender ) == false )
	{
		Print( LV1 , "_PG_ChannelBase_CtoChat_DataTransmitToCli_Group" , "角色不屬於此頻道 SockID=%d Account=%s" , Sockid , Sender->Account() );
		return;
	}

	This->DataTransmit_CliToCli_Group( Sender , Group , PG->DataSize , PG->Data );
}

void NetSrv_ChannelBase::_PG_ChannelBase_toChat_DataTransmitToGSrv_Group( int SockID , int Size , void* Data )
{
    PG_ChannelBase_toChat_DataTransmitToGSrv_Group*   PG = (PG_ChannelBase_toChat_DataTransmitToGSrv_Group*)Data;
    BaseItemObject* Sender = BaseItemObject::GetObjByDBID( PG->SenderDBID );

    if( Sender == NULL )
        return;

    GroupObjectClass* Group = GroupObjectClass::GetObj( (GroupObjectTypeEnum)PG->Type , PG->ChannelID );
    if( Group == NULL )
        return;

    This->DataTransmitToGSrv_Group( Sender , Group , PG->DataSize , &PG->Data );
}
void NetSrv_ChannelBase::_PG_ChannelBase_toChat_DataTransmitToCli_Group( int SockID , int Size , void* Data )
{
    PG_ChannelBase_toChat_DataTransmitToCli_Group*   PG = (PG_ChannelBase_toChat_DataTransmitToCli_Group*)Data;
    BaseItemObject* Sender = BaseItemObject::GetObjByDBID( PG->SenderDBID );

    if( Sender == NULL )
        return;

    GroupObjectClass* Group = GroupObjectClass::GetObj( (GroupObjectTypeEnum)PG->Type , PG->ChannelID );
    if( Group == NULL )
        return;

    This->DataTransmitToCli_Group( Sender , Group , PG->DataSize , &PG->Data );
}
void NetSrv_ChannelBase::_PG_ChannelBase_CtoChat_DataTransmitToCli_Name( int SockID , int Size , void* Data )
{
    PG_ChannelBase_CtoChat_DataTransmitToCli_Name*   PG = (PG_ChannelBase_CtoChat_DataTransmitToCli_Name*)Data;
	BaseItemObject* Sender = BaseItemObject::GetObjBySockID( SockID );

    if( Sender == NULL )
        return;

    BaseItemObject* Target = BaseItemObject::GetObjByRoleName( (char*)PG->Name.Begin() );
    if( Target == NULL )
	{
		SC_PlayerNotFind( SockID , PG->Name.Begin() );
        return;
	}

    This->DataTransmit_CliToCli_Name( Sender , Target , PG->DataSize , PG->Data );
}
void NetSrv_ChannelBase::_PG_ChannelBase_toChat_DataTransmitToSrv( int SockID , int Size , void* Data )
{
    PG_ChannelBase_toChat_DataTransmitToSrv*   PG = (PG_ChannelBase_toChat_DataTransmitToSrv*)Data;
    BaseItemObject* Sender = BaseItemObject::GetObjByDBID( PG->SenderDBID );

    if( Sender == NULL )
        return;

    BaseItemObject* TarObj = BaseItemObject::GetObjByDBID( PG->DBID );

    if( TarObj == NULL )
    {
        TarObj = BaseItemObject::GetObjByRoleName( (char*)PG->Name.Begin() );
        if( TarObj == NULL )
            return;
    }

    This->DataTransmitToGSrv( Sender , TarObj , PG->DataSize , &PG->Data );

}
void NetSrv_ChannelBase::_PG_ChannelBase_toChat_DataTransmitToCli( int SockID , int Size , void* Data )
{
    PG_ChannelBase_toChat_DataTransmitToCli*   PG = (PG_ChannelBase_toChat_DataTransmitToCli*)Data;
    BaseItemObject* Sender = BaseItemObject::GetObjByDBID( PG->SenderDBID );

    if( Sender == NULL )
        return;

    BaseItemObject* TarObj = BaseItemObject::GetObjByDBID( PG->DBID );

    if( TarObj == NULL )
    {
        TarObj = BaseItemObject::GetObjByRoleName( (char*)PG->Name.Begin() );
        if( TarObj == NULL )
            return;


		// 暫時發給 Client ( Mine 除錯用 ) 之後要改成系統訊息
		This->DataTransmitToCli( TarObj , Sender , PG->DataSize , &PG->Data );
    }

    This->DataTransmitToCli( Sender , TarObj , PG->DataSize , &PG->Data );
}

void NetSrv_ChannelBase::_PG_ChannelBase_LtoChat_CreateChannel ( int Sockid , int Size , void* Data )
{
    PG_ChannelBase_LtoChat_CreateChannel*   PG = (PG_ChannelBase_LtoChat_CreateChannel*)Data;
    BaseItemObject* Sender = BaseItemObject::GetObjByDBID( PG->UserDBID );

    if( Sender == NULL )
        return;

    This->CreateChannel( Sender , PG->Type , (char*)PG->Name.Begin() , (char*)PG->Password.Begin() );
}
void NetSrv_ChannelBase::_PG_ChannelBase_LtoChat_AddUser       ( int Sockid , int Size , void* Data )
{
    PG_ChannelBase_LtoChat_AddUser*   PG = (PG_ChannelBase_LtoChat_AddUser*)Data;
    BaseItemObject* Sender = BaseItemObject::GetObjByDBID( PG->UserDBID );

    if( Sender == NULL )
        return;

    This->AddUser( Sender , PG->Type , PG->ChannelID , (char*)PG->ChannelName.Begin() , (char*)PG->Password.Begin() , PG->IsForceJoin );
}
void NetSrv_ChannelBase::_PG_ChannelBase_LtoChat_DelUser       ( int Sockid , int Size , void* Data )
{
    PG_ChannelBase_LtoChat_DelUser*   PG = (PG_ChannelBase_LtoChat_DelUser*)Data;
    BaseItemObject* Sender = BaseItemObject::GetObjByDBID( PG->UserDBID );

    if( Sender == NULL )
        return;

    This->DelUser( Sender , PG->Type , PG->ChannelID );
}
void NetSrv_ChannelBase::_PG_ChannelBase_LtoChat_ChangeRoleInfo( int Sockid , int Size , void* Data )
{
    PG_ChannelBase_LtoChat_ChangeRoleInfo* PG = (PG_ChannelBase_LtoChat_ChangeRoleInfo*)Data;
    This->ChangeRoleInfo(  (char*)PG->Account.Begin() , PG->Info );
}

void NetSrv_ChannelBase::_PG_ChannelBase_CtoChat_ChannelPlayerList		( int Sockid , int Size , void* Data )
{

	PG_ChannelBase_CtoChat_ChannelPlayerList*   PG = (PG_ChannelBase_CtoChat_ChannelPlayerList*)Data;
	BaseItemObject* Sender = BaseItemObject::GetObjBySockID( Sockid );

	if( Sender == NULL )
		return;

	This->ChannelPlayerList( Sender , PG->Type , PG->ChannelID );	
}
void NetSrv_ChannelBase::_PG_ChannelBase_CtoChat_JoinPlayerDisabled		( int Sockid , int Size , void* Data )
{
	PG_ChannelBase_CtoChat_JoinPlayerDisabled*   PG = (PG_ChannelBase_CtoChat_JoinPlayerDisabled*)Data;
	BaseItemObject* Sender = BaseItemObject::GetObjBySockID( Sockid );

	if( Sender == NULL )
		return;

	This->JoinPlayerDisabled( Sender , PG->Type , PG->ChannelID , PG->JoinEnabled );	
}
void NetSrv_ChannelBase::_PG_ChannelBase_CtoChat_KickPlayer				( int Sockid , int Size , void* Data )
{
	PG_ChannelBase_CtoChat_KickPlayer*   PG = (PG_ChannelBase_CtoChat_KickPlayer*)Data;
	BaseItemObject* Sender = BaseItemObject::GetObjBySockID( Sockid );

	if( Sender == NULL )
		return;

	This->KickPlayer( Sender , PG->Type , PG->ChannelID , (char*)PG->PlayerName.Begin() );	
}
void NetSrv_ChannelBase::_PG_ChannelBase_CtoChat_SetManager				( int Sockid , int Size , void* Data )
{
	PG_ChannelBase_CtoChat_SetManager*   PG = (PG_ChannelBase_CtoChat_SetManager*)Data;
	BaseItemObject* Sender = BaseItemObject::GetObjBySockID( Sockid );

	if( Sender == NULL )
		return;

	This->SetManager( Sender , PG->Type , PG->ChannelID , (char*)PG->PlayerName.Begin() );	
}
void NetSrv_ChannelBase::_PG_ChannelBase_CtoChat_ZoneChannelID				( int Sockid , int Size , void* Data )
{
	PG_ChannelBase_CtoChat_ZoneChannelID*   PG = (PG_ChannelBase_CtoChat_ZoneChannelID*)Data;
	BaseItemObject* Sender = BaseItemObject::GetObjBySockID( Sockid );

	if( Sender == NULL )
		return;

	This->ZoneChannelID( Sender , PG->MapZoneID );	
}
//-------------------------------------------------------------------

//-------------------------------------------------------------------
void NetSrv_ChannelBase::CreateChannelOK     ( int GSrvID , GroupObjectTypeEnum Type , int ChannelID , int UserDBID , char* Name )
{
    PG_ChannelBase_ChattoL_CreateChannelResult Send;
    Send.Result     = true;
    Send.UserDBID   = UserDBID;
    Send.Type       = Type;
    Send.ChannelID  = ChannelID;
	Send.Name		= Name;
    SendToLocal( GSrvID , sizeof( Send ) , &Send );
}
void NetSrv_ChannelBase::CreateChannelFalse  ( int GSrvID , GroupObjectTypeEnum Type , int UserDBID , char* Name )
{
    PG_ChannelBase_ChattoL_CreateChannelResult Send;
    Send.Result     = false;
    Send.UserDBID   = UserDBID;
    Send.Type       = Type;
    Send.ChannelID  = -1;
	Send.Name = Name;

    SendToLocal( GSrvID , sizeof( Send ) , &Send );
}
void NetSrv_ChannelBase::AddUserOK           ( int GSrvID , GroupObjectTypeEnum Type , int ChannelID , int UserDBID , char* ChannelName , int ChannelOwnerDBID , char* ChannelOwnerName )
{
    PG_ChannelBase_ChattoL_AddUserResult    Send;
    Send.Result     = true;
    Send.Type       = Type;
    Send.UserDBID   = UserDBID;
    Send.ChannelID  = ChannelID;
	Send.ChannelName = ChannelName;
	Send.ChannelOwnerDBID = ChannelOwnerDBID;
	Send.ChannelOwnerName = ChannelOwnerName;
    
    SendToLocal( GSrvID , sizeof( Send ) , &Send );
}
void NetSrv_ChannelBase::AddUserFalse        ( int GSrvID , GroupObjectTypeEnum Type , int ChannelID , int UserDBID , char* ChannelName  )
{
    PG_ChannelBase_ChattoL_AddUserResult    Send;
    Send.Result     = false;
    Send.Type       = Type;
    Send.UserDBID   = UserDBID;
    Send.ChannelID  = ChannelID;
	Send.ChannelName = ChannelName;
	Send.ChannelOwnerDBID = -1;

    SendToLocal( GSrvID , sizeof( Send ) , &Send );
}
void NetSrv_ChannelBase::DelUserOK           ( int GSrvID , GroupObjectTypeEnum Type , int ChannelID , int UserDBID  )
{
    PG_ChannelBase_ChattoL_DelUserResult Send;
    Send.ChannelID  = ChannelID;
    Send.Result     = true;
    Send.Type       = Type;
    Send.UserDBID   = UserDBID;

    SendToLocal( GSrvID , sizeof( Send ) , &Send );
}
void NetSrv_ChannelBase::DelUserFalse        ( int GSrvID , GroupObjectTypeEnum Type , int ChannelID , int UserDBID  )
{
    PG_ChannelBase_ChattoL_DelUserResult Send;
    Send.ChannelID  = ChannelID;
    Send.Result     = false;
    Send.Type       = Type;
    Send.UserDBID   = UserDBID;

    SendToLocal( GSrvID , sizeof( Send ) , &Send );
}

void NetSrv_ChannelBase::SL_AddorSetOnlinePlayer  ( OnlinePlayerInfoStruct& Info )
{
	PG_ChannelBase_ChattoL_AddorSetOnlinePlayer Send;
	Send.Info = Info;
	//SendToAllLocal( sizeof( Send ) , &Send );
	SendToAllServer( sizeof( Send ) , &Send );
	
}
void NetSrv_ChannelBase::SL_AddorSetOnlinePlayer  ( int SockID , OnlinePlayerInfoStruct& Info )
{
	PG_ChannelBase_ChattoL_AddorSetOnlinePlayer Send;
	Send.Info = Info;
//	SendToSrv( LocalID , sizeof( Send ) , &Send );
	_Net->SendToSrv( SockID , sizeof( Send ) , &Send );
}

void NetSrv_ChannelBase::SL_DelOnlinePlayer  ( char* Name , int DBID )
{
	PG_ChannelBase_ChattoL_DelOnlinePlayer Send;
	Send.Name = Name;
	Send.DBID = DBID;
	//SendToAllLocal( sizeof( Send ) , &Send );
	SendToAllServer( sizeof( Send ) , &Send );
}
//頻道玩家列表
void NetSrv_ChannelBase::ChannelPlayerListResult( int SockID , GroupObjectTypeEnum Type , int ChannelID , vector< string > &Name )
{
	PG_ChannelBase_ChattoC_ChannelPlayerListResult Send;
	Send.Count = (int)Name.size();
	Send.ChannelID = ChannelID;
	Send.Type = Type;
	for( unsigned int i = 0 ; i < Name.size() ; i++ )
	{
		Send.PlayerNameList[i] = Name[i].c_str();
	}
	SendToCli( SockID , Send.PGSize() , &Send );
}
//通知所有此頻道的玩家
void NetSrv_ChannelBase::JoinPlayerDisabledResult( int SockID , int ChannelID , GroupObjectTypeEnum Type , bool JoinEnabled )
{
	PG_ChannelBase_ChattoC_JoinPlayerDisabledResult Send;

	Send.ChannelID = ChannelID;
	Send.Type = Type;
	Send.JoinEnabled = JoinEnabled;

	SendToCli( SockID , sizeof( Send ) , &Send );
}

//通知玩家有那些系統頻道
void NetSrv_ChannelBase::SysChannelInfo( int SockID , GroupObjectTypeEnum Type , int ChannelID , char* Name )
{
	PG_ChannelBase_ChattoC_SysChannel Send;
	Send.Type = Type;
	Send.ChannelID = ChannelID;
	Send.Name = Name;

	SendToCli( SockID , sizeof( Send ) , &Send );
}

//通知Server 系統時間
void NetSrv_ChannelBase::SSrv_SystemTime( int LocalID , int NowTime )
{
	PG_ChannelBase_ChattoAll_SystemTime Send;
	Send.Time = NowTime;
	Send.TimeZone = RoleDataEx::G_TimeZone;
	SendToLocal( LocalID , sizeof( Send ) , &Send );
}

//通知Server 系統時間
void NetSrv_ChannelBase::SCli_SystemTime( int SockID , int NowTime )
{
	PG_ChannelBase_ChattoAll_SystemTime Send;
	Send.Time = NowTime;
	Send.TimeZone = RoleDataEx::G_TimeZone;
	_Net->SendToCli( SockID , sizeof( Send ) , &Send );
}

void NetSrv_ChannelBase::SSrv_SendAll_SystemTime( int NowTime )
{
	PG_ChannelBase_ChattoAll_SystemTime Send;
	Send.Time = NowTime;
	Send.TimeZone = RoleDataEx::G_TimeZone;
	SendToAllLocal( sizeof( Send ) , &Send );
}
void NetSrv_ChannelBase::SCli_SendAll_SystemTime( int NowTime )
{
	PG_ChannelBase_ChattoAll_SystemTime Send;
	Send.Time = NowTime;
	Send.TimeZone = RoleDataEx::G_TimeZone;
	SendToAllCli( sizeof( Send ) , &Send );
}
void NetSrv_ChannelBase::SC_AddUserNotify( int SockID , GroupObjectTypeEnum Type , int ChannelID , const  char* Name )
{
	PG_ChannelBase_ChattoC_AddUserNotify Send;
	Send.Type = Type;
	Send.ChannelID = ChannelID;
	Send.Name = Name;    

	_Net->SendToCli( SockID , sizeof( Send ) , &Send );
}
void NetSrv_ChannelBase::SC_DelUserNotify( int SockID , GroupObjectTypeEnum Type , int ChannelID , const  char* Name )
{
	PG_ChannelBase_ChattoC_DelUserNotify Send;
	Send.Type = Type;
	Send.ChannelID = ChannelID;
	Send.Name = Name;    
	_Net->SendToCli( SockID , sizeof( Send ) , &Send );
}

void NetSrv_ChannelBase::SC_SetManagerResult	(  int SockID , GroupObjectTypeEnum Type , int ChannelID , bool Result )
{
	PG_ChannelBase_ChattoC_SetManagerResult Send;
	Send.Type = Type;
	Send.ChannelID = ChannelID;
	Send.Result = Result;
	_Net->SendToCli( SockID , sizeof( Send ) , &Send );
}
void NetSrv_ChannelBase::SC_ChannelOwner		( int SockID , GroupObjectTypeEnum Type , int ChannelID , char* OwnerName , int OwnerDBID )
{
	PG_ChannelBase_ChattoC_ChannelOwner Send;
	Send.Type = Type;
	Send.ChannelID = ChannelID;
	Send.OwnerName = OwnerName;
	Send.OwnerDBID = OwnerDBID;
	_Net->SendToCli( SockID , sizeof( Send ) , &Send );

}