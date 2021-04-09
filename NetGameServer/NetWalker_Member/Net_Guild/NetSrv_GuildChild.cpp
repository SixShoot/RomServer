#include "../NetWakerGSrvInc.h"
#include "NetSrv_GuildChild.h"
#include "../Net_GuildHouses/NetSrv_GuildHousesChild.h"
#include "../../mainproc/GuildHouseManage/GuildHousesClass.h"
#include "AllOnlinePlayerInfo/AllOnlinePlayerInfo.h"
//-----------------------------------------------------------------
map< int, StructGuildFightRoom >			NetSrv_GuildChild::m_mapGuildFightRoom;
//-----------------------------------------------------------------
bool    NetSrv_GuildChild::Init()
{
    NetSrv_Guild::_Init();

    if( This != NULL)
        return false;

    This = NEW( NetSrv_GuildChild );

    return true;
}
//-----------------------------------------------------------------
bool    NetSrv_GuildChild::Release()
{
    if( This == NULL )
        return false;

    NetSrv_Guild::_Release();

    delete This;
    This = NULL;

    return true;
    
}
//-----------------------------------------------------------------
void  NetSrv_GuildChild::RC_CreateGuild				( BaseItemObject* Sender , char* GuildName )
{
	RoleDataEx* Owner = Sender->Role();
	
	if( Owner->CheckOpenMenu( EM_RoleOpenMenuType_CreateNewGuild) == false )
	{
		SC_CloseCreateGuild( Owner->GUID() );
		return;
	}

	//察看此人是否已有公會
	GuildMemberStruct* Member = GuildManageClass::This()->GetMember( Owner->DBID() );
	if( Member != NULL )
	{
		Owner->BaseData.GuildID = Member->GuildID;
		Print( LV3 , "RC_CreateGuild" , "Member != NULL  Owner = %s Guild=%d" , Global::GetRoleName( Owner ) , Member->GuildID );
		//已有公會
		SC_CreateGuildResult( Owner->DBID() , 0 , GuildName , EM_CreateGuildResult_Failed_Exist );
		return; 
	}
	if( strlen( GuildName ) == 0 )
	{	//沒設定名稱
		SC_CreateGuildResult( Owner->DBID() , 0 , GuildName , EM_CreateGuildResult_Failed_Name );
		return; 
	}

	GuildStruct* GuildInfo = GuildManageClass::This()->GetGuildInfo( GuildName );
	if( GuildInfo != NULL )
	{	//公會名稱重復
		SC_CreateGuildResult( Owner->DBID() , 0 , GuildName , EM_CreateGuildResult_Failed_Name );
		return; 
	}

	std::wstring outStrName;
	CharacterNameRulesENUM  Ret = g_ObjectData->CheckGuildNameRules( GuildName , (CountryTypeENUM)Global::Ini()->CountryType , outStrName );
	if( Ret != EM_CharacterNameRules_Rightful )
	{
		SC_CreateGuildResult( Owner->DBID() , 0 , GuildName , EM_CreateGuildResult_Failed_Name );
		return;
	}


	int	Guild_CreateCost = g_ObjectData->GetSysKeyValue( "Guild_CreateCost" );

	if( Owner->PlayerBaseData->Body.Money < Guild_CreateCost )
	{
		SC_CreateGuildResult( Owner->DBID() , 0 , GuildName , EM_CreateGuildResult_Failed_Money );
		return;
	}

	Owner->AddBodyMoney( Guild_CreateCost*-1 , NULL , EM_ActionType_CreateGuild , true );
	SD_CreateGuild( Owner->DBID() , GuildName );

}
void  NetSrv_GuildChild::RC_NewGuildInvite			( BaseItemObject* Sender , char* PlayerName )
{
	RoleDataEx* Owner = Sender->Role();

	//察看此人是否已有公會
	GuildMemberStruct* LeaderMemberInfo = GuildManageClass::This()->GetMember( Owner->DBID() );
	if( LeaderMemberInfo == NULL )
	{
		//Owner->Msg( "沒有公會,找不到成員資料" );
		Owner->Net_GameMsgEvent( EM_GameMessageEvent_Guild_GuildNoFound );
		return; 		
	}

	GuildStruct* GuildInfo;
	GuildInfo = GuildManageClass::This()->GetGuildInfo( LeaderMemberInfo->GuildID );
	if( GuildInfo == NULL )
	{	//已有公會
		//Owner->Msg( "沒有公會" );
		Owner->Net_GameMsgEvent( EM_GameMessageEvent_Guild_NoGuild );
		return; 
	}

	if( GuildInfo->Base.IsReady != false )
	{
		//Owner->Msg( "公會已成立" );
		Owner->Net_GameMsgEvent( EM_GameMessageEvent_Guild_GuildIsReady );
		return;
	}
	
	//檢查是否為會長
	if( GuildInfo->Base.LeaderDBID != Owner->DBID()  )
	{
		//Owner->Msg( "非公會會長" );
		Owner->Net_GameMsgEvent( EM_GameMessageEvent_Guild_NotLeader );
		return;
	}

	OnlinePlayerInfoStruct* PlayerInfo = AllOnlinePlayerInfoClass::This()->GetInfo_ByName( PlayerName );
	if( PlayerInfo == NULL )
	{
		//Owner->Msg( "此人不在線上" );
		Owner->Net_GameMsgEvent( EM_GameMessageEvent_Guild_TargetNotOnline );
		return;
	}

	GuildMemberStruct* PlayerMemberInfo = GuildManageClass::This()->GetMember( PlayerInfo->DBID );
	if( PlayerMemberInfo != NULL )
	{
		char Buf[256];
		if( LeaderMemberInfo->GuildID == PlayerMemberInfo->GuildID )
		{
			sprintf( Buf , "%s 已在連署名單內" , PlayerName );
		}
		else
		{
			sprintf( Buf , "%s 已加入公會" , PlayerName );
		}

		Owner->Msg( Buf );
		SC_NewGuildInviteResult( Owner->TempData.Sys.SockID , Owner->TempData.Sys.ProxyID , PlayerName , EM_NewGuildInviteResult_HasGuild );
		return;
	}
	

	SC_NewGuildInvite( PlayerInfo->SockID , PlayerInfo->ProxyID , LeaderMemberInfo->GuildID , (char*)Owner->RoleName() , Owner->DBID() );

}
void  NetSrv_GuildChild::RC_NewGuildInviteResult	( BaseItemObject* Sender , int GuildID , int LeaderDBID , bool Result )
{
	RoleDataEx* Owner = Sender->Role();
	GuildStruct* GuildInfo;

	GuildInfo = GuildManageClass::This()->GetGuildInfo( GuildID );
	if( GuildInfo == NULL )
	{	//已有公會
		//Owner->Msg( "沒有連署公會" );
		Owner->Net_GameMsgEvent( EM_GameMessageEvent_Guild_GuildNoFound );
		return; 
	}

	if( GuildInfo->Base.IsReady != false )
	{
		//Owner->Msg( "連署公會已成立" );
		Owner->Net_GameMsgEvent( EM_GameMessageEvent_Guild_GuildIsReady );
		return;
	}
	//檢查是否為會長
	if( GuildInfo->Base.LeaderDBID != LeaderDBID  )
	{
		//Owner->Msg( "非公會會長邀請" );
		Owner->Net_GameMsgEvent( EM_GameMessageEvent_Guild_NotLeader );
		return;
	}

	OnlinePlayerInfoStruct* LeaderInfo = AllOnlinePlayerInfoClass::This()->GetInfo_ByDBID( LeaderDBID );
	if( LeaderInfo == NULL )
	{
		//Owner->Msg( "公會會長不在線上" );
		Owner->Net_GameMsgEvent( EM_GameMessageEvent_Guild_LeaderNotOnline );
		return;
	}

	if( Result == false )
	{	//拒絕加入
		SC_NewGuildInviteResult( LeaderInfo->SockID , LeaderInfo->ProxyID , (char*)Owner->RoleName() , EM_NewGuildInviteResult_Disagree );
	}
	else
	{	//同意加入
		SD_AddNewGuildMember( GuildID , (char*)LeaderInfo->Name.Begin() , LeaderInfo->DBID , (char*)Owner->RoleName() , Owner->DBID() );
	}
}

void  NetSrv_GuildChild::RD_CreateGuildResult		( int PlayerDBID , char* GuildName , int GuildID , CreateGuildResultENUM Result )
{
	BaseItemObject* OwnerObj = Global::GetPlayerObj_ByDBID( PlayerDBID );
	if( OwnerObj == NULL )
		return;

	RoleDataEx* Owner = OwnerObj->Role();
	if( Result != EM_CreateGuildResult_OK )
	{
		int	Guild_CreateCost = g_ObjectData->GetSysKeyValue( "Guild_CreateCost" );
		//失敗則退錢
		Owner->AddBodyMoney( Guild_CreateCost , NULL , EM_ActionType_CreateGuild , true );
	}
	//通知公會會長
	SC_CreateGuildResult		( PlayerDBID , GuildID , GuildName , Result );	

}
void  NetSrv_GuildChild::RD_AddNewGuildMemberResult	( int LeaderDBID , char* LeaderName , int GuildID , char* NewMemberName , bool Result )
{
	OnlinePlayerInfoStruct* LeaderInfo = AllOnlinePlayerInfoClass::This()->GetInfo_ByDBID( LeaderDBID );
	if( LeaderInfo != NULL )
	{
		if( Result != false )
		{
			SC_NewGuildInviteResult		( LeaderInfo->SockID , LeaderInfo->ProxyID 
										, NewMemberName , EM_NewGuildInviteResult_OK );
		}
		else
		{
			SC_NewGuildInviteResult		( LeaderInfo->SockID , LeaderInfo->ProxyID 
										, NewMemberName , EM_NewGuildInviteResult_Failed );
		}
	}


	OnlinePlayerInfoStruct* MemberInfo = AllOnlinePlayerInfoClass::This()->GetInfo_ByName( NewMemberName );
	if( MemberInfo != NULL && Result != false )
	{
		SC_JoindNewGuild			( MemberInfo->SockID , MemberInfo->ProxyID , GuildID , LeaderName );
	}


};

void NetSrv_GuildChild::RD_AddGuild( GuildBaseStruct& Guild )
{
	Print( LV1 , "RD_AddGuild" , "GuildID=%d GuildName=%s" , Guild.GuildID  , Guild.GuildName.Begin() );
	GuildManageClass::This()->CreateGuild( Guild );
/*
	if( Global::Ini()->IsGuildHouseZone && Guild.IsOwnHouse != false )
	{
		static int BkRoomID = 1;
		GuildHousesManageClass* houseClass = GuildHousesManageClass::CreateHouse( Guild.GuildID , BkRoomID++ );

		if( houseClass != NULL )
			NetSrv_GuildHouses::SD_HouseInfoLoading( Guild.GuildID );
	}
*/
	SC_All_GuildBaseInfo( Guild.GuildName.Begin() , Guild.GuildID , Guild.GuildFlag , Guild.IsReady );
}
void NetSrv_GuildChild::RD_DelGuild( int GuildID )
{
	set<BaseItemObject* >&	PlayerObjSet = *(BaseItemObject::PlayerObjSet());
	set< BaseItemObject*>::iterator   PlayerObjIter;
	//計算每個房間的人數
	for( PlayerObjIter = PlayerObjSet.begin() ; PlayerObjIter != PlayerObjSet.end() ; PlayerObjIter++ )
	{
		BaseItemObject* Obj = *PlayerObjIter;
		if( Obj == NULL )
			continue;

		Print( LV2 , "RD_DelGuild" , "RoleName=%s Guild=%d " , Global::GetRoleName( Obj->Role() ) , GuildID );
		RoleDataEx* Role = Obj->Role();
		if( Role->BaseData.GuildID == GuildID )
			Role->BaseData.GuildID = 0;
	}

	SC_All_GuildDestroy ( GuildID );

	GuildManageClass::This()->DelGuild( GuildID );

}
void NetSrv_GuildChild::RD_AddGuildMember( GuildMemberStruct& Member )
{
	if( GuildManageClass::This()->AddMember( Member ) == false )
	{
		///assert( 0 );
		return;
	}

	BaseItemObject* MemberObj = BaseItemObject::GetObjByDBID( Member.MemberDBID );
	if( MemberObj != NULL )
	{
		MemberObj->Role()->BaseData.GuildID = Member.GuildID;
		SendRangeGuildIDChange( MemberObj->Role() );
	}

	OnlinePlayerInfoStruct* PlayerInfo = AllOnlinePlayerInfoClass::This()->GetInfo_ByDBID( Member.MemberDBID );

	if( PlayerInfo != NULL )
	{
		GuildManageClass::This()->SetMemberOnline( Member.MemberDBID , PlayerInfo );
	}
}
void NetSrv_GuildChild::RD_DelGuildMember( int GuildID , char* MemberName )
{

	BaseItemObject* MemberObj = BaseItemObject::GetObjByName( MemberName );
	if( MemberObj != NULL )
	{
		RoleDataEx* Role = MemberObj->Role();
		Role->BaseData.GuildLeaveTime = RoleDataEx::G_Now_Float;
		Role->BaseData.GuildID = 0;
		Print( LV2 , "RD_DelGuildMember" , "RoleName=%s Guild=%d " , Global::GetRoleName( MemberObj->Role() ) , GuildID );
		SendRangeGuildIDChange( MemberObj->Role() );

		//如果角色在戰場世界的公會戰 通知此角色退出
		if( Role->TempData.Sys.OnChangeZone != false )
		{
			NetSrv_CliConnect::SC_WorldReturnNotify( Role->GUID() , 402 );
		}
	}	

	GuildManageClass::This()->DelMember( GuildID , MemberName );

}

void NetSrv_GuildChild::OnDataCenterDisconnect( )
{
	GuildManageClass::This()->Clear();
}
void NetSrv_GuildChild::RC_LeaveNewGuild		( BaseItemObject* Sender , char* KickPlayerName )
{
	SD_LeaveNewGuild( Sender->Role()->DBID() , KickPlayerName );
}
void NetSrv_GuildChild::RC_DelNewGuild			( BaseItemObject* Sender )
{
	SD_DelNewGuild( Sender->Role()->DBID()  );
}
void NetSrv_GuildChild::RD_LeaveNewGuildResult	( int LeaderDBID , char* PlayerName , bool Result )
{
	SC_LeaveNewGuildResult( LeaderDBID , PlayerName , Result );
}
void NetSrv_GuildChild::RD_DelNewGuildResult	( int LeaderDBID , bool Result)
{
	SC_DelNewGuildResult( LeaderDBID , Result );
}

void NetSrv_GuildChild::RC_GuildInvite			( BaseItemObject* Sender , char* PlayerName )
{
	RoleDataEx* Owner = Sender->Role();

	//察看此人是否已有公會
	GuildMemberStruct* LeaderMemberInfo = GuildManageClass::This()->GetMember( Owner->DBID() );
	if( LeaderMemberInfo == NULL )
	{
		//Owner->Msg( "沒有公會,找不到成員資料" );
		Owner->Net_GameMsgEvent( EM_GameMessageEvent_Guild_GuildNoFound );
		return; 		
	}

	GuildStruct* GuildInfo = LeaderMemberInfo->Guild;

	if( GuildInfo->Base.IsReady == false )
	{
		//Owner->Msg( "還在連署中" );
		Owner->Net_GameMsgEvent( EM_GameMessageEvent_Guild_GuildIsNotReady );
		return;
	}

	if( GuildInfo->Base.MaxMemberCount <= int( GuildInfo->Member.size() ) )
	{
		Owner->Net_GameMsgEvent( EM_GameMessageEvent_Guild_MaxMember );
		return;
	}

	//權限檢查
	if( GuildInfo->Base.Rank[ LeaderMemberInfo->Rank].Setting.Invite == false )
	{
		//Owner->Msg( "沒有邀請加入的權限" );
		Owner->Net_GameMsgEvent( EM_GameMessageEvent_Guild_NoInviteRight );
		return;
	}

	OnlinePlayerInfoStruct* PlayerInfo = AllOnlinePlayerInfoClass::This()->GetInfo_ByName( PlayerName );
	if( PlayerInfo == NULL )
	{
		//Owner->Msg( "此人不在線上" );
		Owner->Net_GameMsgEvent( EM_GameMessageEvent_Guild_TargetNotOnline );
		return;
	}
	
	//在退出公會某時間內不得加入公會
	//BaseItemObject* MemberObj = BaseItemObject::GetObjByName( PlayerName );
	//if( MemberObj != NULL )
	//{
	//	RoleDataEx* Role = MemberObj->Role();
	//	int nowtime = TimeExchangeFloatToInt(RoleDataEx::G_Now_Float);
	//	int leavetime = TimeExchangeFloatToInt(Role->BaseData.GuildLeaveTime);
	//	int difference;
	//	if(nowtime - leavetime < 60 * 60)
	//	{
	//		char Buf[256];
	//		//計算時間
	//		__time32_t NowTime = leavetime - RoleDataEx::G_TimeZone * 60 * 60;
	//		struct tm *gmt;
	//		gmt = _localtime32( &NowTime );
	//		if( gmt == NULL ) return;
	//		//sprintf( Buf , "欲邀請的玩家退出前公會時間 %d:%d 。1小時內不得加入公會！" ,gmt->tm_min,gmt->tm_sec);
	//		//Owner->Msg( Buf );
	//		NetSrv_Talk::GameMsgEventData( Sender->Role()->GUID(), 2, EM_GameMessageEventData_Guild_TargetCanNotInvite, EM_GameMessageDataType_Int,gmt->tm_hour, EM_GameMessageDataType_Int,gmt->tm_min);
	//		//return;
	//	}
	//}
	
	//改由clinet檔
	//struct tm *gmt;
	//__time32_t NowTime = RoleDataEx::G_Now;
	//gmt = _localtime32( &NowTime );
	//if(gmt->tm_hour >= 18 && gmt->tm_hour < 21)
	//{//現在正在進行公會城戰，不能新增成員！
	//	Owner->Net_GameMsgEvent( EM_GameMessageEvent_GuildWar_CantInvite );
	//	return;
	//}

	GuildMemberStruct* PlayerMemberInfo = GuildManageClass::This()->GetMember( PlayerInfo->DBID );
	if( PlayerMemberInfo != NULL )
	{
		char Buf[256];
		if( LeaderMemberInfo->GuildID == PlayerMemberInfo->GuildID )
		{
			sprintf( Buf , "%s 為同公會的玩家" , PlayerName );
		}
		else
		{
			sprintf( Buf , "%s 已加入公會" , PlayerName );
		}

		Owner->Msg( Buf );
		SC_GuildInviteResult( Owner->TempData.Sys.SockID , Owner->TempData.Sys.ProxyID , PlayerName , EM_GuildInviteResult_HasGuild );
		return;
	}


	SC_GuildInvite( PlayerInfo->SockID , PlayerInfo->ProxyID , LeaderMemberInfo->GuildID , (char*)Owner->RoleName() , Owner->DBID() );

}
void NetSrv_GuildChild::RC_GuildInviteResult	( BaseItemObject* Sender , int GuildID , int LeaderDBID , bool Result )
{
	RoleDataEx* Owner = Sender->Role();
	/*
	GuildStruct* GuildInfo;
	GuildInfo = GuildManageClass::This()->GetGuildInfo( GuildID );
	if( GuildInfo == NULL )
	{	//已有公會
		Owner->Msg( "沒有公會" );
		return; 
	}

	if( GuildInfo->Base.IsReady == false )
	{
		Owner->Msg( "公會還在連署中" );
		return;
	}
*/

	OnlinePlayerInfoStruct* LeaderInfo = AllOnlinePlayerInfoClass::This()->GetInfo_ByDBID( LeaderDBID );
	if( LeaderInfo == NULL )
	{
		//Owner->Msg( "邀請者不在線上" );
		Owner->Net_GameMsgEvent( EM_GameMessageEvent_Guild_InviterNotOnline );
		return;
	}

	if( LeaderInfo->DBID == Owner->DBID() )
		return;

	//在退出公會某時間內不得加入公會
	int nowtime = TimeExchangeFloatToInt(RoleDataEx::G_Now_Float);
	int leavetime = TimeExchangeFloatToInt(Owner->BaseData.GuildLeaveTime);
	int	notInviteTime = 24;

	if(nowtime - leavetime < 60 * 60 * notInviteTime)
	{
		//計算時間
		__time32_t NowTime = leavetime - RoleDataEx::G_TimeZone * 60 * 60;
		struct tm *gmt;
		gmt = _localtime32( &NowTime );
		if( gmt != NULL )
		{
			//sprintf( Buf , "欲邀請的玩家退出前公會時間 %02d:%02d 。24小時內不得加入公會！" ,gmt->tm_min,gmt->tm_sec);
			NetSrv_Talk::GameMsgEventData( Owner->GUID(), 3, EM_GameMessageEventData_Guild_TargetCanNotInvite, EM_GameMessageDataType_Int,gmt->tm_hour, EM_GameMessageDataType_Int,gmt->tm_min, EM_GameMessageDataType_Int, notInviteTime);
			Result = false;
		}
	}



	if( Result == false )
	{	//拒絕加入
		SC_GuildInviteResult( LeaderInfo->SockID , LeaderInfo->ProxyID , (char*)Owner->RoleName() , EM_GuildInviteResult_Disagree );
	}
	else
	{	//同意加入
		SD_AddGuildMember( GuildID , (char*)LeaderInfo->Name.Begin() , LeaderInfo->DBID , (char*)Owner->RoleName() , Owner->DBID() );
	}
}
void NetSrv_GuildChild::RC_LeaveGuild			( BaseItemObject* Sender , char* KickPlayerName )
{
	SD_LeaveGuild( Sender->Role()->DBID() , KickPlayerName );
}
void NetSrv_GuildChild::RC_DelGuild				( BaseItemObject* Sender )
{
	SD_DelGuild( Sender->Role()->DBID()  );
}
void NetSrv_GuildChild::RD_AddGuildMemberResult	( int LeaderDBID , char* LeaderName , int GuildID , char* NewMemberName , bool Result )
{
	OnlinePlayerInfoStruct* LeaderInfo = AllOnlinePlayerInfoClass::This()->GetInfo_ByDBID( LeaderDBID );
	if( LeaderInfo != NULL )
	{
		if( Result != false )
		{
			SC_GuildInviteResult		( LeaderInfo->SockID , LeaderInfo->ProxyID 
				, NewMemberName , EM_GuildInviteResult_OK );
		}
		else
		{
			SC_GuildInviteResult		( LeaderInfo->SockID , LeaderInfo->ProxyID 
				, NewMemberName , EM_GuildInviteResult_Failed );
		}
	}

	OnlinePlayerInfoStruct* MemberInfo = AllOnlinePlayerInfoClass::This()->GetInfo_ByName( NewMemberName );
	if( MemberInfo != NULL && Result != false )
	{
		SC_JoindGuild			( MemberInfo->SockID , MemberInfo->ProxyID , GuildID , LeaderName );
	}


}
void NetSrv_GuildChild::RD_LeaveGuildResult		( int LeaderDBID , char* PlayerName , bool Result )
{
	SC_LeaveGuildResult( LeaderDBID , PlayerName , Result );
}
void NetSrv_GuildChild::RD_DelGuildResult		( int LeaderDBID , bool Result )
{
	SC_DelGuildResult( LeaderDBID , Result );
}

void NetSrv_GuildChild::RD_ModifyGuild			( GuildBaseStruct&	Guild )
{
	GuildManageClass::This()->ModifyGuild( Guild );

}
void NetSrv_GuildChild::RD_ModifySimpleGuild	( SimpleGuildInfoStruct& SimpleGuildInfo )
{
	GuildManageClass::This()->ModifySimpleGuild( SimpleGuildInfo );
}


void NetSrv_GuildChild::RD_ModifyGuildMember	( GuildMemberStruct&	Member )	
{
	GuildManageClass::This()->ModifyMember( Member );
}

void NetSrv_GuildChild::RC_GuildInfoRequet		( BaseItemObject* Sender , int GuildID )
{
	GuildStruct* Guild = GuildManageClass::This()->GetGuildInfo( GuildID );	
	RoleDataEx* Owner = Sender->Role();
	if( Guild == NULL )
	{
		//Owner->Msg( "找不到公會" );
		Owner->Net_GameMsgEvent( EM_GameMessageEvent_Guild_GuildNoFound );
		return;
	}

	SC_GuildInfo( Owner->GUID() , Guild->Base , (int)Guild->Member.size() );
}

void NetSrv_GuildChild::RC_GuildMemberListRequet( BaseItemObject* Sender , int GuildID )	
{
	GuildStruct* Guild = GuildManageClass::This()->GetGuildInfo( GuildID );	
	RoleDataEx* Owner = Sender->Role();
	if( Guild == NULL )
	{
		//Owner->Msg( "找不到公會" );
		Owner->Net_GameMsgEvent( EM_GameMessageEvent_Guild_GuildNoFound );
		SC_GuildMemberListCount( Owner->GUID() , GuildID , 0 );
		return;
	}

	
	SC_GuildMemberListCount( Owner->GUID() , GuildID , (int)Guild->Member.size() );

	for( int i = 0 ; i < (int)Guild->Member.size() ; i++ )
	{
		GuildMemberStruct* Member = Guild->Member[i];

		GuildMemberBaseInfoStruct Temp;
		
		Temp.MemberName = Member->MemberName;
		Temp.MemberDBID = Member->MemberDBID;
		Temp.Rank		= Member->Rank;				//角色公會等級
		Temp.Voc		= Member->Voc;
		Temp.Voc_Sub	= Member->Voc_Sub;
		Temp.Sex		= Member->Sex;
		Temp.LV			= Member->LV;
		Temp.LV_Sub		= Member->LV_Sub;
		Temp.GroupID	= Member->GroupID;
		Temp.IsOnline	= Member->IsOnline;

		OnlinePlayerInfoStruct* PlayerInfo = AllOnlinePlayerInfoClass::This()->GetInfo_ByDBID( Member->MemberDBID );
		if( PlayerInfo == NULL )
			Temp.GUID = -1;
		else
			Temp.GUID = PlayerInfo->GUID;


		SC_GuildMemberBaseInfo( Owner->GUID() , Temp );
	}
}

void NetSrv_GuildChild::RC_GuildMemberInfoRequet( BaseItemObject* Sender , int MemberDBID )	
{
	GuildMemberStruct* GuildMember = GuildManageClass::This()->GetMember( MemberDBID );
	RoleDataEx* Owner = Sender->Role();

	if( GuildMember == NULL )
	{
		//Owner->Msg( "找不到此人的公會資料" );
		Owner->Net_GameMsgEvent( EM_GameMessageEvent_Guild_NoGuild );
		return;
	}
	SC_GuildMemberInfo( Owner->GUID() , *GuildMember );
}

void NetSrv_GuildChild::RC_ModifyGuildInfoRequest( BaseItemObject* Sender , GuildBaseStruct&	Guild )	
{
	RoleDataEx* Owner = Sender->Role();

	if( Owner->BaseData.GuildID != Guild.GuildID )
	{
		Print( LV2 , "RC_ModifyGuildInfoRequest" , "Owner->BaseData.GuildID != Guild.GuildID  GuildID = %d" , Guild.GuildID );
		return;
	}
	//取得此公會
	GuildStruct* SrvGuild = GuildManageClass::This()->GetGuildInfo( Guild.GuildID );	
	if( SrvGuild == NULL )
	{
		Print( LV2 , "RC_ModifyGuildInfoRequest" , "GuildID Not Find  GuildID = %d [%s]" , Guild.GuildID , Guild.GuildName.Begin() );
		return;
	}

	if( stricmp( Guild.GuildName.Begin() , SrvGuild->Base.GuildName.Begin() ) != 0 )
	{
		Print( LV2 , "RC_ModifyGuildInfoRequest" , "GuildName Error GuildID = %d [%s] => [%s]" , Guild.GuildID , Guild.GuildName.Begin()  , SrvGuild->Base.GuildName.Begin() );
		return;
	}

	if( SrvGuild->Base.IsReady == false )
	{
		Print( LV2 , "RC_ModifyGuildInfoRequest" , "Guild Not Ready(IsReady == false), GuildID = %d [%s]" , Guild.GuildID , Guild.GuildName.Begin() );
		return;
	}

	Guild.MaxMemberCount = 0;
	//Guild.Score = 0;
	Guild.Resource.Init();
	Guild.RankCount = 0;

	SD_ModifyGuildInfoRequest( Sender->Role()->DBID() , Guild );
}

void NetSrv_GuildChild::RC_ModifyGuildMemberInfoRequest	( BaseItemObject* Sender , GuildMemberStruct&	Member )
{
	SD_ModifyGuildMemberInfoRequest( Sender->Role()->DBID() , Member );
}

//----------------------------------------------------------------------------------------------
//留言板
//----------------------------------------------------------------------------------------------

bool NetSrv_GuildChild::CheckIsEnabledGuildBoard	( RoleDataEx* Role )
{
	GuildStruct* SrvGuild = GuildManageClass::This()->GetGuildInfo( Role->BaseData.GuildID );	
	if( SrvGuild == NULL )
	{
		Role->Msg( "公會不存在" );
		return false;
	}
	if( SrvGuild->Base.IsEnabledGuildBoard == false )
	{
		Role->Msg( "IsEnabledGuildBoard" );
		return false;
	}
	return true;
}

void  NetSrv_GuildChild::RC_BoardPostRequest		( BaseItemObject* SenderObj , GuildBoardStruct& Message )
{
	if( CheckIsEnabledGuildBoard( SenderObj->Role() ) == false )
		return;

	SD_BoardPostRequest( SenderObj->Role()->DBID() , Message );
}
void  NetSrv_GuildChild::RC_BoardListRequest		( BaseItemObject* SenderObj , int PageID )
{
	if( CheckIsEnabledGuildBoard( SenderObj->Role() ) == false )
		return;

	SD_BoardListRequest( SenderObj->Role()->DBID() , PageID );
}
void  NetSrv_GuildChild::RC_BoardMessageRequest		( BaseItemObject* SenderObj , int MessageGUID )
{
	if( CheckIsEnabledGuildBoard( SenderObj->Role() ) == false )
		return;

	SD_BoardMessageRequest( SenderObj->Role()->DBID() , MessageGUID );
}
void  NetSrv_GuildChild::RC_BoardModifyMessageRequest( BaseItemObject* SenderObj , int MessageGUID , char* Message )
{
	if( CheckIsEnabledGuildBoard( SenderObj->Role() ) == false )
		return;

	SD_BoardModifyMessageRequest( SenderObj->Role()->DBID() , MessageGUID , Message );
}
void  NetSrv_GuildChild::RC_BoardModifyModeRequest	( BaseItemObject* SenderObj , int MessageGUID , GuildBoardModeStruct Mode )
{
	if( CheckIsEnabledGuildBoard( SenderObj->Role() ) == false )
		return;

	SD_BoardModifyModeRequest( SenderObj->Role()->DBID() , MessageGUID , Mode );
}
//----------------------------------------------------------------------------------------------
//送所有的公會基本資料給Client
void NetSrv_GuildChild::SendAllGuildBaseInfo( int SendID )
{
	for( int i = 0 ; i < GuildManageClass::This()->MaxGuildID() ; i++ )
	{
		GuildStruct* GuildInfo = GuildManageClass::This()->GetGuildInfo( i );

		if(GuildInfo == NULL || GuildInfo->Member.size() == 0)
		{
			Print( LV1 , "SendAllGuildBaseInfo" , "GuildInfo == NULL || GuildInfo->Member.size() == 0" );
			continue;
		}
		
		Print( LV1 , "SendAllGuildBaseInfo" , "GuildID=%d GuildName=%s" , GuildInfo->Base.GuildID , GuildInfo->Base.GuildName.Begin() );
		SC_GuildBaseInfo( SendID , GuildInfo->Base.GuildName.Begin() , GuildInfo->Base.GuildID , GuildInfo->Base.GuildFlag , GuildInfo->Base.IsReady );
	}
	
}
//----------------------------------------------------------------------------------------------
void  NetSrv_GuildChild::RC_CloseCreateGuild		( BaseItemObject* SenderObj )
{
	RoleDataEx* Role = SenderObj->Role();
	if( Role->CheckOpenMenu( EM_RoleOpenMenuType_CreateNewGuild) == false )
		return;

	Role->ClsOpenMenu();	
}
void  NetSrv_GuildChild::RC_GuildContributionItem	( BaseItemObject* SenderObj , int Pos , ItemFieldStruct& Item , bool IsPut )
{
	RoleDataEx* Role = SenderObj->Role();
	/*
	if( Role->CheckOpenMenu( EM_RoleOpenMenuType_GuildContribution) == false )
	{
		SC_GuildContributionClose( Role->GUID() );
		return;
	}
*/

	ItemFieldStruct* BodyItem =  Role->GetBodyItem( Pos );
	if( *BodyItem != Item || 
		(	BodyItem->Pos != EM_USE_GUILDCONTRIBUTION 
			&&BodyItem->Pos != EM_ItemState_NONE  ) )
	{
		Role->Net_FixItemInfo_Body( Pos );
		SC_GuildContributionItemResult( Role->GUID() , false );
		return;
	}

	GameObjDbStructEx* ItemDB = Global::GetObjDB( BodyItem->OrgObjID );
	if( ItemDB->IsItem_Pure() == false || ItemDB->Item.GuildResourceType == EM_GuildResourceType_None )
	{
		Role->Net_FixItemInfo_Body( Pos );
		SC_GuildContributionItemResult( Role->GUID() , false );
		return;
	}

	if( IsPut )
		BodyItem->Pos = EM_USE_GUILDCONTRIBUTION; 
	else
		BodyItem->Pos = EM_ItemState_NONE; 

	Role->Net_FixItemInfo_Body( Pos );
	SC_GuildContributionItemResult( Role->GUID() , true );
}

void  NetSrv_GuildChild::RC_GuildContributionOK		( BaseItemObject* SenderObj , int Money , int Bonus_Money )
{
	RoleDataEx* Role = SenderObj->Role();
	if( Role->TempData.Sys.OnChangeZone != false )
	{
		return;
	}


	bool	Result;
	if( Money < 0 || Money > Role->PlayerBaseData->Body.Money )
		return;

	if( Bonus_Money < 0 || Bonus_Money > Role->PlayerBaseData->Money_Bonus )
		return;


	GuildResourceStruct	Resource;
	Resource.Init();

	if( Money > 0 )
	{
		Resource.Gold = Money;
		Result = Role->AddBodyMoney( Money * -1 , Role , EM_ActionType_GuildContribution , true );
		if( Result == false )
			return;
	}

	if( Bonus_Money > 0 )
	{		
		Resource.BonusGold = Bonus_Money;
		Result = Role->AddMoney_Bonus( Bonus_Money*-1 , Role , EM_ActionType_GuildContribution );
		if( Result == false )
			return;
	}


	//清除所有的貢獻物品資料
	for( int i = 0 ; i < Role->PlayerBaseData->Body.Count ; i++ )
	{
		if( Role->PlayerBaseData->Body.Item[i].Pos == EM_USE_GUILDCONTRIBUTION )
		{
			GameObjDbStructEx* OrgDB = Global::GetObjDB( Role->PlayerBaseData->Body.Item[i].OrgObjID );

			if( OrgDB == NULL )
			{
				Role->PlayerBaseData->Body.Item[i].Pos = EM_ItemState_NONE;
				Role->Net_FixItemInfo_Body( i );
				continue;
			}

			Resource._Value[ OrgDB->Item.GuildResourceType ] +=  OrgDB->Item.GuildResourceValue * Role->PlayerBaseData->Body.Item[i].Count; 
			Global::Log_ItemDestroy( Role , EM_ActionType_GuildContribution , Role->PlayerBaseData->Body.Item[i] , Role->PlayerBaseData->Body.Item[i].Count , -1  , "" );
			Role->PlayerBaseData->Body.Item[i].Init();
			Role->Net_FixItemInfo_Body( i );
		}
	}

	SD_AddGuildResource( Role->BaseData.GuildID , Role->DBID() , Resource , "" , EM_AddGuildResourceType_Contribution , 0 );

//	Role->ClsOpenMenu();		
}

void  NetSrv_GuildChild::RC_GuildContributionClose	( BaseItemObject* SenderObj )
{
	RoleDataEx* Role = SenderObj->Role();
	/*
	if( Role->CheckOpenMenu( EM_RoleOpenMenuType_GuildContribution) == false )
	{
		return;
	}
*/
	//清除所有的貢獻物品資料
	for( int i = 0 ; i < Role->PlayerBaseData->Body.Count ; i++ )
	{
		if( Role->PlayerBaseData->Body.Item[i].Pos == EM_USE_GUILDCONTRIBUTION )
		{
			Role->PlayerBaseData->Body.Item[i].Pos = EM_ItemState_NONE;
			Role->Net_FixItemInfo_Body( i );
		}
	}
//	Role->ClsOpenMenu();		
}
//----------------------------------------------------------------------------------------------------------
//公會商店
//----------------------------------------------------------------------------------------------------------
/*
void  NetSrv_GuildChild::RC_GuildShopBuy				( BaseItemObject* SenderObj , int FlagID )
{
	RoleDataEx* Role = SenderObj->Role();
	if( Role->CheckOpenMenu( EM_RoleOpenMenuType_GuildShop) == false )
	{
		return;
	}

	GuildStruct* GuildInfo = GuildManageClass::This()->GetGuildInfo( Role->BaseData.GuildID );
	if( GuildInfo == NULL )
	{
		return;
	}
	SD_GuildShopBuy( Role->Base.DBID , FlagID );
}
void  NetSrv_GuildChild::RC_GuildShopClose				( BaseItemObject* SenderObj )
{
	RoleDataEx* Role = SenderObj->Role();
	if( Role->CheckOpenMenu( EM_RoleOpenMenuType_GuildShop) == false )
	{
		return;
	}

	Role->ClsOpenMenu();
}
*/
//----------------------------------------------------------------------------------------------------------
void  NetSrv_GuildChild::RC_AllGuildMemberCountRequest		( BaseItemObject* Sender )
{
	vector< GuildStruct* >&	GuildList = GuildManageClass::This()->GuildList();			//公會列表

	SC_AllGuildMemberCountResult( Sender->GUID() , GuildList );
}
//----------------------------------------------------------------------------------------------------------
//公會競標
//----------------------------------------------------------------------------------------------------------
void  NetSrv_GuildChild::RC_GuildFightAuctionClose		( BaseItemObject* SenderObj )
{

}
void  NetSrv_GuildChild::RC_GuildFightAuctionBid		( BaseItemObject* SenderObj , int iCrystalID, int iBidScore )
{
	/*
	BaseItemObject*		pObj	= BaseItemObject::GetObj( iNPCID );
	if( pObj == NULL )
		return;

	RoleDataEx*			pNPC	= pObj->Role();

	int iCrystalID = pNPC->TempData.CrystalID;
	*/

	if( iCrystalID != 0 )
	{
		SD_GuildFightAuctionBid( SenderObj->Role()->DBID(), iCrystalID, iBidScore );
	}
	
}
void  NetSrv_GuildChild::RD_GuildFightAuctionInfo		( int CrystalID , int OwnerGuildID , int BitGuildID )
{

}
//----------------------------------------------------------------------------------------------------------
void  NetSrv_GuildChild::RD_CreateGuildFightRoom ( int CrystalID , int iDefenderGuildID , int iAttackerGuildID )
{
	int						iRoomID		= 1;
	StructGuildFightRoom	fightRoom;
	GuildStruct*			pGuildInfo	= NULL;

	fightRoom.CrystalID			= CrystalID;
	fightRoom.iDefenderGuildID	= iDefenderGuildID;
	fightRoom.iAttackerGuildID	= iAttackerGuildID;
	fightRoom.emType			= EM_GuildFightRoomType_GuildFight;

	// 找出一個空的房間
		map< int, StructGuildFightRoom >::iterator it;

		while(1)
		{
			it = m_mapGuildFightRoom.find( iRoomID );
			if( it == m_mapGuildFightRoom.end() )
				break;
			else
				iRoomID++;
		};

	// 建立資料
		m_mapGuildFightRoom[ iRoomID ] = fightRoom;

	// 建立房間
		Global::CopyRoomMonster( 0, iRoomID );

	// 通知 iDefenderGuildID / iAttackerGuildID 公會會長( 或者是具有權力的人 ), 房間已經建立好. 可以開始進入

		pGuildInfo = GuildManageClass::This()->GetGuildInfo( iDefenderGuildID );
		if( pGuildInfo == NULL )
		{			
			SC_GuildFightLeaderInvite( pGuildInfo->Base.LeaderDBID, RoleDataEx::G_ZoneID, iRoomID, CrystalID, iDefenderGuildID, iAttackerGuildID );
		}

		pGuildInfo = GuildManageClass::This()->GetGuildInfo( iAttackerGuildID );
		if( pGuildInfo == NULL )
		{			
			SC_GuildFightLeaderInvite( pGuildInfo->Base.LeaderDBID, RoleDataEx::G_ZoneID, iRoomID, CrystalID, iDefenderGuildID, iAttackerGuildID );
		}
}
//----------------------------------------------------------------------------------------------------------
void  NetSrv_GuildChild::RC_GuildFightRequestInvite	( PG_Guild_CtoL_GuildFightRequestInvite* pPacket )
{
	// 收到公會會長要求, 讓某公會會員進入公會戰
	// 檢查該角色是否為公會會員, 若是, 則檢查該角色是否已經在戰場內

	int						iRoomID		= pPacket->iRoomID;

	GuildStruct*			pGuildInfo	= NULL;
	GuildMemberStruct*		pMember		= NULL;
	StructGuildFightRoom	RoomInfo;

	map< int, StructGuildFightRoom >::iterator it = m_mapGuildFightRoom.find( iRoomID );
	if( it == m_mapGuildFightRoom.end() )
		return;

	if( pPacket->iGuildID != RoomInfo.iAttackerGuildID || pPacket->iGuildID != RoomInfo.iDefenderGuildID )
		return;

	RoomInfo = it->second;
	pGuildInfo = GuildManageClass::This()->GetGuildInfo( pPacket->iGuildID );
	
	if( pGuildInfo == NULL )
		return;

	for( vector<GuildMemberStruct*>::iterator itMember = pGuildInfo->Member.begin(); itMember != pGuildInfo->Member.end(); itMember++ )
	{
		if( (*itMember)->MemberDBID == pPacket->iMemberDBID )
		{
			pMember = (*itMember);
		}
	}

	if( pMember == NULL )
		return;

	// 檢查戰場內的人數, 要考慮斷線上來的人的問題

	// 發出請求加入的封包	
	SC_GuildFightInviteNotify( pMember->MemberDBID, RoomInfo.CrystalID, iRoomID );
}
//----------------------------------------------------------------------------------------------------------
void  NetSrv_GuildChild::RC_GuildFightInviteRespond	( PG_Guild_CtoL_GuildFightInviteRespond* pPacket )
{
	int						iRoomID		= pPacket->iRoomID;

	GuildStruct*			pGuildInfo	= NULL;
	GuildMemberStruct*		pMember		= NULL;
	StructGuildFightRoom	RoomInfo;

	map< int, StructGuildFightRoom >::iterator it = m_mapGuildFightRoom.find( iRoomID );
	if( it == m_mapGuildFightRoom.end() )
		return;

	if( pPacket->iGuildID != RoomInfo.iAttackerGuildID || pPacket->iGuildID != RoomInfo.iDefenderGuildID )
		return;

	RoomInfo = it->second;
	pGuildInfo = GuildManageClass::This()->GetGuildInfo( pPacket->iGuildID );

	if( pGuildInfo == NULL )
		return;

	for( vector<GuildMemberStruct*>::iterator itMember = pGuildInfo->Member.begin(); itMember != pGuildInfo->Member.end(); itMember++ )
	{
		if( (*itMember)->MemberDBID == pPacket->iMemberDBID )
		{
			pMember = (*itMember);
		}
	}

	if( pMember == NULL )
		return;


	// 收到戰場要求的答案
	if( pPacket->bJoin == false )
	{
		return;
	}

	// 呼叫  Server 模組中設定的 Lua 函式, 呼叫 LuaZone_InviteMember( iRoomID, DBID
	// 就將角色轉移過來
	// 看是發送封包給 Client 還是發送封包給 人物所在的 Srv 傳送人物
}

//送公會ID改變 給周圍的完家
void NetSrv_GuildChild::SendRangeGuildIDChange			( RoleDataEx* Owner )
{

	//_Def_View_Block_Range_
	PlayIDInfo**	Block = Global::PartSearch( Owner , _Def_View_Block_Range_ );

	int			i , j;
	PlayID*		TopID;

	for( i = 0 ; Block[i] != NULL ; i++ )
	{
		TopID = Block[i]->Begin;

		for( j = 0 ; TopID != NULL ; TopID = TopID->Next )
		{
			BaseItemObject* OtherObj = BaseItemObject::GetObj( TopID->ID );
			if( OtherObj == NULL )
				continue;
			
			SC_GuildIDChange( OtherObj->GUID() , Owner->GUID() , Owner->BaseData.GuildID );
		}
	}
}

//----------------------------------------------------------------------------------------------
//公會自由對戰
//----------------------------------------------------------------------------------------------
void  NetSrv_GuildChild::RC_DeclareWar				( BaseItemObject* Obj , int Type , int TargetGuildID )
{

	int		PKTime;
	int		PKMoney;

	RoleDataEx* Owner = Obj->Role();

	if( TargetGuildID == Owner->GuildID() )
	{
		SC_DeclareWarResult( Owner->GUID() , Type , TargetGuildID , EM_DeclareWarResult_DataErr );
		return;
	}

	g_ObjectData->GetGuildWarDeclareTypeInfo( Type , PKTime , PKMoney );

	if( PKTime == 0 )
	{
		SC_DeclareWarResult( Owner->GUID() , Type , TargetGuildID , EM_DeclareWarResult_ScoreErr );
		return;
	}

	//檢查公會
	GuildMemberStruct* LeaderMemberInfo = GuildManageClass::This()->GetMember( Owner->DBID() );
	if( LeaderMemberInfo == NULL )
	{
		SC_DeclareWarResult( Owner->GUID() , Type , TargetGuildID , EM_DeclareWarResult_DataErr );
		return; 		
	}

	GuildStruct* GuildInfo = LeaderMemberInfo->Guild;

	if( g_ObjectData->GuildLvTable().size() <= (unsigned)GuildInfo->Base.Level )
		return;		

	GuildLvInfoStruct& Info = g_ObjectData->GuildLvTable()[ GuildInfo->Base.Level ];
	if( Info.DeclareWar == 0 )
	{
		SC_DeclareWarResult( Owner->GUID() , Type , TargetGuildID , EM_DeclareWarResult_GuildLvErr );
		return;
	}
	//////////////////////////////////////////////////////////////////////////
	//查目標公會等級
	//////////////////////////////////////////////////////////////////////////
	{
		GuildStruct* TargetGuildInfo = GuildManageClass::This()->GetGuildInfo( TargetGuildID );
		if( TargetGuildInfo == NULL )
		{
			SC_DeclareWarResult( Owner->GUID() , Type , TargetGuildID , EM_DeclareWarResult_TargetGuildErr );
			return;
		}

		if( g_ObjectData->GuildLvTable().size() <= (unsigned)TargetGuildInfo->Base.Level )
			return;		

		GuildLvInfoStruct& Info = g_ObjectData->GuildLvTable()[ TargetGuildInfo->Base.Level ];
		if( Info.DeclareWar == 0 )
		{
			SC_DeclareWarResult( Owner->GUID() , Type , TargetGuildID , EM_DeclareWarResult_TargetGuildLvErr );
			return;
		}
		
	}
	//////////////////////////////////////////////////////////////////////////

	GuildMemberStruct* memberInfo =  GuildManageClass::This()->GetMember( Owner->DBID() );
	if( memberInfo == NULL )
	{
		Owner->Msg( "guild member not find" );
		return;
	}
	if( (unsigned)memberInfo->Rank >= EM_GuildRank_Count )
		return;

	GuildManageSettingStruct& rankSetting = GuildInfo->Base.Rank[ memberInfo->Rank ].Setting;

	//檢查是否為公會會長
	if(		GuildInfo->Base.LeaderDBID != Owner->DBID()
		&&	rankSetting.GuildWarRegister == false )
	{
		Owner->Msg( "不是公會會長" );
		SC_DeclareWarResult( Owner->GUID() , Type , TargetGuildID , EM_DeclareWarResult_LeaderErr );
		return;
	}

	if( GuildInfo->Base.IsWar != false || GuildInfo->Base.IsWarPrepare != false )
	{
		Owner->Msg( "已經與其他公會宣戰" );
		SC_DeclareWarResult( Owner->GUID() , Type , TargetGuildID , EM_DeclareWarResult_OnWarErr );
		return;
	}
	if( GuildManageClass::This()->IsWarDeclare( Owner->GuildID() , TargetGuildID ) != false )
	{
		Owner->Msg( "已經與此公會宣戰" );
		SC_DeclareWarResult( Owner->GUID() , Type , TargetGuildID , EM_DeclareWarResult_OnWarErr );
		return;
	}
	SD_DeclareWar( Type , Owner->DBID() , TargetGuildID );
//	Owner->AddBodyMoney( PKMoney * -1 , Owner , EM_ActionType_GuildWarDeclare  );
}
void  NetSrv_GuildChild::RD_DeclareWarResult		( int PlayerDBID , int GuildID  , int Type , int TargetGuildID , DeclareWarResultENUM Result )
{
	RoleDataEx* Owner = Global::GetRoleDataByDBID( PlayerDBID );
	if( Owner == NULL )
		return;

	SC_DeclareWarResult( Owner->GUID() , Type , TargetGuildID , Result );

}
void  NetSrv_GuildChild::RD_WarInfo					( bool IsDeclareWar , int GuildID[2] , int Time )
{
	GuildStruct* GuildInfo = GuildManageClass::This()->GetGuildInfo( GuildID[0] );	
	if( GuildInfo != NULL )
	{
		GuildInfo->Base.IsWar = IsDeclareWar;
	}

	if( IsDeclareWar )
	{
		GuildManageClass::This()->WarDeclareInsert( GuildID[0] , GuildID[1] );
	}
	else
	{
		GuildManageClass::This()->WarDeclareEarse( GuildID[0] , GuildID[1] );
	}
	if( GuildInfo != NULL )
		SC_All_WarInfo( IsDeclareWar , GuildID[0] , GuildID[1] , Time , GuildInfo->Base.WarDeclareTime );
}

//----------------------------------------------------------------------------------------------
//公會旗設定
//----------------------------------------------------------------------------------------------
void NetSrv_GuildChild::RC_SetGuildFlagInfoRequest		( BaseItemObject* Obj , GuildFlagStruct& Flag )
{

	RoleDataEx* Owner = Obj->Role();

	//檢查公會
	GuildMemberStruct* LeaderMemberInfo = GuildManageClass::This()->GetMember( Owner->DBID() );
	if( LeaderMemberInfo == NULL )
	{
		SC_SetGuildFlagInfoResult( Owner->GUID() , EM_SetGuildFlagInfoResult_GuildError );
		return;
	}

	GuildStruct* GuildInfo = LeaderMemberInfo->Guild;

	//檢查是否為公會會長
	if( GuildInfo->Base.LeaderDBID != Owner->DBID() )
	{
		Owner->Msg( "不是公會會長" );
		SC_SetGuildFlagInfoResult( Owner->GUID() , EM_SetGuildFlagInfoResult_LeaderError );
		return;
	}

	SD_SetGuildFlagInfoRequest( Owner->DBID() , Flag );
}

void NetSrv_GuildChild::RC_ChangeGuildNameRequest		( BaseItemObject*  Obj , char* GuildName )
{
	RoleDataEx* Owner = Obj->Role();

	if( strlen( GuildName ) == 0 )
		return;
/*
	//檢查公會
	GuildMemberStruct* LeaderMemberInfo = GuildManageClass::This()->GetMember( Owner->DBID() );
	if( LeaderMemberInfo == NULL )
	{
		return;
	}

	GuildStruct* GuildInfo = LeaderMemberInfo->Guild;

	//檢查是否為公會會長
	if( GuildInfo->Base.LeaderDBID != Owner->DBID() )
	{
		Owner->Msg( "不是公會會長" );
		return;
	}
*/
	SD_ChangeGuildNameRequest( Owner->GuildID() , Owner->DBID() , GuildName );
}

//----------------------------------------------------------------------------------------------
//公會升級
//----------------------------------------------------------------------------------------------	
void NetSrv_GuildChild::RC_GuildUpgradeRequest			( BaseItemObject* Obj , int GuildLv )
{
	SD_GuildUpgradeRequest( Obj->Role()->GuildID() , GuildLv , Obj->Role()->DBID() );
}
void NetSrv_GuildChild::RD_AddGuildResourceResult		( int GuildID , int PlayerDBID , GuildResourceStruct& Resource , char* Ret_Runplot , bool Result )
{
	if( strlen( Ret_Runplot ) == 0 )
		return;

	BaseItemObject* Obj = BaseItemObject::GetObjByDBID( PlayerDBID );
	if( Obj == NULL )
		return;

	char Buf[256];
	sprintf( Buf , "%s(%d)" , Ret_Runplot , (int)Result );

	LUA_VMClass::PCallByStrArg( Buf , Obj->Role()->GUID() , Obj->Role()->GUID() );
}

void  NetSrv_GuildChild::RC_SelectContributionLog		( BaseItemObject* Obj , int DayBefore )
{
	SD_SelectContributionLog( DayBefore , Obj->Role()->DBID() );
}
//要求公會戰歷史資料
void NetSrv_GuildChild::RC_GuildWarHistroy( BaseItemObject* obj  )
{
	SD_GuildWarHistroy( obj->Role()->DBID() );
}
