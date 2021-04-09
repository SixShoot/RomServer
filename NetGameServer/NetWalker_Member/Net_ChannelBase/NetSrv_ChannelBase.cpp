#include "NetSrv_ChannelBase.h"
#include "../../MainProc/Global.h"
#include "../NetWakerGSrvInc.h"
//-------------------------------------------------------------------
NetSrv_ChannelBase*    NetSrv_ChannelBase::This         = NULL;
//-------------------------------------------------------------------
bool NetSrv_ChannelBase::_Init()
{
    Global::Net_ServerList->RegServerLoginEvent ( EM_SERVER_TYPE_CHAT , _OnChatConnect );
	Global::Net_ServerList->RegServerLogoutEvent( EM_SERVER_TYPE_CHAT , _OnChatDisconnect );
    _Net->RegOnSrvPacketFunction	( EM_PG_ChannelBase_ChattoL_CreateChannelResult , _PG_ChannelBase_ChattoL_CreateChannelResult   );
    _Net->RegOnSrvPacketFunction	( EM_PG_ChannelBase_ChattoL_AddUserResult       , _PG_ChannelBase_ChattoL_AddUserResult         );
    _Net->RegOnSrvPacketFunction	( EM_PG_ChannelBase_ChattoL_DelUserResult       , _PG_ChannelBase_ChattoL_DelUserResult         );

    _Net->RegOnSrvPacketFunction	( EM_PG_ChannelBase_ChattoAll_SystemTime        , _PG_ChannelBase_ChattoAll_SystemTime         );
//	_Net->RegOnSrvPacketFunction	( EM_PG_ChannelBase_ChattoL_AddorSetOnlinePlayer, _PG_ChannelBase_ChattoL_AddorSetOnlinePlayer  );
//	_Net->RegOnSrvPacketFunction	( EM_PG_ChannelBase_ChattoL_DelOnlinePlayer     , _PG_ChannelBase_ChattoL_DelOnlinePlayer       );

	Srv_NetCliReg( PG_ChannelBase_CtoL_CreateChannel );
	Srv_NetCliReg( PG_ChannelBase_CtoL_JoinChannel	 );
	Srv_NetCliReg( PG_ChannelBase_CtoL_LeaveChannel	 );

    return true;
}
//-------------------------------------------------------------------
bool NetSrv_ChannelBase::_Release()
{
    return true;
}
//-------------------------------------------------------------------
void NetSrv_ChannelBase::_PG_ChannelBase_ChattoAll_SystemTime	    ( int Sockid , int Size , void* Data )
{

	PG_ChannelBase_ChattoAll_SystemTime* PG = (PG_ChannelBase_ChattoAll_SystemTime*)Data;

	This->OnSystemTimeNotify( PG->Time );
}
/*
void NetSrv_ChannelBase::_PG_ChannelBase_ChattoL_AddorSetOnlinePlayer	( int Sockid , int Size , void* Data )
{
	PG_ChannelBase_ChattoL_AddorSetOnlinePlayer* PG = ( PG_ChannelBase_ChattoL_AddorSetOnlinePlayer* )Data;
	This->RChat_AddorSetOnlinePlayer( PG->Info );
}
void NetSrv_ChannelBase::_PG_ChannelBase_ChattoL_DelOnlinePlayer	( int Sockid , int Size , void* Data )
{
	PG_ChannelBase_ChattoL_DelOnlinePlayer* PG = ( PG_ChannelBase_ChattoL_DelOnlinePlayer* )Data;
	This->RChat_DelOnlinePlayer( (char*)PG->Name.Begin() , PG->DBID );
}
*/

void NetSrv_ChannelBase::_PG_ChannelBase_ChattoL_CreateChannelResult ( int Sockid , int Size , void* Data )
{
    PG_ChannelBase_ChattoL_CreateChannelResult* PG = ( PG_ChannelBase_ChattoL_CreateChannelResult* )Data;
    if( PG->Result != false )
        This->CreateChannelOK( PG->Type , PG->ChannelID , PG->UserDBID , (char*)PG->Name.Begin() );
    else
        This->CreateChannelFalse( PG->Type , PG->UserDBID , (char*)PG->Name.Begin() );
}
void NetSrv_ChannelBase::_PG_ChannelBase_ChattoL_AddUserResult       ( int Sockid , int Size , void* Data )
{
    PG_ChannelBase_ChattoL_AddUserResult* PG = (PG_ChannelBase_ChattoL_AddUserResult*)Data;
    if( PG->Result != false )
        This->AddUserOK( PG->Type , PG->ChannelID , PG->UserDBID , (char*)PG->ChannelName.Begin() , PG->ChannelOwnerDBID , (char*)PG->ChannelOwnerName.Begin() );
    else
        This->AddUserFalse( PG->Type , PG->ChannelID , PG->UserDBID , (char*)PG->ChannelName.Begin());
}
void NetSrv_ChannelBase::_PG_ChannelBase_ChattoL_DelUserResult       ( int Sockid , int Size , void* Data )
{
    PG_ChannelBase_ChattoL_DelUserResult* PG = (PG_ChannelBase_ChattoL_DelUserResult*)Data;
    if( PG->Result != false )
        This->DelUserOK( PG->Type , PG->ChannelID , PG->UserDBID );
    else
        This->DelUserFalse( PG->Type , PG->ChannelID , PG->UserDBID );
}
void NetSrv_ChannelBase::_OnChatConnect( EM_SERVER_TYPE ServerType , DWORD ZoneID )
{
    This->OnChatConnect( ServerType , ZoneID );
}
void NetSrv_ChannelBase::_OnChatDisconnect( EM_SERVER_TYPE ServerType , DWORD ZoneID )
{
	This->OnChatDisconnect( ServerType , ZoneID );
}

//-------------------------------------------------------------------
//Client端來的要求
void NetSrv_ChannelBase::_PG_ChannelBase_CtoL_CreateChannel		( int Sockid , int Size , void* Data )
{
	PG_ChannelBase_CtoL_CreateChannel* PG = (PG_ChannelBase_CtoL_CreateChannel*)Data;
	
	BaseItemObject* Player = BaseItemObject::GetObjBySockID( Sockid );

	if( Player == NULL )
		return;

	This->R_CreateChannel( Player  , (char*)PG->ChannelName.Begin() , (char*)PG->Password.Begin() );
}
void NetSrv_ChannelBase::_PG_ChannelBase_CtoL_JoinChannel		( int Sockid , int Size , void* Data )
{
	PG_ChannelBase_CtoL_JoinChannel* PG = (PG_ChannelBase_CtoL_JoinChannel*)Data;

	BaseItemObject* Player = BaseItemObject::GetObjBySockID( Sockid );

	if( Player == NULL )
		return;

	This->R_JoinChannel( Player  , (char*)PG->ChannelName.Begin(), (char*)PG->Password.Begin() , PG->IsForceJoin );
}
void NetSrv_ChannelBase::_PG_ChannelBase_CtoL_LeaveChannel		( int Sockid , int Size , void* Data )
{
	PG_ChannelBase_CtoL_LeaveChannel* PG = (PG_ChannelBase_CtoL_LeaveChannel*)Data;

	BaseItemObject* Player = BaseItemObject::GetObjBySockID( Sockid );

	if( Player == NULL )
		return;

	This->R_LeaveChannel( Player  , PG->ChannelID  );
}
//-------------------------------------------------------------------
void NetSrv_ChannelBase::DataTransmitToGSrv_Group( int SenderDBID , GroupObjectTypeEnum Type , int ChannelID , int Size , const void* Data )
{
    PG_ChannelBase_toChat_DataTransmitToGSrv_Group  Send;
    Send.ChannelID = ChannelID;
    Send.DataSize = Size;
    memcpy( &Send.Data , Data , Size );
    Send.SenderDBID = SenderDBID;
    Send.Type = Type;

    SendToChat( Send.Size() , &Send );
}
void NetSrv_ChannelBase::DataTransmitToCli_Group ( int SenderDBID , GroupObjectTypeEnum Type , int ChannelID , int Size , const void* Data )
{
    
    PG_ChannelBase_toChat_DataTransmitToCli_Group    Send;
    Send.ChannelID = ChannelID;
    Send.DataSize = Size;
    memcpy( &Send.Data , Data , Size );
    Send.SenderDBID = SenderDBID;
    Send.Type = Type;

    SendToChat( Send.Size() , &Send );
}
/*
void NetSrv_ChannelBase::DataTransmitToGSrv_DBID ( int SenderDBID , int TargetDBID  , int Size , const void* Data )
{
    PG_ChannelBase_toChat_DataTransmitToSrv    Send;
    Send.SenderDBID = SenderDBID;
    Send.DataSize   = Size;
    Send.DBID       = TargetDBID;
    Send.Name.Clear();
    memcpy( &Send.Data , Data , Size );

    SendToChat( Send.Size() , &Send );
}
*/
/*
void NetSrv_ChannelBase::DataTransmitToCli_DBID  ( int SenderDBID, int TargetDBID , int Size , const void* Data )
{
    PG_ChannelBase_toChat_DataTransmitToCli    Send;
    Send.SenderDBID = SenderDBID;
    Send.DataSize   = Size;
    Send.DBID       = TargetDBID;
    Send.Name.Clear();
    memcpy( &Send.Data , Data , Size );

    SendToChat( Send.Size() , &Send );
}
*/
/*
void NetSrv_ChannelBase::DataTransmitToGSrv_RoleName ( int SenderDBID , const char* Name , int Size , const void* Data )
{
    PG_ChannelBase_toChat_DataTransmitToSrv   Send;
    Send.SenderDBID = SenderDBID;
    Send.DataSize   = Size;
    Send.DBID       = -1;
    memcpy( &Send.Data , Data , Size );
   // MyStrcpy( Send.Name , Name , sizeof(Send.Name) );
	Send.Name = Name;

    SendToChat( Send.Size() , &Send );
}
*/
/*
void NetSrv_ChannelBase::DataTransmitToCli_RoleName  ( int SenderDBID, const char* Name , int Size , const void* Data )
{
    PG_ChannelBase_toChat_DataTransmitToCli    Send;
    Send.SenderDBID = SenderDBID;
    Send.DataSize   = Size;
    Send.DBID       = -1;
    memcpy( &Send.Data , Data , Size );
//    MyStrcpy( Send.Name , Name , sizeof(Send.Name) );
	Send.Name = Name;

    SendToChat( Send.Size() , &Send );
}
*/

void NetSrv_ChannelBase::CreateChannel  ( int SenderDBID , GroupObjectTypeEnum Type , char* ChannelName , char* Password )
{
    PG_ChannelBase_LtoChat_CreateChannel    Send;
    Send.Type = Type;
    Send.UserDBID = SenderDBID;
	Send.Name = ChannelName;
	Send.Password = Password;

    SendToChat( sizeof( Send ) , &Send );
}
void NetSrv_ChannelBase::AddUser        ( int SenderDBID , int AddDBID , GroupObjectTypeEnum Type , int ChannelID , char* Password)
{
    PG_ChannelBase_LtoChat_AddUser  Send;
    Send.ChannelID = ChannelID;
    Send.SenderDBID = SenderDBID;
    Send.Type = Type;
    Send.UserDBID = AddDBID;
	Send.Password = Password;
	

    SendToChat( sizeof( Send ) , &Send );
}

void NetSrv_ChannelBase::AddUser_PlayerChannel( int SenderDBID , char* ChannelName, char* Password , bool IsForceJoin )
{
	PG_ChannelBase_LtoChat_AddUser  Send;
	Send.ChannelID = -1;
	Send.SenderDBID = SenderDBID;
	Send.Type = EM_GroupObjectType_PlayerChannel;
	Send.UserDBID = SenderDBID;
	Send.Password = Password;
	Send.ChannelName = ChannelName;
	Send.IsForceJoin = IsForceJoin;

	SendToChat( sizeof( Send ) , &Send );
}

void NetSrv_ChannelBase::DelUser        ( int DelDBID , GroupObjectTypeEnum Type , int ChannelID )
{
    PG_ChannelBase_LtoChat_DelUser Send;
    Send.ChannelID = ChannelID;
    Send.Type = Type;
    Send.UserDBID = DelDBID;

    SendToChat( sizeof( Send ) , &Send );
}

void NetSrv_ChannelBase::ChangeRoleInfo( const char* Account , OnlinePlayerInfoStruct& Info )
{
    PG_ChannelBase_LtoChat_ChangeRoleInfo	Send;
    //角色資訊
	Send.Account = Account;
	Send.Info	 = Info;

    SendToChat( sizeof( Send ) , &Send );
}

void NetSrv_ChannelBase::SetDSysTime( int DSysTime )
{
   PG_ChannelBase_LtoChat_SetDSysTime	Send;
   Send.Time = DSysTime;
   SendToChat( sizeof( Send ) , &Send );
}
//-------------------------------------------------------------------
//-------------------------------------------------------------------
//  送封包 Client
//-------------------------------------------------------------------
void NetSrv_ChannelBase::S_CreateChannelOK( int GUID , GroupObjectTypeEnum Type , int ChannelID , char* ChannelName )
{
	PG_ChannelBase_LtoC_CreateChannelResult Send;
	Send.Result = true;
	Send.Type = Type;
	Send.ChannelID = ChannelID;
	Send.ChannelName = ChannelName;

	Global::SendToCli_ByGUID( GUID , sizeof( Send ) , &Send );
}
void NetSrv_ChannelBase::S_CreateChannelFalse( int GUID , GroupObjectTypeEnum Type , char* ChannelName )
{
	PG_ChannelBase_LtoC_CreateChannelResult Send;
	Send.Result = false;
	Send.Type = Type;
	Send.ChannelName = ChannelName;

	Global::SendToCli_ByGUID( GUID , sizeof( Send ) , &Send );
}
void NetSrv_ChannelBase::S_JoinChannelOK( int GUID , GroupObjectTypeEnum Type , int ChannelID , char* ChannelName, int ChannelOwnerDBID , char* ChannelOwnerName )
{
	PG_ChannelBase_LtoC_JoinChannelResult Send;
	Send.ChannelID = ChannelID;
	Send.Type = Type;
	Send.Result = true;
	Send.ChannelName = ChannelName;
	Send.ChannelOwnerDBID = ChannelOwnerDBID;
	Send.ChannelOwnerName = ChannelOwnerName;
	Global::SendToCli_ByGUID( GUID , sizeof( Send ) , &Send );
}
void NetSrv_ChannelBase::S_JoinChannelFalse( int GUID , GroupObjectTypeEnum Type , int ChannelID , char* ChannelName )
{
	PG_ChannelBase_LtoC_JoinChannelResult Send;
	Send.ChannelID = ChannelID;
	Send.Type = Type;
	Send.Result = false;
	Send.ChannelName = ChannelName;
	Global::SendToCli_ByGUID( GUID , sizeof( Send ) , &Send );
}
void NetSrv_ChannelBase::S_LeaveChannelOK( int GUID , GroupObjectTypeEnum Type , int ChannelID )
{
	PG_ChannelBase_LtoC_LeaveChannelResult Send;
	Send.ChannelID = ChannelID;
	Send.Type = Type;
	Send.Result = true;
	Global::SendToCli_ByGUID( GUID , sizeof( Send ) , &Send );
}
void NetSrv_ChannelBase::S_LeaveChannelFalse( int GUID , GroupObjectTypeEnum Type , int ChannelID )
{
	PG_ChannelBase_LtoC_LeaveChannelResult Send;
	Send.ChannelID = ChannelID;
	Send.Type = Type;
	Send.Result = false;
	Global::SendToCli_ByGUID( GUID , sizeof( Send ) , &Send );
}
void NetSrv_ChannelBase::S_ChangeChannelOwner( GroupObjectTypeEnum Type , int ChannelID , int PlayerDBID )
{
	PG_ChannelBase_ChattoL_SetChannelOwner Send;
	Send.Type = Type;
	Send.ChannelID = ChannelID;
	Send.PlayerDBID = PlayerDBID;
	SendToChat( sizeof( Send ) , &Send );
}