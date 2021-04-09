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
//  �ƥ��~�ӹ갵
//-------------------------------------------------------------------
void NetSrv_PartyChild::R_Invite( BaseItemObject* Sender , const char* InviteName , int ClientInfo )
{
	//�q�����H�n��
	BaseItemObject* Target = BaseItemObject::GetObjByRoleName( InviteName );         //������
	if( Target == NULL )
	{
		//NetSrv_Talk::SysMsg( Sender->SockID() , "�n�ХؼФ��s�b" );
		return;
	}
	if( Target == Sender )
	{
		//NetSrv_Talk::SysMsg( Sender->SockID() , "����n�Цۤv" );
		return;
	}
	if( Target->PartyID( ) != -1 )
	{
		NetSrv_Talk::GameMsgEvent( Sender->SockID() , EM_GameMessageEvent_TargetHasParty );
		//NetSrv_Talk::SysMsg( Sender->SockID() , "�ؼФw�[�J����" );
		return;
	}
	if( Target->Role()->State.HonorParty )
	{
		//NetSrv_Talk::SysMsg( Sender->SockID() , "�����ܽЦ��a�A�ն��ĪG���H" );
		return;
	}
	//�ˬd�O�_�n�Ъ̦�����(����) or �S����A
	if( Sender->PartyID() != -1 )
	{
		GroupParty* Party = (GroupParty*)GroupObjectClass::GetObj( EM_GroupObjectType_Party , Sender->PartyID() );

		if( Party == NULL )
		{
			Print( LV2 , "R_Invite" , "�n�Ъ̪�PartyID�����D" );
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
			NetSrv_Talk::SysMsg_ChartoUtf8( Sender->SockID() , "�����~�i�H�n��" );
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
	BaseItemObject* Leader = BaseItemObject::GetObjByRoleName( LeaderName );         //������

	if( Leader == NULL )
	{
		NetSrv_Talk::SysMsg_ChartoUtf8( Sender->SockID() , "�������b�u�W" );
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
			NetSrv_Talk::SysMsg_ChartoUtf8( Sender->SockID() , "??�䤣��Party���" );
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
				{//�a�A�ն��ɡA�H�Ƥ��o�W�L�G�H
					//NetSrv_Talk::GameMsgEvent( Leader->SockID() , EM_GameMessageEvent_HonorParty_NumLimit );
					NetSrv_Talk::GameMsgEvent( Sender->SockID() , EM_GameMessageEvent_HonorParty_NumLimit );
					return;
				}
			}
			else if( Party->MemberCount() >= _MAX_PARTY_COUNT_PARTY_ )
			{
				NetSrv_Talk::SysMsg_ChartoUtf8( Sender->SockID() , "����w��" );
				return;
			}

		}
		else
		{
			if( Party->MemberCount() >= _MAX_PARTY_COUNT_RAID_ )
			{
				NetSrv_Talk::SysMsg_ChartoUtf8( Sender->SockID() , "Raid�w��" );
				return;
			}

		}
		if(		strcmp(  Party->Info()->LeaderName.Begin() , LeaderName ) != 0  
			&&	LeaderMemberInfo->Info.Mode.IsAssistant == false )
		{
			sprintf( Buf , "%s ���O����" , LeaderName );
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
	BaseItemObject* Member = BaseItemObject::GetObjByRoleName( MemberName );         //������
	if( Member ==  NULL ||  Member->PartyID() != Sender->PartyID() )
	{
		Party = (GroupParty*)GroupObjectClass::GetObj( EM_GroupObjectType_Party , PartyID );

		if( Party == NULL )
		{
			NetSrv_Talk::SysMsg_ChartoUtf8( Sender->SockID() , "�䤣�즹�H �P Party" );
		}
		else
		{
			if( strcmp( Sender->RoleName() , Party->Info()->LeaderName.Begin() ) != 0 )
			{
				NetSrv_Talk::SysMsg_ChartoUtf8( Sender->SockID() , "�n�����~�i�H��H" );
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
		sprintf( Buf , "%s ���b���" , MemberName );
		NetSrv_Talk::SysMsg_ChartoUtf8( Sender->SockID() , Buf );
		return;
	}

	Party = (GroupParty*)GroupObjectClass::GetObj( EM_GroupObjectType_Party , Member->PartyID() );
	if( Party == NULL )
	{
		NetSrv_Talk::SysMsg_ChartoUtf8( Sender->SockID() , "??�䤣��Party���" );
		return;
	}
	//�p�G���O�ۤv  �]���O����
	if( Member != Sender && strcmp( Sender->RoleName() , Party->Info()->LeaderName.Begin() ) != 0 )
	{
		NetSrv_Talk::SysMsg_ChartoUtf8( Sender->SockID() , "�n�����~�i�H��H" );
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
		NetSrv_Talk::SysMsg_ChartoUtf8( Sender->SockID() , "??�䤣��Party���" );
		return;
	}
	if( strcmp( Sender->RoleName() , Party->Info()->LeaderName.Begin() ) != 0 )
	{
		NetSrv_Talk::SysMsg_ChartoUtf8( Sender->SockID() , "�����~�i�H�]�w" );
		return;
	}
	Info.PartyID = Sender->PartyID();
	if( Party->SetRule( Info ) == false )
	{
		//NetSrv_Talk::SysMsg( Sender->SockID() , "�]�w����" );
	};
}

void NetSrv_PartyChild::R_PartyTreasure( int PartyID , ItemFieldStruct& Item , StaticVector< int , _MAX_PARTY_COUNT_  >&  LootDBID  )
{
	static int ItemVersion = 0;
	ItemVersion++;

	//�q��Client
	GroupParty*			Party;
	Party = (GroupParty*)GroupObjectClass::GetObj( EM_GroupObjectType_Party , PartyID );
	if( Party == NULL )
	{
		Print( LV2 , "R_PartyTreasure" , "??�䤣��Party���" );
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
		Print( LV2 , "R_PartyTreasureLoot" , "??�䤣��Party��� Name = %s" , Sender->RoleName()	);
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
		Print( LV2 , "R_PartyRelogin" , "??�䤣��Party��� PlayerDBID = %d" , PlayerDBID	);
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
	{	//�S���v�½s��
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
	{	//�S���v�½s��		
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