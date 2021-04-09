#include "NetChat_Talk.h"
#include "../../MainProc/Global.h"
//-------------------------------------------------------------------
NetSrv_Talk*    NetSrv_Talk::This         = NULL;
//-------------------------------------------------------------------
bool NetSrv_Talk::_Init()
{
	Srv_NetCliReg( PG_Talk_Channel	 );
	_Net->RegOnSrvPacketFunction	( EM_PG_Talk_LtoChat_SysChannel    , _PG_Talk_LtoChat_SysChannel );


	_Net->RegOnCliConnectFunction       ( _OnCliConnect );
	_Net->RegOnCliDisconnectFunction    ( _OnCliDisconnect );

	Srv_NetCliReg( PG_Talk_CtoChat_RegisterBillBoardInfo	 );
	Srv_NetCliReg( PG_Talk_CtoChat_BillBoardLiveTime		 );
    return true;
}
//-------------------------------------------------------------------
bool NetSrv_Talk::_Release()
{
    return true;
}
//-------------------------------------------------------------------
void NetSrv_Talk::_OnCliConnect( int CliID , string Account )
{
	This->OnCliConnect( CliID , (char*)Account.c_str() );
}
//-------------------------------------------------------------------
void NetSrv_Talk::_OnCliDisconnect( int CliID )
{
	This->OnCliDisconnect( CliID );
}

void NetSrv_Talk::_PG_Talk_CtoChat_RegisterBillBoardInfo	( int Sockid , int Size , void* Data )
{
	PG_Talk_CtoChat_RegisterBillBoardInfo* PG = (PG_Talk_CtoChat_RegisterBillBoardInfo*)Data;
	This->R_RegisterBillBoardInfo( Sockid , PG->DataSize , PG->Data );
}

void NetSrv_Talk::_PG_Talk_CtoChat_BillBoardLiveTime		( int Sockid , int Size , void* Data )
{
	PG_Talk_CtoChat_BillBoardLiveTime* PG = (PG_Talk_CtoChat_BillBoardLiveTime*)Data;
	This->R_BillBoardLiveTime( Sockid , PG->LiveTime );
}

void NetSrv_Talk::_PG_Talk_LtoChat_SysChannel( int Sockid , int Size , void* Data )
{
	PG_Talk_LtoChat_SysChannel* PG = (PG_Talk_LtoChat_SysChannel*)Data;
	This->R_SysChannel( PG->PlayerDBID , PG->Type , PG->ChannelID , (char*)PG->Name.Begin() , (char*)PG->Content.Begin() , PG->SenderInfo );
}
//-------------------------------------------------------------------
void NetSrv_Talk::_PG_Talk_Channel	( int Sockid , int Size , void* Data )
{
	PG_Talk_Channel* PG = (PG_Talk_Channel*)Data;
	This->R_Channel( Sockid ,  PG->Type , PG->ChannelID , (char*)PG->Name.Begin() , (char*)PG->Content.Begin() , PG->SenderInfo );
}
//-------------------------------------------------------------------
void NetSrv_Talk::TalkTo( Voc_ENUM Job , int SockID , char* TalkName , char* Context )
{
	PG_Talk_TalkTo Send;

    BaseItemObject* pObj = BaseItemObject::GetObjBySockID( SockID  );
    if( pObj == NULL )
        return;
    ChatRoleData*		Role	 = pObj->Role();

	Send.Content		= Context;
	Send.ContentSize	= Send.Content.Size()+1;
	Send.SenderName		= Role->RoleName;
	Send.TargetName		= TalkName;
	Send.Job			= Job;

	Global::SendToCli( SockID , Send.Size() , &Send );
}
//-------------------------------------------------------------------
void NetSrv_Talk::SysMsg( int SockID , char* Msg )
{
	PG_Talk_LtoC_SystemMsg Send;
	Send.Content = Msg;
	Send.ContentSize = Send.Content.Size() + 1;

	Global::SendToCli( SockID , Send.Size() , &Send );
}
void NetSrv_Talk::SysMsg_ChartoUtf8( int SockID , char* Msg )
{
	SysMsg( SockID , (char*)CharToUtf8( Msg ).c_str() );
}
//-------------------------------------------------------------------
void NetSrv_Talk::Channel( int SockID , Voc_ENUM Job , GroupObjectTypeEnum Type , int ID , TalkChannelTypeENUM	MsgType , char* TalkName , char* Context , TalkSenderInfo& SenderInfo )
{
	PG_Talk_Channel Send;
	Send.Type = Type;
	Send.ChannelID= ID;
	Send.Content = Context;
	Send.ContentSize = Send.Content.Size() + 1;
	Send.MsgType = MsgType;
	Send.SenderInfo = SenderInfo;
	Send.Job = Job;

	Global::SendToCli( SockID , Send.Size() , &Send );

}
//-------------------------------------------------------------------
void	NetSrv_Talk::Send_Channel( Voc_ENUM Job , GroupObjectTypeEnum Type , int ChannelID , TalkChannelTypeENUM	MsgType , char* Name  , char* Context , TalkSenderInfo& SenderInfo )
{
	GroupObjectClass* Group = GroupObjectClass::GetObj( Type , ChannelID );
	if( Group == NULL )
		return;

	PG_Talk_Channel Send;

	Send.Name = Name;
	Send.Content = Context;
	Send.Type = Type;
	Send.ChannelID = ChannelID;
	Send.ContentSize = Send.Content.Size()+1;
	Send.MsgType = MsgType;
	Send.SenderInfo = SenderInfo;
	Send.Job = Job;

	set< int >* UserList = Group->UserList_SockID();
	set< int >::iterator Iter;
	

	for( Iter = UserList->begin() ; Iter != UserList->end() ; Iter ++ )
	{
		int SockID = *Iter;
		SendToCli( SockID , Send.Size() , &Send );
	}   
}
//-------------------------------------------------------------------
void	NetSrv_Talk::Send_SysChannel( Voc_ENUM Job , int ChannelID , TalkChannelTypeENUM MsgType , char* Name  , char* Context , TalkSenderInfo& SenderInfo )
{

	PG_Talk_Channel Send;

	Send.Name = Name;
	Send.Content = Context;
	Send.Type = EM_GroupObjectType_SysChannel;
	Send.ChannelID = ChannelID;
	Send.ContentSize = Send.Content.Size()+1;
	Send.MsgType = MsgType;
	Send.SenderInfo = SenderInfo;
	Send.Job = Job;

	SendToAllCli( sizeof( Send ) , & Send ); 
}

void	NetSrv_Talk::Send_ZoneChannel( Voc_ENUM Job , GroupObjectClass* Group , int ChannelID , TalkChannelTypeENUM MsgType , char* Name  , char* Context , TalkSenderInfo& SenderInfo )
{

	PG_Talk_Channel Send;

	Send.Name = Name;
	Send.Content = Context;
	Send.Type = EM_GroupObjectType_SysZoneChannel;
	Send.ChannelID = ChannelID;
	Send.ContentSize = Send.Content.Size()+1;
	Send.MsgType = MsgType;
	Send.SenderInfo = SenderInfo;
	Send.Job = Job;

	set< int >* UserList = Group->UserList_SockID();
	set< int >::iterator Iter;

	for( Iter = UserList->begin() ; Iter != UserList->end() ; Iter ++ )
	{
		int SockID = *Iter;
		SendToCli( SockID , Send.Size() , &Send );
	}   

}
//-------------------------------------------------------------------
void NetSrv_Talk::GameMsgEvent	( int iSockID , GameMessageEventENUM Event )
{
	PG_GM_LtoC_SendGameMessageEvent Send;
	Send.Event = Event;
	SendToCli( iSockID  , sizeof( Send ) , &Send );
}

void NetSrv_Talk::SC_BillBoardInfo			( int Sockid , int PlayerDBID , int DataSize , char* Data )
{
	PG_Talk_ChattoC_BillBoardInfo Send;
	memcpy( Send.Data , Data , DataSize );
	Send.DataSize = DataSize;
	Send.PlayerDBID = PlayerDBID;

	SendToCli( Sockid  , Send.Size() , &Send );
}
void NetSrv_Talk::SC_All_BillBoardInfo		( int PlayerDBID , int DataSize , char* Data )
{
	PG_Talk_ChattoC_BillBoardInfo Send;
	memcpy( Send.Data , Data , DataSize );
	Send.DataSize = DataSize;
	Send.PlayerDBID = PlayerDBID;

	SendToAllPlayer( Send.Size() , &Send );
}
void NetSrv_Talk::SC_All_BillBoardDelete	( int PlayerDBID )
{
	PG_Talk_ChattoC_BillBoardDelete Send;
	Send.PlayerDBID = PlayerDBID;

	SendToAllPlayer( sizeof( Send ) , &Send );
}

//-------------------------------------------------------------------