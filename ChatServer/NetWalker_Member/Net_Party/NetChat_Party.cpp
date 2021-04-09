#include "NetChat_Party.h"
#include "../../MainProc/Global.h"
#include "../../MainProc/GroupParty/GroupParty.h"
//-------------------------------------------------------------------
NetSrv_Party*    NetSrv_Party::This         = NULL;
//-------------------------------------------------------------------
bool NetSrv_Party::_Init()
{
	Srv_NetCliReg( PG_Party_CtoChat_Invite	 );
	Srv_NetCliReg( PG_Party_CtoChat_Join		 );
	Srv_NetCliReg( PG_Party_CtoChat_KickMember );
	Srv_NetCliReg( PG_Party_CtoChat_PartyRule	 );

	Srv_NetCliReg( PG_Party_CtoChat_PartyTreasureLoot	 );

	Srv_NetCliReg( PG_Party_CtoChat_SetMemberPosRequest	 );
	Srv_NetCliReg( PG_Party_CtoChat_SetMemberMode	 );

    _Net->RegOnSrvPacketFunction	( EM_PG_Party_LtoChat_PartyTreasure			, _PG_Party_LtoChat_PartyTreasure );
	_Net->RegOnSrvPacketFunction	( EM_PG_Party_LtoChat_PartyRelogin			, _PG_Party_LtoChat_PartyRelogin );
	_Net->RegOnSrvPacketFunction	( EM_PG_Party_CtoChat_SetMemberPosRequest   , _PG_Party_CtoChat_SetMemberPosRequest );

	 Global::Net_ServerList->RegServerReadyEvent( EM_SERVER_TYPE_LOCAL , _OnSrvConnectEvent );

    return true;
}
//-------------------------------------------------------------------
bool NetSrv_Party::_Release()
{
    return true;
}
//-------------------------------------------------------------------
void NetSrv_Party::_PG_Party_CtoChat_SetMemberPosRequest( int Sockid , int Size , void* Data )
{
	PG_Party_CtoChat_SetMemberPosRequest*   PG = (PG_Party_CtoChat_SetMemberPosRequest*)Data;

	BaseItemObject* Sender = BaseItemObject::GetObjBySockID( Sockid );
	if( Sender == NULL )
		return;

	This->R_SetMemberPosRequest( Sender , PG->MemberDBID , PG->Pos );
}

void NetSrv_Party::_PG_Party_CtoChat_SetMemberMode		( int Sockid , int Size , void* Data )
{
	PG_Party_CtoChat_SetMemberMode*   PG = (PG_Party_CtoChat_SetMemberMode*)Data;

	BaseItemObject* Sender = BaseItemObject::GetObjBySockID( Sockid );
	if( Sender == NULL )
		return;

	This->R_SetMemberMode( Sender , PG->MemberDBID , PG->Mode );
}

void NetSrv_Party::_OnSrvConnectEvent ( EM_SERVER_TYPE ServerType, DWORD dwServerLocalID )
{
	This->OnGameSrvConnect( dwServerLocalID );
}
//-------------------------------------------------------------------
void NetSrv_Party::_PG_Party_LtoChat_PartyRelogin		( int Sockid , int Size , void* Data )
{
	PG_Party_LtoChat_PartyRelogin*   PG = (PG_Party_LtoChat_PartyRelogin*)Data;
	This->R_PartyRelogin( Sockid , PG->PartyID , PG->PlayerDBID );
}

void NetSrv_Party::_PG_Party_CtoChat_Invite		( int Sockid , int Size , void* Data )	
{
	PG_Party_CtoChat_Invite*   PG = (PG_Party_CtoChat_Invite*)Data;
	BaseItemObject* Sender = BaseItemObject::GetObjBySockID( Sockid );

	if( Sender == NULL )
		return;

	This->R_Invite( Sender , PG->InviteName.Begin() , PG->ClientInfo );
}
void NetSrv_Party::_PG_Party_CtoChat_Join		( int Sockid , int Size , void* Data )
{
	PG_Party_CtoChat_Join*   PG = (PG_Party_CtoChat_Join*)Data;
	BaseItemObject* Sender = BaseItemObject::GetObjBySockID( Sockid );

	if( Sender == NULL )
		return;

	This->R_Join( Sender , PG->LeaderName.Begin() );
}
void NetSrv_Party::_PG_Party_CtoChat_KickMember	( int Sockid , int Size , void* Data )
{
	PG_Party_CtoChat_KickMember*   PG = (PG_Party_CtoChat_KickMember*)Data;
	BaseItemObject* Sender = BaseItemObject::GetObjBySockID( Sockid );

	if( Sender == NULL )
		return;

	This->R_KickMember( Sender , PG->MemberName.Begin() , PG->PartyID );
}
void NetSrv_Party::_PG_Party_CtoChat_PartyRule	( int Sockid , int Size , void* Data )
{
	PG_Party_CtoChat_PartyRule*   PG = (PG_Party_CtoChat_PartyRule*)Data;
	BaseItemObject* Sender = BaseItemObject::GetObjBySockID( Sockid );

	if( Sender == NULL )
		return;

	This->R_PartyRule( Sender , PG->PartyInfo );
}
void NetSrv_Party::_PG_Party_LtoChat_PartyTreasure		( int Sockid , int Size , void* Data )
{
	PG_Party_LtoChat_PartyTreasure*   PG = (PG_Party_LtoChat_PartyTreasure*)Data;
	This->R_PartyTreasure( PG->PartyID , PG->Item , PG->LootDBID );
}
void NetSrv_Party::_PG_Party_CtoChat_PartyTreasureLoot	( int Sockid , int Size , void* Data )
{
	PG_Party_CtoChat_PartyTreasureLoot*   PG = (PG_Party_CtoChat_PartyTreasureLoot*)Data;
	BaseItemObject* Sender = BaseItemObject::GetObjBySockID( Sockid );

	if( Sender == NULL )
		return;

	This->R_PartyTreasureLoot( Sender , PG->ItemVersion , PG->Type , PG->DBID );
}
//-------------------------------------------------------------------
void NetSrv_Party::SC_Invite		( int SockID , char* Name , int ClientInfo )
{
	PG_Party_ChattoC_Invite	Send;
	Send.LeaderName = Name;
	Send.ClientInfo = ClientInfo;
	SendToCli( SockID , sizeof( Send ) , &Send );
}
void NetSrv_Party::SL_PartyBaseInfo	( PartyBaseInfoStruct& Info )
{
	PG_Party_ChattoL_PartyBaseInfo	Send;
	Send.Info = Info;

	SendToAllLocal( sizeof( Send ) , &Send );
}

void NetSrv_Party::SL_PartyBaseInfo	( int LocalID , PartyBaseInfoStruct& Info )
{
	PG_Party_ChattoL_PartyBaseInfo	Send;
	Send.Info = Info;

	//SendToAllLocal( sizeof( Send ) , &Send );
	SendToLocal( LocalID , sizeof( Send ) , &Send );
}


void NetSrv_Party::SL_AddMember		( int PartyID , PartyMemberInfoStruct& NewMember )
{
	PG_Party_ChattoL_AddMember	Send;
	Send.NewMember = NewMember;
	Send.PartyID = PartyID;

	SendToAllLocal( sizeof( Send ) , &Send );
}
void NetSrv_Party::SL_AddMember		( int ServerLocalID , int PartyID , PartyMemberInfoStruct& NewMember )
{
	PG_Party_ChattoL_AddMember	Send;
	Send.NewMember = NewMember;
	Send.PartyID = PartyID;

	SendToLocal( ServerLocalID , sizeof( Send ) , &Send );
}

void NetSrv_Party::SL_PartyReloginResult( int Sockid ,  int PartyID , int PlayerDBID , bool Result )
{
	PG_Party_ChattoL_PartyReloginResult Send;
	Send.PartyID	= PartyID;
	Send.PlayerDBID = PlayerDBID;
	Send.Result		= Result;
	SendToSrvBySockID( Sockid , sizeof(Send) , &Send );
	//SendToLocal( ZoneID , sizeof(Send) , &Send );
}

void NetSrv_Party::SL_DelMember		( int PartyID , PartyMemberInfoStruct& Member , bool IsOffline )
{
	PG_Party_ChattoL_DelMember	Send;
	Send.Member = Member;
	Send.PartyID = PartyID;
	Send.IsOffline = IsOffline;

	SendToAllLocal( sizeof( Send ) , &Send );
}
void NetSrv_Party::SL_FixMember		( int PartyID , PartyMemberInfoStruct& Info )
{
	PG_Party_ChattoL_FixMember	Send;
	Send.Member = Info;
	Send.PartyID = PartyID;
	SendToAllLocal( sizeof( Send ) , &Send );
}
void NetSrv_Party::SC_PartyBaseInfo	( int SockID , PartyBaseInfoStruct& Info )
{
	PG_Party_ChattoC_PartyBaseInfo	Send;
	Send.Info = Info;
	SendToCli( SockID , sizeof( Send ) , &Send );
}
void NetSrv_Party::SC_AddMember		( int SockID , PartyMemberInfoStruct& Member)
{
	PG_Party_ChattoC_AddMember	Send;
	Send.NewMember = Member;
	SendToCli( SockID , sizeof( Send ) , &Send );
}
void NetSrv_Party::SC_DelMember		( int SockID , PartyMemberInfoStruct& Member )
{
	PG_Party_ChattoC_DelMember	Send;
	Send.Member = Member;
	SendToCli( SockID , sizeof( Send ) , &Send );
}
void NetSrv_Party::SC_FixMember		( int SockID , PartyMemberInfoStruct& Info )
{
	PG_Party_ChattoC_FixMember	Send;
	Send.Member = Info;
	SendToCli( SockID , sizeof( Send ) , &Send );
}

void NetSrv_Party::SC_PartyTreasure		( int PlayerDBID , int ItemVersion , PartyItemShareENUM LootType , ItemFieldStruct& Item , StaticVector< int , _MAX_PARTY_COUNT_  >&  LootDBID  )
{
	PG_Party_ChattoC_PartyTreasure Send;
	Send.Item = Item;
	Send.ItemVersion = ItemVersion;
	//memcpy( Send.LootDBID , LootDBID , sizeof( Send.LootDBID ) );
	Send.LootDBID = LootDBID;
	Send.LootType = LootType;

	BaseItemObject* Obj = BaseItemObject::GetObjByDBID( PlayerDBID );
	if( Obj != NULL )
	{
		SendToCli( Obj->SockID() , sizeof( Send) , &Send );
	}
}
void NetSrv_Party::SC_PartyTreasure		( GroupParty* Party , int ItemVersion , PartyItemShareENUM LootType , ItemFieldStruct& Item , StaticVector< int , _MAX_PARTY_COUNT_  >&  LootDBID )
{
	PG_Party_ChattoC_PartyTreasure Send;
	Send.Item = Item;
	Send.ItemVersion = ItemVersion;
	//memcpy( Send.LootDBID , LootDBID , sizeof( Send.LootDBID ) );
	Send.LootDBID = LootDBID;
	Send.LootType = LootType;

	set< int >* UserList = Party->UserList_SockID();
	set< int >::iterator Iter;

	for( Iter = UserList->begin() ; Iter != UserList->end() ; Iter ++ )
	{
		int SockID = *Iter;
		SendToCli( SockID , sizeof( Send) , &Send );
	}   
}
void NetSrv_Party::SL_GiveItem			( int ZoneID , int DBID , int GItemID , ItemFieldStruct& Item )
{
	PG_Party_ChattoL_GiveItem Send;
	Send.DBID = DBID;
	Send.GItemID = GItemID;
	Send.Item = Item;

	SendToLocal( ZoneID , sizeof(Send) , &Send );
}

void NetSrv_Party::SC_PartyTreasureLoot	( GroupParty* Party , int DBID , int ItemVersion , int LootValue )
{
	PG_Party_ChattoC_PartyTreasureLoot Send;
	Send.DBID = DBID;
	Send.ItemVersion = ItemVersion;
	Send.LootValue = LootValue;

	set< int >* UserList = Party->UserList_SockID();
	set< int >::iterator Iter;

	for( Iter = UserList->begin() ; Iter != UserList->end() ; Iter ++ )
	{
		int SockID = *Iter;
		SendToCli( SockID , sizeof( Send) , &Send );
	}   
}

void NetSrv_Party::SC_GiveItemInfo		( GroupParty* Party , int DBID , ItemFieldStruct& Item , int ItemVersion )
{
	PG_Party_ChattoC_GiveItemInfo Send;
	Send.DBID = DBID;
	Send.Item = Item;
	Send.Version = ItemVersion;

	set< int >* UserList = Party->UserList_SockID();
	set< int >::iterator Iter;

	for( Iter = UserList->begin() ; Iter != UserList->end() ; Iter ++ )
	{
		int SockID = *Iter;
		SendToCli( SockID , sizeof( Send) , &Send );
	}   
}
void NetSrv_Party::SC_SetMemberPosResult	( int SockID , bool Result )
{
	PG_Party_ChattoC_SetMemberPosResult Send;
	Send.Result = Result;
	SendToCli( SockID , sizeof( Send) , &Send );
}