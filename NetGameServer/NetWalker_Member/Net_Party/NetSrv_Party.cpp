#include "../NetWakerGSrvInc.h"
#include "NetSrv_Party.h"
#include "../../mainproc/PartyInfoList/PartyInfoList.h"
//-------------------------------------------------------------------
NetSrv_Party*    NetSrv_Party::This         = NULL;

//-------------------------------------------------------------------
bool NetSrv_Party::_Init()
{
	_Net->RegOnSrvPacketFunction			( EM_PG_Party_ChattoL_PartyBaseInfo			, _PG_Party_ChattoL_PartyBaseInfo		);
	_Net->RegOnSrvPacketFunction			( EM_PG_Party_ChattoL_AddMember				, _PG_Party_ChattoL_AddMember			);
	_Net->RegOnSrvPacketFunction			( EM_PG_Party_ChattoL_DelMember				, _PG_Party_ChattoL_DelMember			);
	_Net->RegOnSrvPacketFunction			( EM_PG_Party_ChattoL_FixMember				, _PG_Party_ChattoL_FixMember			);

	_Net->RegOnSrvPacketFunction			( EM_PG_Party_ChattoL_GiveItem				, _PG_Party_ChattoL_GiveItem			);
	_Net->RegOnSrvPacketFunction			( EM_PG_Party_ChattoL_PartyReloginResult	, _PG_Party_ChattoL_PartyReloginResult	);

	_Net->RegOnSrvPacketFunction			( EM_PG_Party_LtoL_GetMemberInfo			, _PG_Party_LtoL_GetMemberInfo	);

	Srv_NetCliReg( PG_ZoneParty_CtoL_Invite				);
	Srv_NetCliReg( PG_ZoneParty_CtoL_Join				);
	Srv_NetCliReg( PG_ZoneParty_CtoL_KickMember			);
	Srv_NetCliReg( PG_ZoneParty_CtoL_SetMemberPosRequest);
	Srv_NetCliReg( PG_ZoneParty_CtoL_ChangeLeader		);
	Srv_NetCliReg( PG_ZoneParty_CtoL_Talk				);
	Srv_NetCliReg( PG_Party_CtoL_GetMemberInfo			);

	Global::Net_ServerList->RegServerLogoutEvent( EM_SERVER_TYPE_CHAT , _OnSrvDisconnectEvent );

    return true;
}
//-------------------------------------------------------------------
bool NetSrv_Party::_Release()
{
    return true;
}
//-------------------------------------------------------------------
void NetSrv_Party::_PG_Party_LtoL_GetMemberInfo			( int NetID , int Size , void* Data )
{
	PG_Party_LtoL_GetMemberInfo* PG = (PG_Party_LtoL_GetMemberInfo*) Data;

	This->RL_GetMemberInfo( PG->RequestPlayerDBID , PG->LeaderDBID );
}

void NetSrv_Party::_PG_Party_CtoL_GetMemberInfo			( int NetID , int Size , void* Data )
{
	PG_Party_CtoL_GetMemberInfo* PG = (PG_Party_CtoL_GetMemberInfo*) Data;
	BaseItemObject*	Obj = BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;

	This->RC_GetMemberInfo( Obj , PG->LeaderDBID );
}

void NetSrv_Party::_PG_ZoneParty_CtoL_Talk					( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj = BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;

	PG_ZoneParty_CtoL_Talk* pg =(PG_ZoneParty_CtoL_Talk	*)Data;
	This->RC_Zone_Talk( Obj , pg->Content.Begin() );
}
void NetSrv_Party::_PG_ZoneParty_CtoL_ChangeLeader			( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj = BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;

	PG_ZoneParty_CtoL_ChangeLeader* pg =(PG_ZoneParty_CtoL_ChangeLeader	*)Data;
	This->RC_Zone_ChangeLeader( Obj , pg->NewLeaderDBID );
}
void NetSrv_Party::_PG_ZoneParty_CtoL_Invite				( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj = BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;

	PG_ZoneParty_CtoL_Invite* pg =(PG_ZoneParty_CtoL_Invite	*)Data;
	This->RC_Zone_Invite( Obj , pg->InviteDBID , pg->ClientInfo );
}
void NetSrv_Party::_PG_ZoneParty_CtoL_Join					( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj = BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;

	PG_ZoneParty_CtoL_Join* pg =(PG_ZoneParty_CtoL_Join	*)Data;
	This->RC_Zone_Join( Obj , pg->LeaderDBID );
}
void NetSrv_Party::_PG_ZoneParty_CtoL_KickMember			( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj = BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;

	PG_ZoneParty_CtoL_KickMember* pg =(PG_ZoneParty_CtoL_KickMember	*)Data;
	This->RC_Zone_KickMember( Obj , pg->MemberDBID );
}
void NetSrv_Party::_PG_ZoneParty_CtoL_SetMemberPosRequest	( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj = BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;

	PG_ZoneParty_CtoL_SetMemberPosRequest* pg =(PG_ZoneParty_CtoL_SetMemberPosRequest	*)Data;
	This->RC_Zone_SetMemberPosRequest( Obj , pg->MemberDBID , pg->Pos );
}


void NetSrv_Party::_OnSrvDisconnectEvent ( EM_SERVER_TYPE ServerType, DWORD dwServerLocalID )
{
	This->OnChatServerDisconnect();
}


void NetSrv_Party::_PG_Party_ChattoL_PartyReloginResult	( int NetID , int Size , void* Data )
{
	PG_Party_ChattoL_PartyReloginResult* PG = (PG_Party_ChattoL_PartyReloginResult*)Data;

	This->R_PartyReloginResult( PG->PartyID , PG->PlayerDBID , PG->Result );
}

void NetSrv_Party::_PG_Party_ChattoL_PartyBaseInfo	( int NetID , int Size , void* Data )
{
    PG_Party_ChattoL_PartyBaseInfo* PG = (PG_Party_ChattoL_PartyBaseInfo*)Data;
	
	This->R_PartyBaseInfo( PG->Info );
}
void NetSrv_Party::_PG_Party_ChattoL_AddMember		( int NetID , int Size , void* Data )
{
    PG_Party_ChattoL_AddMember* PG = (PG_Party_ChattoL_AddMember*)Data;
	This->R_AddMember( PG->PartyID , PG->NewMember );

}
void NetSrv_Party::_PG_Party_ChattoL_DelMember		( int NetID , int Size , void* Data )
{
    PG_Party_ChattoL_DelMember* PG = (PG_Party_ChattoL_DelMember*)Data;
	This->R_DelMember( PG->PartyID , PG->Member , PG->IsOffline );
}
void NetSrv_Party::_PG_Party_ChattoL_FixMember		( int NetID , int Size , void* Data )
{
	PG_Party_ChattoL_FixMember* PG = (PG_Party_ChattoL_FixMember*)Data;
	This->R_FixMember( PG->PartyID , PG->Member );
}

void NetSrv_Party::_PG_Party_ChattoL_GiveItem		( int NetID , int Size , void* Data )
{
	PG_Party_ChattoL_GiveItem* PG = (PG_Party_ChattoL_GiveItem*)Data;
	This->R_GiveItem( PG->DBID , PG->GItemID , PG->Item );
}
/*
void NetSrv_Party::S_PartyMemberInfo( int GItemID , PartyMemberBaseInfo& Info )
{
	PG_Party_LtoC_PartyMemberInfo Send;
	Send.Info = Info;

	SendToCli_ByGUID( GItemID  , sizeof( Send ) , &Send );
}

void	NetSrv_Party::S_AllParty_PartyMemberInfo( int PartyID , PartyMemberBaseInfo& Info )
{
	PartyInfoStruct* Party = PartyInfoListClass::This()->GetPartyInfo( PartyID );	

	for( int i = 0 ; i < Party->Member.size() ; i++ )
	{
		S_PartyMemberInfo( Party->Member[i].GItemID );
	}
}
*/
void NetSrv_Party::S_PartyTreasure( int PartyID , ItemFieldStruct& Item , StaticVector< int , _MAX_PARTY_COUNT_  >& LootDBID )
{
	PG_Party_LtoChat_PartyTreasure Send;
	Send.PartyID = PartyID;
	Send.Item = Item;
	Send.LootDBID = LootDBID;

	SendToChat( sizeof( Send ) , &Send );
}

void NetSrv_Party::S_Relogin( int PartyID , int PlayerDBID )
{
	PG_Party_LtoChat_PartyRelogin Send;
	Send.PartyID = PartyID;
	Send.PlayerDBID = PlayerDBID;

	SendToChat( sizeof( Send ) , &Send );
}

void NetSrv_Party::S_MemberLogout( int ToMemberDBID , int LogoutMemberDBID )
{
	PG_Party_LtoC_MemberLogout Send;
	Send.MemberDBID = LogoutMemberDBID;
	SendToCli_ByDBID( ToMemberDBID , sizeof(Send) , &Send );
}

void NetSrv_Party::SC_Zone_Invite	( int SendID , int InviteDBID , int ClientInfo )
{
	PG_ZoneParty_LtoC_Invite Send;
	Send.LeaderDBID = InviteDBID;
	Send.ClientInfo = ClientInfo;
	SendToCli_ByGUID( SendID , sizeof(Send) , &Send );
}
void NetSrv_Party::SC_Zone_AddMember	( int SendID , int PartyID , int MemberDBID , int PartyNo , int MemberID )
{
	PG_ZoneParty_LtoC_AddMember Send;
	Send.PartyID = PartyID;
	Send.MemberDBID = MemberDBID;
	Send.PartyNo = PartyNo;
	Send.MemberID = MemberID;
	SendToCli_ByGUID( SendID , sizeof(Send) , &Send );
}
void NetSrv_Party::SC_Zone_DelMember	( int SendID , int PartyID , int MemberDBID )
{
	PG_ZoneParty_LtoC_DelMember Send;
	Send.MemberDBID = MemberDBID;
	SendToCli_ByGUID( SendID , sizeof(Send) , &Send );
}
void NetSrv_Party::SC_Zone_FixMember	( int SendID , int PartyID , int MemberDBID , int PartyNo , int MemberID )
{
	PG_ZoneParty_LtoC_FixMember Send;
	Send.PartyID = PartyID;
	Send.MemberDBID = MemberDBID;
	Send.PartyNo = PartyNo;
	Send.MemberID = MemberID;

	SendToCli_ByGUID( SendID , sizeof(Send) , &Send );
}
void NetSrv_Party::SC_Member_Zone_FixMember	( int PartyID , int MemberDBID , int PartyNo , int MemberID )
{
	PartyInfoStruct* Party = PartyInfoListClass::This()->GetPartyInfo( PartyID );
	if( Party == NULL )
		return;

	vector< PartyMemberInfoStruct >& PartyMember = Party->Member;
	for( unsigned i = 0 ; i < PartyMember.size() ; i++ )
	{
		SC_Zone_FixMember( PartyMember[i].GItemID , PartyID , MemberDBID , PartyNo , MemberID );
	}

}

void NetSrv_Party::SC_Zone_SetMemberPosResult	( int SendID , bool Result  )
{
	PG_ZoneParty_LtoC_SetMemberPosResult Send;
	Send.Result = Result;
	SendToCli_ByGUID( SendID , sizeof(Send) , &Send );
}
void NetSrv_Party::SC_Zone_PartyBase			( int SendID , int PartyID , int LeaderDBID )
{
	PG_ZoneParty_LtoC_PartyBase Send;
	Send.PartyID = PartyID;
	Send.LeaderDBID = LeaderDBID;
	SendToCli_ByGUID( SendID , sizeof(Send) , &Send );
}

void NetSrv_Party::SC_Zone_ChangeLeaderResult	( int SendID , bool Result )
{
	PG_ZoneParty_LtoC_ChangeLeaderResult Send;
	Send.Result = Result;
	SendToCli_ByGUID( SendID , sizeof(Send) , &Send );
}

void NetSrv_Party::SC_Zone_Talk				( int GItemID , const char* Content  )
{
	/*
	PG_ZoneParty_LtoC_Talk Send;
	PartyInfoStruct* Party = PartyInfoListClass::This()->GetPartyInfo( PartyID );
	if( Party == NULL )
		return;
	Send.GItemID = GItemID;
	Send.Content = Content;
	Send.ContentSize = Send.Content.Size() + 1;

	vector< PartyMemberInfoStruct >& PartyMember = Party->Member;
	for( unsigned i = 0 ; i < PartyMember.size() ; i++ )
	{
		SendToCli_ByGUID( PartyMember[i].GItemID , Send.Size() , &Send );
	}
*/
	RoleDataEx *Owner = Global::GetRoleDataByGUID( GItemID );
	if( Owner == NULL )
		return;

	PG_ZoneParty_LtoC_Talk Send;
	Send.GItemID = GItemID;
	Send.Content = Content;
	Send.ContentSize = Send.Content.Size() + 1;

	set<BaseItemObject* >&	PlayObjSet = *( BaseItemObject::PlayerObjSet() );
	set<BaseItemObject* >::iterator Iter;

	for( Iter = PlayObjSet.begin() ; Iter != PlayObjSet.end() ; Iter++ )
	{			
		RoleDataEx* Role = (*Iter)->Role();		
		if(		Role->RoomID() != Owner->RoomID() 
			||	Role->TempData.AI.CampID != Owner->TempData.AI.CampID )
			continue;
		SendToCli_ByGUID( Role->GUID() , Send.Size() , &Send );
	}



}

void NetSrv_Party::SC_GetMemberInfoResult( int SendDBID , int LeaderDBID , int TotalCount , int ID , PartyMemberInfoStruct& Member )
{
	PG_Party_LtoC_GetMemberInfoResult Send;
	Send.ID = ID;
	Send.LeaderDBID = LeaderDBID;
	Send.TotalCount = TotalCount;
	Send.Member = Member;
	SendToCli_ByDBID( SendDBID , sizeof(Send) , &Send );
}

void NetSrv_Party::SL_GetMemberInfo			( int ZoneID , int RequestPlayerDBID , int LeaderDBID )
{
	PG_Party_LtoL_GetMemberInfo Send;
	Send.LeaderDBID = LeaderDBID;
	Send.RequestPlayerDBID = RequestPlayerDBID;
	SendToLocal( ZoneID , sizeof(Send) , &Send );
}