#include "../NetWakerChatInc.h"
#include "NetChat_PartyChild.h"
#include "../../MainProc/GroupParty/GroupParty.h"

//-----------------------------------------------------------------
//-----------------------------------------------------------------
bool    NetSrv_PartyChild::Init()
{
    NetSrv_Party::_Init();

    if( This != NULL)
        return false;

    This = new NetSrv_PartyChild;

    return true;
}
//-----------------------------------------------------------------
bool    NetSrv_PartyChild::Release()
{
    if( This == NULL )
        return false;

    delete This;
    This = NULL;

    NetSrv_Party::_Release();
    return true;
    
}
//-------------------------------------------------------------------
//  事件繼承實做
//-------------------------------------------------------------------
void NetSrv_PartyChild::R_Invite( BaseItemObject* Sender , const char* InviteName , int ClientInfo )
{
	//通知有人要請
	BaseItemObject* Target = BaseItemObject::GetObjByRoleName( InviteName );         //取物件
	if( Target == NULL )
	{
		//NetSrv_Talk::SysMsg( Sender->SockID() , "要請目標不存在" );
		return;
	}
	if( Target == Sender )
	{
		//NetSrv_Talk::SysMsg( Sender->SockID() , "不能要請自己" );
		return;
	}
	if( Target->PartyID( ) != -1 )
	{
		NetSrv_Talk::GameMsgEvent( Sender->SockID() , EM_GameMessageEvent_TargetHasParty );
		//NetSrv_Talk::SysMsg( Sender->SockID() , "目標已加入隊伍" );
		return;
	}
	if( Target->Role()->State.HonorParty )
	{
		//NetSrv_Talk::SysMsg( Sender->SockID() , "不能邀請有榮譽組隊效果的人" );
		return;
	}
	//檢查是否要請者有隊伍(隊長) or 沒隊伍狀態
	if( Sender->PartyID() != -1 )
	{
		GroupParty* Party = (GroupParty*)GroupObjectClass::GetObj( EM_GroupObjectType_Party , Sender->PartyID() );

		if( Party == NULL )
		{
			Print( LV2 , "R_Invite" , "要請者的PartyID有問題" );
			return;
		}
		GroupPartyMemberStruct* Member = Party->MemberByDBID( Sender->DBID() ); 
		if( Member == NULL )
		{
			Print( LV2 , "R_Invite" , "Member == NULL " );
			return;
		}

		if(		strcmp( Party->Info()->LeaderName.Begin() , Sender->RoleName()  ) != 0  
			&&	Member->Info.Mode.IsAssistant == false )
		{
			NetSrv_Talk::SysMsg_ChartoUtf8( Sender->SockID() , "隊長才可以要請" );
			return;
		}
	}

	SC_Invite( Target->SockID() , Sender->RoleName() , ClientInfo );

}
void NetSrv_PartyChild::R_Join( BaseItemObject* Sender , const char* LeaderName )
{
	char	Buf[256];
//	PartyBaseInfoStruct PartyInfo;
	GroupParty*			Party;
	BaseItemObject* Leader = BaseItemObject::GetObjByRoleName( LeaderName );         //取物件

	if( Leader == NULL )
	{
		NetSrv_Talk::SysMsg_ChartoUtf8( Sender->SockID() , "隊長不在線上" );
		return;
	}


	if( Leader->PartyID( ) == -1 )
	{
		Party = new GroupParty( );
		Party->AddUser( Leader );	
	}
	else
	{
		Party = (GroupParty*)GroupObjectClass::GetObj( EM_GroupObjectType_Party , Leader->PartyID() );
		if( Party == NULL )
		{
			NetSrv_Talk::SysMsg_ChartoUtf8( Sender->SockID() , "??找不到Party資料" );
			return;
		}

		GroupPartyMemberStruct* LeaderMemberInfo = Party->MemberByDBID( Leader->DBID() ); 
		if( LeaderMemberInfo == NULL )
		{
			Print( LV2 , "R_Join" , "LeaderMemberInfo == NULL " );
			return;
		}


		if( Party->Info()->Type == EM_PartyType_Party )
		{
			if( Leader->Role()->State.HonorParty )
			{
				if( Party->MemberCount() >= 2 )
				{//榮譽組隊時，人數不得超過二人
					//NetSrv_Talk::GameMsgEvent( Leader->SockID() , EM_GameMessageEvent_HonorParty_NumLimit );
					NetSrv_Talk::GameMsgEvent( Sender->SockID() , EM_GameMessageEvent_HonorParty_NumLimit );
					return;
				}
			}
			else if( Party->MemberCount() >= _MAX_PARTY_COUNT_PARTY_ )
			{
				NetSrv_Talk::SysMsg_ChartoUtf8( Sender->SockID() , "隊伍已滿" );
				return;
			}

		}
		else
		{
			if( Party->MemberCount() >= _MAX_PARTY_COUNT_RAID_ )
			{
				NetSrv_Talk::SysMsg_ChartoUtf8( Sender->SockID() , "Raid已滿" );
				return;
			}

		}
		if(		strcmp(  Party->Info()->LeaderName.Begin() , LeaderName ) != 0  
			&&	LeaderMemberInfo->Info.Mode.IsAssistant == false )
		{
			sprintf( Buf , "%s 不是隊長" , LeaderName );
			NetSrv_Talk::SysMsg_ChartoUtf8( Sender->SockID() , Buf );
			return;
		}
	}

	Party->AddUser( Sender );
}
void NetSrv_PartyChild::R_KickMember( BaseItemObject* Sender ,const char* MemberName , int PartyID )
{
	char	Buf[256];
	PartyBaseInfoStruct PartyInfo;
	GroupParty*			Party;
	BaseItemObject* Member = BaseItemObject::GetObjByRoleName( MemberName );         //取物件
	if( Member ==  NULL ||  Member->PartyID() != Sender->PartyID() )
	{
		Party = (GroupParty*)GroupObjectClass::GetObj( EM_GroupObjectType_Party , PartyID );

		if( Party == NULL )
		{
			NetSrv_Talk::SysMsg_ChartoUtf8( Sender->SockID() , "找不到此人 與 Party" );
		}
		else
		{
			if( strcmp( Sender->RoleName() , Party->Info()->LeaderName.Begin() ) != 0 )
			{
				NetSrv_Talk::SysMsg_ChartoUtf8( Sender->SockID() , "要隊長才可以踢人" );
				return;
			}

			Party->DelOfflineUser( MemberName );	

			if( Party->Member()->size() == 0 )
			{
				delete Party;
			}
		}
		return;
	}

	if( Member->PartyID() != Sender->PartyID() )
	{
		sprintf( Buf , "%s 不在隊伍內" , MemberName );
		NetSrv_Talk::SysMsg_ChartoUtf8( Sender->SockID() , Buf );
		return;
	}

	Party = (GroupParty*)GroupObjectClass::GetObj( EM_GroupObjectType_Party , Member->PartyID() );
	if( Party == NULL )
	{
		NetSrv_Talk::SysMsg_ChartoUtf8( Sender->SockID() , "??找不到Party資料" );
		return;
	}
	//如果不是自己  也不是隊長
	if( Member != Sender && strcmp( Sender->RoleName() , Party->Info()->LeaderName.Begin() ) != 0 )
	{
		NetSrv_Talk::SysMsg_ChartoUtf8( Sender->SockID() , "要隊長才可以踢人" );
		return;
	}
	Party->DelUser( Member );	
	if( Party->Member()->size() == 0 )
	{
		delete Party;
	}
}
void NetSrv_PartyChild::R_PartyRule( BaseItemObject* Sender , PartyBaseInfoStruct& Info )
{
	GroupParty*			Party;
	Party = (GroupParty*)GroupObjectClass::GetObj( EM_GroupObjectType_Party , Sender->PartyID() );
	if( Party == NULL )
	{
		NetSrv_Talk::SysMsg_ChartoUtf8( Sender->SockID() , "??找不到Party資料" );
		return;
	}
	if( strcmp( Sender->RoleName() , Party->Info()->LeaderName.Begin() ) != 0 )
	{
		NetSrv_Talk::SysMsg_ChartoUtf8( Sender->SockID() , "隊長才可以設定" );
		return;
	}
	Info.PartyID = Sender->PartyID();
	if( Party->SetRule( Info ) == false )
	{
		//NetSrv_Talk::SysMsg( Sender->SockID() , "設定失敗" );
	};
}

void NetSrv_PartyChild::R_PartyTreasure( int PartyID , ItemFieldStruct& Item , StaticVector< int , _MAX_PARTY_COUNT_  >&  LootDBID  )
{
	static int ItemVersion = 0;
	ItemVersion++;

	//通知Client
	GroupParty*			Party;
	Party = (GroupParty*)GroupObjectClass::GetObj( EM_GroupObjectType_Party , PartyID );
	if( Party == NULL )
	{
		Print( LV2 , "R_PartyTreasure" , "??找不到Party資料" );
		return;
	}

	Party->AddLootItem( Item , LootDBID );
}

void NetSrv_PartyChild::R_PartyTreasureLoot( BaseItemObject* Sender , int ItemVersion , PartyTreasureLootTypeENUM Type , int DBID )
{
	GroupParty*			Party;
	Party = (GroupParty*)GroupObjectClass::GetObj( EM_GroupObjectType_Party , Sender->PartyID() );
	if( Party == NULL )
	{
		Print( LV2 , "R_PartyTreasureLoot" , "??找不到Party資料 Name = %s" , Sender->RoleName()	);
		return;
	}

	switch( Type )
	{
	case EM_PartyTreasureLootType_Loot:
		Party->PlayerLoot( Sender->DBID() , ItemVersion , 100 );
		break;

	case EM_PartyTreasureLootType_Loot_NotNeed:
		Party->PlayerLoot( Sender->DBID() , ItemVersion , 0 );
		break;

	case EM_PartyTreasureLootType_Pass:
		Party->PlayerGiveUp( Sender->DBID() , ItemVersion );
		break;
	case EM_PartyTreasureLootType_Assist:
		Party->PlayerAssist( Sender->DBID() , DBID , ItemVersion );
		break;
	}
}

void NetSrv_PartyChild::OnGameSrvConnect( int dwServerLocalID )
{
	Print( LV2 , "OnGameSrvConnect" , "dwServerLocalID =%d" , dwServerLocalID );
	int PartyCount = GroupObjectClass::MaxCount( EM_GroupObjectType_Party );
	GroupParty* Party;

	for( int PartyID = 0 ; PartyID < PartyCount ; PartyID++ )
	{
		Party = ( GroupParty* )GroupObjectClass::GetObj( EM_GroupObjectType_Party , PartyID );
		if( Party == NULL )
			continue;

		NetSrv_Party::SL_PartyBaseInfo( dwServerLocalID , *Party->Info() );
		for( int j = 0 ; j < Party->OnlineMemberCount() ; j++ )
		{
			GroupPartyMemberStruct* M = Party->Member( j );

			NetSrv_Party::SL_AddMember( dwServerLocalID , PartyID , M->Info );
		}
	}
}

void NetSrv_PartyChild::R_PartyRelogin( int SockID , int PartyID , int PlayerDBID )
{
	GroupParty*			Party;
	Party = (GroupParty*)GroupObjectClass::GetObj( EM_GroupObjectType_Party , PartyID );
	if( Party == NULL )
	{
		Print( LV2 , "R_PartyRelogin" , "??找不到Party資料 PlayerDBID = %d" , PlayerDBID	);
		SL_PartyReloginResult( SockID , PartyID , PlayerDBID , false );
		return;
	}
	
	if( Party->AddLoginUser( PlayerDBID ) == true )
		SL_PartyReloginResult( SockID , PartyID , PlayerDBID , true );
	else
		SL_PartyReloginResult( SockID , PartyID , PlayerDBID , false );
}

void NetSrv_PartyChild::R_SetMemberPosRequest( BaseItemObject* Sender , int MemberDBID[2] , int Pos[2] )
{
	GroupParty*	Party = (GroupParty*)GroupObjectClass::GetObj( EM_GroupObjectType_Party , Sender->PartyID() );

	if( Party == NULL )
	{
		SC_SetMemberPosResult( Sender->SockID() , false );
		return;
	}

	GroupPartyMemberStruct* Member_Sender = Party->MemberByDBID( Sender->DBID() ); 
	if( Member_Sender == NULL )
	{
		Print( LV2 , "R_SetMemberPosRequest" , "Member_Sender == NULL " );
		return;
	}

	if( Member_Sender->Info.Mode.IsAssistant == false && strcmp( Party->Info()->LeaderName.Begin() , Sender->RoleName() ) != 0 )
	{	//沒有權威編輯
		SC_SetMemberPosResult( Sender->SockID() , false );
		return;
	}
	
	bool Result = Party->SetPartyMemberPos( MemberDBID , Pos );
	SC_SetMemberPosResult( Sender->SockID() , Result );

}
void NetSrv_PartyChild::R_SetMemberMode( BaseItemObject* Sender , int MemberDBID , PartyMemberModeStruct Mode )
{
	GroupParty*	Party = (GroupParty*)GroupObjectClass::GetObj( EM_GroupObjectType_Party , Sender->PartyID() );

	if( Party == NULL )
	{		
		return;
	}

	GroupPartyMemberStruct* Member_Sender = Party->MemberByDBID( Sender->DBID() ); 
	if( Member_Sender == NULL )
	{
		Print( LV2 , "R_SetMemberMode" , "Member_Sender == NULL " );
		return;
	}

/*	if( strcmp( Party->Info()->LeaderName.Begin() , Sender->RoleName() ) != 0 )
	{	//沒有權威編輯		
		return;
	}*/

	GroupPartyMemberStruct* Member = Party->MemberByDBID( MemberDBID );
	if( Member == NULL )
	{
		return;
	}

	Member->Info.Mode._Mode = Mode._Mode;
	
	Party->FixMemberInfo( &Member->Info );
}