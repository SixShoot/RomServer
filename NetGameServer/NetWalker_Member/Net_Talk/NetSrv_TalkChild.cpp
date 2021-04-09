#include "../NetWakerGSrvInc.h"
#include "NetSrv_TalkChild.h"
//-----------------------------------------------------------------
//-----------------------------------------------------------------
bool    NetSrv_TalkChild::Init()
{
    NetSrv_Talk::_Init();

    if( This != NULL)
        return false;

    This = NEW( NetSrv_TalkChild );

    return true;
}
//-----------------------------------------------------------------
bool    NetSrv_TalkChild::Release()
{
    if( This == NULL )
        return false;

    NetSrv_Talk::_Release();

    delete This;
    This = NULL;

    return true;
    
}
//-----------------------------------------------------------------
void NetSrv_TalkChild::Player_TalkTo      ( BaseItemObject* Sender, const char* TargetName , int ContentSize , const char* Content , TalkSenderInfo& SenderInfo )
{
	if( Sender->Role()->BaseData.SysFlag.DisableTalk || Sender->Role()->TempData.Attr.Effect.Silence != false )
	{
		Sender->Role()->Net_GameMsgEvent( EM_GameMessageEvent_TalkDisable );
		return;
	}

	RoleDataEx* Owner = Sender->Role();
	if( Owner->PlayerTempData->PacketCheckPoint[ EM_ClientPacketCheckType_Talk ] > 10 )
		return;

	//信任玩機制
	if( Owner->PlayerBaseData->VipMember.UntrustFlag.DisablePlayerChannelTalk  )
	{
		//Owner->Msg( "信任玩家機制測試_無法密語" );
		Owner->Net_GameMsgEvent( EM_GameMessageEvent_Role_Untrust);
		return;
	}

	Owner->PlayerTempData->PacketCheckPoint[ EM_ClientPacketCheckType_Talk ] += (1.0f/12.0f);

    TalkToByRoleName( Sender , false , TargetName , Content , SenderInfo );
}
//-----------------------------------------------------------------
void NetSrv_TalkChild::Player_Range       ( BaseItemObject* Sender , int ContentSize , const char* Content , TalkSenderInfo& SenderInfo )
{
	if( Sender->Role()->BaseData.SysFlag.DisableTalk || Sender->Role()->TempData.Attr.Effect.Silence != false )
	{
		Sender->Role()->Net_GameMsgEvent( EM_GameMessageEvent_TalkDisable );
		return;
	}
	RoleDataEx* Owner = Sender->Role();
	if( Owner->PlayerTempData->PacketCheckPoint[ EM_ClientPacketCheckType_Talk ] > 10 )
		return;
	Owner->PlayerTempData->PacketCheckPoint[ EM_ClientPacketCheckType_Talk ] += (1.0f/12.0f);

    Range( Sender->GUID() , false , Content, false , &SenderInfo  );
	Global::Log_Talk( EM_TalkLogType_Range , RoleDataEx::G_ZoneID , Sender->Role()->DBID() , 0 , Content );
}
//-----------------------------------------------------------------
/*
void NetSrv_TalkChild::Player_Zone        ( BaseItemObject* Sender , int ContentSize , const char* Content )
{
	Zone( Sender->GUID() , Content );
}
*/
//-----------------------------------------------------------------
void NetSrv_TalkChild::Player_GMCommand   ( BaseItemObject* Sender , int ContentSize , const char* Content )
{
	RoleDataEx* Owner = Sender->Role();

	_St->GM.GMCmdProc( Sender , Content , Sender->Role()->RoleName() , Sender->Role()->PlayerBaseData->ManageLV , true );
}
//-----------------------------------------------------------------
void NetSrv_TalkChild::ToGMTools          ( BaseItemObject* Sender , const char* Content )
{
    NetSrv_GMTools::S_PlayerMsg( DF_NO_LOCALID , (char*)Sender->Role()->RoleName() , (char*) Content );
}
//-----------------------------------------------------------------
void NetSrv_TalkChild::R_Guild( BaseItemObject* Sender , int ContentSize , const char* Content , TalkSenderInfo& SenderInfo )
{
	if( Sender->Role()->BaseData.SysFlag.DisableTalk || Sender->Role()->TempData.Attr.Effect.Silence != false )
	{
		Sender->Role()->Net_GameMsgEvent( EM_GameMessageEvent_TalkDisable );
		return;
	}

	GuildMemberStruct* Member = GuildManageClass::This()->GetMember( Sender->Role()->DBID() );

	if( Member == NULL )
		return;

	Guild_All( Member->Voc , Member->GuildID , EM_TalkChannelType_Normal , Sender->Role()->RoleName() , Content , SenderInfo );

	Global::Log_Talk( EM_TalkLogType_Guild , Sender->Role()->GuildID() , Sender->Role()->DBID() , 0 , Content );

}
//-----------------------------------------------------------------
void NetSrv_TalkChild::R_SysChannel		( BaseItemObject* Sender , GroupObjectTypeEnum Type , int ChannelID , int ContentSize , char* Content , TalkSenderInfo& SenderInfo )
{
	if( Sender->Role()->BaseData.SysFlag.DisableTalk || Sender->Role()->TempData.Attr.Effect.Silence != false )
	{
		Sender->Role()->Net_GameMsgEvent( EM_GameMessageEvent_TalkDisable );
		return;
	}

	RoleDataEx* Owner = Sender->Role();
	if( Owner->PlayerTempData->PacketCheckPoint[ EM_ClientPacketCheckType_Talk ] > 10 )
		return;
	Owner->PlayerTempData->PacketCheckPoint[ EM_ClientPacketCheckType_Talk ] += (1.0f/12.0f);
	
	//信任玩機制
	if( Type == EM_GroupObjectType_SysChannel && Owner->PlayerBaseData->VipMember.UntrustFlag.DisableSysChannelTalk )
	{
		//Owner->Msg( "信任玩家機制測試_無法使用世界頻道" );
		Owner->Net_GameMsgEvent( EM_GameMessageEvent_Role_Untrust);
		return;
	}
	else if( Type == EM_GroupObjectType_SysZoneChannel && Owner->PlayerBaseData->VipMember.UntrustFlag.DisableZoneChannelTalk )
	{
		//Owner->Msg( "信任玩家機制測試_無法使用區域頻道" );
		Owner->Net_GameMsgEvent( EM_GameMessageEvent_Role_Untrust);
		return;
	}
		

	//撿查是否有大聲公
	if( Type == EM_GroupObjectType_SysChannel )
	{
		if( Owner->TempData.Sys.OnChangeZone != false )
			return;

		if( Ini()->IsSysChannelNeedItem != false )
		{
			int NeedItemID = g_ObjectData->GetSysKeyValue( "SysChannel_NeedItemID" );
			RoleDataEx* Owner = Sender->Role();
			if( Owner->DelBodyItem( NeedItemID , 1 , EM_ActionType_SysChannel ) == false )
			{
				Sender->Role()->Net_GameMsgEvent( EM_GameMessageEvent_NeedSysChannelItem );
				return;
			}
		}
		SChat_SysChannel( Sender->Role()->DBID() , Type , EM_TalkChannelType_Normal , ChannelID , (char*)Sender->Role()->RoleName() , Content , SenderInfo );
		Global::Log_Talk( EM_TalkLogType_SystemChannel , ChannelID , Sender->Role()->DBID() , 0 , Content );
	}
/*
	if( Type == EM_GroupObjectType_SysChannel )
	{
		SChat_SysChannel( Sender->Role()->DBID() , Type , EM_TalkChannelType_Normal , ChannelID , (char*)Sender->Role()->RoleName() , Content );
		Global::Log_Talk( EM_TalkLogType_SystemChannel , ChannelID , Sender->Role()->DBID() , 0 , Content );
	}
	*/
	else if( Type == EM_GroupObjectType_SysZoneChannel )
	{

		//認定為幣商 頻道發言無效
		if( Sender->Role()->PlayerBaseData->Defendant.MoneyTraderCount >= _DEF_MAX_MONEY_VENTOR_COUNT_ - 2 )
			return;


		int		iLimitLV	= Ini()->ZoneChannelBroadcastLv;
		bool	IsSay		= false;
		//檢查等級
		for( int i = 0 ; i < EM_Max_Vocation ; i++ )
		{
			if( Sender->Role()->PlayerBaseData->AbilityList[i].Level >= iLimitLV )
			{
				IsSay = true;
				SChat_SysChannel( Sender->Role()->DBID() , Type , EM_TalkChannelType_Normal , ChannelID , (char*)Sender->Role()->RoleName() , Content , SenderInfo );
				Global::Log_Talk( EM_TalkLogType_ZoneChannel , RoleDataEx::G_ZoneID * 100 + ChannelID , Sender->Role()->DBID() , 0 , Content );
				break;
			}
		}

		if( IsSay == false )
		{
			//Sender->Role()->GameMsgEventData( EM_GameMessageEvent_ZoneChannelLevelErr, EM_GameMessageDataType_Int, iLimitLV  );
			GameMsgEventData( Sender->Role()->GUID(), 1, EM_GameMessageEventData_ZoneChannelLevelErr, EM_GameMessageDataType_Int, iLimitLV  );
		}
		
	}
}

void NetSrv_TalkChild::RD_OfflineGMCommand( int PlayerDBID , ManagementENUM ManageLv , const char* Content )
{
	BaseItemObject*	Obj =	Global::GetPlayerObj_ByDBID( PlayerDBID );

	if( Obj == NULL )
		Global::Log_GMToolsGMCommand(  "Offline GM Command" , ManageLv , -1 , Content );
	else
		Global::Log_GMToolsGMCommand(  "Offline GM Command" , ManageLv , Obj->Role()->DBID() , Content );

	_St->GM.GMCmdProc( Obj , Content , "Offline GM Command" , ManageLv );
}