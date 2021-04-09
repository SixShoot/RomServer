#include "../NetWakerGSrvInc.h"
#include "NetSrv_Talk.h"
#include "../../mainproc/PartyInfoList/PartyInfoList.h"
#include "../../mainproc/GuildManage/GuildManage.h"
#include "AllOnlinePlayerInfo/AllOnlinePlayerInfo.h"
//-------------------------------------------------------------------
NetSrv_Talk*    NetSrv_Talk::This         = NULL;

//-------------------------------------------------------------------
bool NetSrv_Talk::_Init()
{

	Srv_NetCliReg( PG_Talk_TalkTo						);
	Srv_NetCliReg( PG_Talk_Range						);
	Srv_NetCliReg( PG_Talk_CtoL_GMCommand				);
	Srv_NetCliReg( PG_GM_CtoL_SendMsg					);
	Srv_NetCliReg( PG_Talk_Channel						);

    _Net->RegOnSrvPacketFunction			( EM_PG_Talk_DtoL_OfflineGMCommand					, _PG_Talk_DtoL_OfflineGMCommand      				);
	/*
    _Net->RegOnCliPacketFunction	( EM_PG_Talk_TalkTo     , _PG_Talk_TalkTo 
        , PG_Talk_TalkTo::SizeBase()   , sizeof(PG_Talk_TalkTo) );

    _Net->RegOnCliPacketFunction	( EM_PG_Talk_Range     , _PG_Talk_Range 
        , PG_Talk_Range::SizeBase()   , sizeof(PG_Talk_Range) );

//    _Net->RegOnCliPacketFunction	( EM_PG_Talk_Zone     , _PG_Talk_Zone 
//        , PG_Talk_Zone::SizeBase()   , sizeof(PG_Talk_Zone) );

    _Net->RegOnCliPacketFunction	( EM_PG_Talk_CtoL_GMCommand     , _PG_Talk_CtoL_GMCommand 
        , PG_Talk_CtoL_GMCommand::SizeBase()   , sizeof(PG_Talk_CtoL_GMCommand) );

    _Net->RegOnCliPacketFunction	( EM_PG_GM_CtoL_SendMsg     , _PG_GM_CtoL_SendMsg 
        ,  sizeof(PG_GM_CtoL_SendMsg)  , sizeof(PG_GM_CtoL_SendMsg) );


	_Net->RegOnCliPacketFunction	( EM_PG_Talk_Channel     , _PG_Talk_Channel 
		, PG_Talk_Channel::SizeBase()   , sizeof(PG_Talk_Channel) );
*/
    return true;
}
//-------------------------------------------------------------------
bool NetSrv_Talk::_Release()
{
    return true;
}
//-------------------------------------------------------------------
void NetSrv_Talk::_PG_Talk_DtoL_OfflineGMCommand	( int Sockid , int Size , void* Data )
{
	PG_Talk_DtoL_OfflineGMCommand* PG = (PG_Talk_DtoL_OfflineGMCommand*) Data;

	This->RD_OfflineGMCommand( PG->PlayerDBID , PG->ManageLv , PG->Content.Begin() );
}

void NetSrv_Talk::_PG_Talk_Channel		( int Sockid , int Size , void* Data )
{
	BaseItemObject* Player = BaseItemObject::GetObjBySockID( Sockid );
	if( Player == NULL )
		return;

	PG_Talk_Channel* PG = (PG_Talk_Channel*) Data;

//	if( PG->CheckPG() == false )
//		return;

	switch( PG->Type )
	{
	case EM_GroupObjectType_SysChannel:		//系統
	case EM_GroupObjectType_SysZoneChannel:	//每個區域都有的
		This->R_SysChannel( Player , PG->Type , PG->ChannelID , PG->ContentSize , (char*)PG->Content.Begin() , PG->SenderInfo );
		break;
	case EM_GroupObjectType_PlayerChannel:	//玩家的
	case EM_GroupObjectType_Party:			//隊伍
		//This->R_Channel( Player , PG->Type , PG->ContentSize , (char*)PG->Content.Begin() );
		break;
	case EM_GroupObjectType_Guild:			//工會
		This->R_Guild( Player , PG->ContentSize , (char*)PG->Content.Begin() , PG->SenderInfo );
		break;
	}
	
}


//-------------------------------------------------------------------
void NetSrv_Talk::_PG_GM_CtoL_SendMsg     ( int Sockid , int Size , void* Data )
{
	BaseItemObject* Player = BaseItemObject::GetObjBySockID( Sockid );
	if( Player == NULL )
		return;

	PG_GM_CtoL_SendMsg* PG = (PG_GM_CtoL_SendMsg*) Data;

	if( PG->CheckPG() == false )
		return;

	This->ToGMTools( Player , (char*)PG->Content.Begin() );
}

void NetSrv_Talk::_PG_Talk_TalkTo         ( int Sockid , int Size , void* Data )
{
    BaseItemObject* Player = BaseItemObject::GetObjBySockID( Sockid );
    if( Player == NULL )
        return;

    PG_Talk_TalkTo* PG = (PG_Talk_TalkTo*) Data;

    if( PG->CheckPG() == false )
        return;

    This->Player_TalkTo( Player , PG->TargetName.Begin() , PG->ContentSize , PG->Content.Begin() , PG->SenderInfo );
}
//-------------------------------------------------------------------
void NetSrv_Talk::_PG_Talk_Range          ( int Sockid , int Size , void* Data )
{
    BaseItemObject* Player = BaseItemObject::GetObjBySockID( Sockid );
    if( Player == NULL )
        return;

    PG_Talk_Range* PG = (PG_Talk_Range*) Data;

    if( PG->CheckPG() == false )
        return;

    This->Player_Range( Player , PG->ContentSize , PG->Content.Begin() , PG->SenderInfo );
}
/*
void NetSrv_Talk::_PG_Talk_Zone           ( int Sockid , int Size , void* Data )
{
    BaseItemObject* Player = BaseItemObject::GetObjBySockID( Sockid );
    if( Player == NULL )
        return;

    PG_Talk_Zone* PG = (PG_Talk_Zone*) Data;

    if( PG->CheckPG() == false )
        return;

    This->Player_Zone( Player , PG->ContentSize , PG->Content.Begin() );
}
*/
void NetSrv_Talk::_PG_Talk_CtoL_GMCommand ( int Sockid , int Size , void* Data )
{
    BaseItemObject* Player = BaseItemObject::GetObjBySockID( Sockid );
    if( Player == NULL )
        return;

    PG_Talk_CtoL_GMCommand* PG = (PG_Talk_CtoL_GMCommand*) Data;

    if( PG->CheckPG() == false )
        return;

    This->Player_GMCommand( Player , PG->ContentSize , PG->Content.Begin() );
}
//-------------------------------------------------------------------
void NetSrv_Talk::TalkTo(  int Sender , bool IsSystem , const char* TalkerName , const char* ToName , const char* Content )
{
    PG_Talk_TalkTo Send;
    Send.ContentSize = int( min( sizeof( Send.Content) , strlen( Content ) + 1 ));
//    MyStrcpy( Send.Content , Content , sizeof( Send.Content ) );
//    MyStrcpy( Send.Name , Name , sizeof( Send.Name ) );
    BaseItemObject* pObj = BaseItemObject::GetObj( Sender  );
    if( pObj == NULL )
        return;
    RoleDataEx*	Role	 = pObj->Role();
/*
	if( Role->IsPlayer() )
	{
		Send.SenderInfo.vocation[0] = Role->TempData.Attr.Voc;
		Send.SenderInfo.vocation[1] = Role->TempData.Attr.Voc_Sub;
		Send.SenderInfo.level[0] = Role->TempData.Attr.Level.;
		Send.SenderInfo.level[1] = Role->TempData.Attr.Level_Sub;
	}
*/
    Send.Content		= Content;
	Send.SenderName		= TalkerName;
    Send.TargetName		= ToName;
	Send.IsSystem		= IsSystem;
	Send.Job			= Role->BaseData.Voc;
    
    SendToCli_ByGUID( Sender , Send.Size() , &Send );
}
//-------------------------------------------------------------------
void NetSrv_Talk::SayTo( int SendID , int SayerID , const char* Content )
{
	PG_Talk_Range Send;

	BaseItemObject* OwnerClass = BaseItemObject::GetObj( SayerID  );
	if( OwnerClass == NULL )
		return;
	RoleDataEx*		Owner	 = OwnerClass->Role();

	char szName[1024];

	if( Owner->IsPlayer() == false )
	{
		//Sys111280_name
		sprintf( szName, "Sys%d_name", Owner->BaseData.ItemInfo.OrgObjID );
	}
	else
	{
		strcpy( szName, Global::GetRoleName( Owner ) );
	}



	Send.ContentSize	= int ( min( sizeof( Send.Content) , strlen( Content ) + 1 ) );
	Send.Content		= Content;
	Send.IsSystem		= false;
	Send.Name			= szName;
	Send.GItemID		= SayerID ;
	Send.Job			= Owner->BaseData.Voc;
	
	SendToCli_ByGUID( SendID , Send.Size() , &Send );

	return;
}
void NetSrv_Talk::Range( int GItemID , bool IsSystem , const char* Content, bool bNpcGossip , TalkSenderInfo* SenderInfo )
{
    PG_Talk_Range Send;

    BaseItemObject* OwnerClass = BaseItemObject::GetObj( GItemID  );
    if( OwnerClass == NULL )
        return;
    RoleDataEx*		Owner	 = OwnerClass->Role();

	char szName[1024];

	//if( Owner->IsPlayer() == false )
	if( Owner->BaseData.RoleName.Size() == 0 && Owner->IsPlayer() == false )
	{
		//Sys111280_name
		sprintf( szName, "Sys%d_name", Owner->BaseData.ItemInfo.OrgObjID );
	}
	else
	{
		strcpy( szName, Global::GetRoleName( Owner ) );
	}



    Send.ContentSize	= int ( min( sizeof( Send.Content) , strlen( Content ) + 1 ) );
    Send.Content		= Content;
	Send.IsSystem		= IsSystem;
	Send.Name			= szName;
    Send.GItemID		= GItemID ;
	Send.IsNPCGossip	= bNpcGossip;
	Send.Job			= Owner->BaseData.Voc;;
	
	if( SenderInfo != NULL )
		Send.SenderInfo		= *SenderInfo;

    //_Def_View_Block_Range_
    PlayIDInfo**	Block = Global::PartSearch( Owner , _Def_View_Block_RangeTalk_ );

    int			i , j;
    PlayID*		TopID;

    for( i = 0 ; Block[i] != NULL ; i++ )
    {
        TopID = Block[i]->Begin;
        for( j = 0 ; TopID != NULL ; TopID = TopID->Next )
            SendToCli_ByGUID( TopID->ID , Send.Size() , &Send );

    }

    return;
}
//-------------------------------------------------------------------
void NetSrv_Talk::Yell( int GItemID , bool IsSystem , const char* Content, int iRangeBlock   )
{
	PG_Talk_Range Send;

	BaseItemObject* OwnerClass = BaseItemObject::GetObj( GItemID  );
	if( OwnerClass == NULL )
		return;
	RoleDataEx*		Owner	 = OwnerClass->Role();

	char szName[1024];

	if( Owner->IsPlayer() == false )
	{
		//Sys111280_name
		sprintf( szName, "Sys%d_name", Owner->BaseData.ItemInfo.OrgObjID );
	}
	else
	{
		strcpy( szName, Global::GetRoleName( Owner ) );
	}



	Send.ContentSize	= int ( min( sizeof( Send.Content) , strlen( Content ) + 1 ) );
	Send.Content		= Content;
	Send.IsSystem		= IsSystem;
	Send.Name			= szName;
	Send.GItemID		= GItemID ;
	Send.Job			= Owner->BaseData.Voc;

	//認定為幣商 發言只有自己看到
	if( Owner->PlayerBaseData->Defendant.MoneyTraderCount >= _DEF_MAX_MONEY_VENTOR_COUNT_ - 2 )
	{
		SendToCli_ByGUID( Owner->GUID() , Send.Size() , &Send );
		return;
	}

	//_Def_View_Block_Range_
	PlayIDInfo**	Block = Global::PartSearch( Owner , iRangeBlock );

	int			i , j;
	PlayID*		TopID;

	for( i = 0 ; Block[i] != NULL ; i++ )
	{
		TopID = Block[i]->Begin;
		for( j = 0 ; TopID != NULL ; TopID = TopID->Next )
			SendToCli_ByGUID( TopID->ID , Send.Size() , &Send );

	}

	return;
}
//-------------------------------------------------------------------
/*
void NetSrv_Talk::Zone( int GItemID  , const char* Content )
{
    PG_Talk_Zone Send;

    BaseItemObject* OwnerClass = BaseItemObject::GetObj( GItemID  );
    if( OwnerClass == NULL )
        return;
    RoleDataEx*		Owner	 = OwnerClass->Role();

    Send.ContentSize = int ( min( sizeof( Send.Content) , strlen( Content ) + 1 ) );
//    MyStrcpy( Send.Content , Content , sizeof( Send.Content ) );
//    MyStrcpy( Send.Name , Owner->BaseData.RoleName , sizeof( Send.Name ) );
    Send.Content = Content;
    Send.Name    = Owner->BaseData.RoleName;
    Send.GItemID = GItemID ;

    SendToAllCli( Send.Size() , &Send );
}*/
//-------------------------------------------------------------------
void NetSrv_Talk::SysMsg( int GItemID  , const char* Content )
{
	PG_Talk_LtoC_SystemMsg Send;

	char szBuff[1024];

	sprintf( szBuff, "[DEBUG] %s", Content );
	Send.Content = szBuff;
    Send.ContentSize = Send.Content.Size() + 1 ;
	
    

	if( GItemID != -1 )
	{
		SendToCli_ByGUID( GItemID  , Send.Size() , &Send );
	}
	else
	{
		Global::SendToAllCli( Send.Size() , & Send );
	}

}
//-------------------------------------------------------------------

void NetSrv_Talk::TalkToByRoleName( BaseItemObject* Sender , bool IsSystem , const char* ToName , const char* Content , TalkSenderInfo& SenderInfo )
{
    RoleDataEx* Role = (RoleDataEx*)Sender->Role();


	//找出目標物件
	OnlinePlayerInfoStruct* Info = AllOnlinePlayerInfoClass::This()->GetInfo_ByName( (char*) ToName );
	if( Info == NULL )
	{
		Role->Net_GameMsgEvent( EM_GameMessageEvent_TellCharacterNotFound );
		Global::Log_OfflineMsg( ToName , Role->RoleName() , Role->DBID() , Content );
		return ;
	}

    PG_Talk_TalkTo Send;

    Send.ContentSize = int( min( sizeof( Send.Content) , strlen( Content ) + 1 ));

	Send.Content	= Content;
    Send.SenderName = Role->RoleName();
	Send.TargetName	= ToName;
	Send.IsSystem   = IsSystem;
	Send.SenderInfo = SenderInfo;
	Send.Job		= Role->BaseData.Voc;

	Global::SendToCli_ByProxyID( Info->SockID , Info->ProxyID , sizeof( Send ) , &Send );
	Global::SendToCli_ByGUID( Role->GUID() , sizeof( Send ) , &Send );

	//寫Log
	Global::Log_Talk( EM_TalkLogType_Talk , 0 , Role->DBID() , Info->DBID , Content );
}
//-------------------------------------------------------------------
void	NetSrv_Talk::ScriptMessage	( int iSayerID, int iTargetID, int iType,  const char* sEmoteString, int iColor )
{
	char				szBuffer[ 4096 ];

	GamePGCommandEnum	Command			= EM_PG_Talk_LtoC_ScriptMessage;

	char*				ptr				= szBuffer;
	int					iszLen			= 0;

	// PacketID
		memcpy( (PVOID)ptr, &Command, sizeof( GamePGCommandEnum ) );
		ptr += sizeof( GamePGCommandEnum );

	// NPCID
		memcpy( (PVOID)ptr, &iSayerID, sizeof( int ) );
		ptr += sizeof( int );

	// TypeID
		memcpy( (PVOID)ptr, &iType, sizeof( int ) );
		ptr += sizeof( int );

	// Color
		memcpy( (PVOID)ptr, &iColor, sizeof( int ) );
		ptr += sizeof( int );

	// String len
		iszLen = (int)strlen( sEmoteString );
		if( iszLen != 0 )
		{
			iszLen += 1;
		}
		memcpy( ptr, &iszLen, sizeof(int) );
		ptr		+= sizeof( int );

	// String
		memcpy( ptr, sEmoteString, iszLen );
		ptr += iszLen;

	int iSize = (int)( ptr - szBuffer );

	BaseItemObject* OwnerClass = BaseItemObject::GetObj( iSayerID  );
	if( OwnerClass == NULL )
		return;

	RoleDataEx*		Owner	 = OwnerClass->Role();

	int	iRoomID = Owner->BaseData.RoomID;

	// 決定發送對像

		if( iTargetID == 0 )
		{
			//_Def_View_Block_Range_
			PlayIDInfo**	Block = Global::PartSearch( Owner , _Def_View_Block_RangeTalk_ );

			int			i , j;
			PlayID*		TopID;

			for( i = 0 ; Block[i] != NULL ; i++ )
			{
				TopID = Block[i]->Begin;
				for( j = 0 ; TopID != NULL ; TopID = TopID->Next )
				{
					BaseItemObject* pObj = Global::GetObj( TopID->ID );

					if( pObj->Role()->IsPlayer() && pObj->Role()->BaseData.RoomID == iRoomID )
					{
						SendToCli_ByGUID( TopID->ID , iSize , (PVOID)szBuffer );
					}					
				}
			}
		}
		else
		if( iTargetID == -1 )
		{
			//SendToAllCli( iSize, (PVOID)szBuffer );
			vector< BaseItemObject* >& ZoneList = *BaseItemObject::GetZoneObjList();

			for( int i = 0 ; i < (int)ZoneList.size() ; i++ )
			{
				BaseItemObject* Obj = ZoneList[i];

				if( Obj == NULL || Obj->Role()->IsPlayer() == false )
					continue;

				if( Obj->Role()->BaseData.RoomID != iRoomID )
					continue;

				SendToCli_ByGUID( Obj->GUID() , iSize , (PVOID)szBuffer );
			}
		}
		else
		if( iTargetID == -2 )	// 全世界所有人
		{
			SendToAllPlayer( iSize , (PVOID)szBuffer );
		}
		else
		{
			RoleDataEx*		pTargetRole = Global::GetRoleDataByGUID( iTargetID );
			if( pTargetRole )
			{
				SendToCli_ByGUID( iTargetID, iSize, (PVOID)szBuffer );
			}
		}

	return;
}
//-------------------------------------------------------------------
void NetSrv_Talk::EventMsg ( int iRoleGUID, EM_EVENT_MSG emEvent, ... )
{
	int i = 0;
	va_list marker;
	vector< int > vecQueue;

	va_start( marker, iRoleGUID );     /* Initialize variable arguments. */

	while(1)
	{
		i = va_arg( marker, int);

		if( i != -1 )
		{
			vecQueue.push_back( i );
		}
		else
			break;
	}
	va_end( marker );              /* Reset variable arguments.      */

	int		iArraySize	=  (int)vecQueue.size() + 3;
	int		*pArray		= NEW int[ iArraySize ];

	for( int index = 3; index < (int)vecQueue.size(); index++ )
	{
		pArray[ index ] = vecQueue[ index ];
	}


	// 0 PackID / 1 EVENT_MSG / 2 SIZE / 3 ... 4 ... DATA

	pArray[0]	= EM_PG_Talk_LtoC_EventMsg;
	pArray[1]	= (int) emEvent;
	pArray[2]	= (int)vecQueue.size();

	SendToCli_ByGUID( iRoleGUID  , ( iArraySize * sizeof(int) ) , &pArray );
}
//-------------------------------------------------------------------
void NetSrv_Talk::GMMsg           ( int SenderID , const char* Content )
{
    PG_GM_LtoC_SendMsg Send;

    Send.ContentSize = (int)strlen( Content );
    MyStrcpy( (char*)Send.Content.Begin() , Content , sizeof( Send.Content ) );
    if( SenderID == -1 )
        Global::SendToAllCli( sizeof( Send ) , &Send );
    else
        SendToCli_ByGUID( SenderID  , sizeof( Send ) , &Send );

}
//-------------------------------------------------------------------
void NetSrv_Talk::Party_All       ( Voc_ENUM Job , int PartyID , TalkChannelTypeENUM MsgType , const char* Name , const char* Content , TalkSenderInfo& SenderInfo )
{
	PartyInfoStruct* PartyInfo = PartyInfoListClass::This()->GetPartyInfo( PartyID );

	if( PartyInfo == NULL )
		return;

	PG_Talk_Channel Send;

	Send.Type = EM_GroupObjectType_Party;
	Send.ChannelID = PartyID;
	Send.MsgType = MsgType;

	Send.Name = Name;
	Send.Content = Content;
	Send.ContentSize = Send.Content.Size() + 1;
	Send.SenderInfo = SenderInfo;

	for( unsigned i = 0 ; i < PartyInfo->Member.size() ; i++ )
	{
		OnlinePlayerInfoStruct* PlayerInfo = AllOnlinePlayerInfoClass::This()->GetInfo_ByDBID( PartyInfo->Member[i].DBID );
		if( PlayerInfo == NULL )
			continue;

		SendToCli_ByProxyID( PlayerInfo->SockID , PlayerInfo->ProxyID , Send.Size() , &Send );
	}
}
//-------------------------------------------------------------------
void NetSrv_Talk::Guild_All       ( Voc_ENUM Job , int GuildID , TalkChannelTypeENUM MsgType , const char* Name  , const char* Content , TalkSenderInfo& SenderInfo )
{
	PG_Talk_Channel Send;

	Send.Type = EM_GroupObjectType_Guild;
	Send.ChannelID = GuildID;
	Send.MsgType = MsgType;

	Send.Name = Name;
	Send.Content = Content;
	Send.ContentSize = Send.Content.Size() + 1 ;
	Send.SenderInfo = SenderInfo;

	GuildStruct* Guild = GuildManageClass::This()->GetGuildInfo( GuildID );
	if( Guild == NULL )
		return;

	for( unsigned i = 0 ; i < Guild->Member.size() ; i++ )
	{
		GuildMemberStruct* Member = Guild->Member[i];
		if( Member->IsOnline != false )
		{
			OnlinePlayerInfoStruct* PlayerInfo = AllOnlinePlayerInfoClass::This()->GetInfo_ByDBID( Member->MemberDBID );
			if( PlayerInfo == NULL )
				continue;

			SendToCli_ByProxyID( PlayerInfo->SockID , PlayerInfo->ProxyID , Send.Size() , &Send );
		}
	}
}

void NetSrv_Talk::GameMsgEvent	( int SenderID , GameMessageEventENUM Event )
{
	PG_GM_LtoC_SendGameMessageEvent Send;
	Send.Event = Event;
	SendToCli_ByGUID( SenderID  , sizeof( Send ) , &Send );
}

void NetSrv_Talk::GameMsgEventData	( int SenderID , int iNumArg, GameMessageEventDataENUM Event
								 , GameMessageDataTypeENUM Type1 , int Value1 
								 , GameMessageDataTypeENUM Type2 , int Value2 
								 , GameMessageDataTypeENUM Type3 , int Value3 
								 , GameMessageDataTypeENUM Type4 , int Value4 
								 , GameMessageDataTypeENUM Type5 , int Value5 )
{
	PG_GM_LtoC_GameMessageDataEvent Send;

	Send.Event		= Event;
	Send.iNumArg	= iNumArg;
	Send.Data[0].Type = Type1;	Send.Data[0].Value = Value1;
	Send.Data[1].Type = Type2;	Send.Data[1].Value = Value2;
	Send.Data[2].Type = Type3;	Send.Data[2].Value = Value3;
	Send.Data[3].Type = Type4;	Send.Data[3].Value = Value4;
	Send.Data[4].Type = Type5;	Send.Data[4].Value = Value5;

	SendToCli_ByGUID( SenderID  , sizeof( Send ) , &Send );
}


void NetSrv_Talk::SendRunningMsg		( int GUID , const char* Content )
{
	PG_Talk_LtoC_RunningMsg Send;	
	Send.Content = Content;
	Send.ContentSize = Send.Content.Size() + 1; 
	SendToCli_ByGUID( GUID  , Send.Size() , &Send );	
}
void NetSrv_Talk::SendRunningMsg_Zone	( const char* Content )
{
	PG_Talk_LtoC_RunningMsg Send;
	Send.Content = Content;
	Send.ContentSize = Send.Content.Size() + 1; 
	SendToAllCli( Send.Size() , &Send );	
}
void NetSrv_Talk::SendRunningMsg_All	( const char* Content )
{
	PG_Talk_LtoC_RunningMsg Send;
	Send.Content = Content;
	Send.ContentSize = Send.Content.Size() + 1; 
	SendToAllPlayer( Send.Size() , &Send );	
}
void NetSrv_Talk::SendRunningMsg_Room( int RoomID , const char* Content  )
{
	PG_Talk_LtoC_RunningMsg Send;
	Send.Content = Content;
	Send.ContentSize = Send.Content.Size() + 1; 
	//SendToAllPlayer( Send.Size() , &Send );	
	Global::SendToCli_ByRoomID( RoomID , Send.Size() , &Send );
}
void NetSrv_Talk::SendRunningMsgEx		( int GUID , RunningMsgExTypeENUM Type , const char* Content )
{
	PG_Talk_LtoC_RunningMsgEx Send;	
	Send.Type = Type;
	Send.Content = Content;
	Send.ContentSize = Send.Content.Size() + 1; 
	SendToCli_ByGUID( GUID  , Send.Size() , &Send );	
}
void NetSrv_Talk::SendRunningMsgEx_Zone	( RunningMsgExTypeENUM Type , const char* Content )
{
	PG_Talk_LtoC_RunningMsgEx Send;
	Send.Type = Type;
	Send.Content = Content;
	Send.ContentSize = Send.Content.Size() + 1; 
	SendToAllCli( Send.Size() , &Send );	
}
void NetSrv_Talk::SendRunningMsgEx_All	( RunningMsgExTypeENUM Type , const char* Content )
{
	PG_Talk_LtoC_RunningMsgEx Send;
	Send.Type = Type;
	Send.Content = Content;
	Send.ContentSize = Send.Content.Size() + 1; 
	SendToAllPlayer( Send.Size() , &Send );	
}
void NetSrv_Talk::SendRunningMsgEx_Room( int RoomID , RunningMsgExTypeENUM Type , const char* Content  )
{
	PG_Talk_LtoC_RunningMsgEx Send;
	Send.Type = Type;
	Send.Content = Content;
	Send.ContentSize = Send.Content.Size() + 1; 
	//SendToAllPlayer( Send.Size() , &Send );	
	Global::SendToCli_ByRoomID( RoomID , Send.Size() , &Send );
}
//送系統頻道
void NetSrv_Talk::SChat_SysChannel( int PlayerDBID , GroupObjectTypeEnum Type , TalkChannelTypeENUM MsgType , int ChannelID , char* Name  , char* Context , TalkSenderInfo& SenderInfo  )
{
	PG_Talk_LtoChat_SysChannel Send;
	Send.PlayerDBID = PlayerDBID;
	Send.Type = Type;
	Send.ChannelID = ChannelID;
	Send.Name = Name;
	Send.Content = Context;
	Send.ContentSize = Send.Content.Size()+1;
	Send.MsgType = MsgType;
	Send.SenderInfo = SenderInfo;
	SendToChat( sizeof(Send)  , &Send );
}

void NetSrv_Talk::SD_OfflineMessageRequest( int PlayerDBID )
{
	PG_Talk_LtoD_OfflineMessageRequest Send;
	Send.PlayerDBID = PlayerDBID;	
	Global::SendToDBCenter( sizeof(Send)  , &Send );
}