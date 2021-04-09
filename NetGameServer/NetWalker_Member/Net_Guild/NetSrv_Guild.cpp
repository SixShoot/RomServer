#include "../NetWakerGSrvInc.h"
#include "NetSrv_Guild.h"
//-------------------------------------------------------------------
NetSrv_Guild*      NetSrv_Guild::This         = NULL;
//-------------------------------------------------------------------
bool NetSrv_Guild::_Init()
{	
	Global::Net_ServerList->RegServerLogoutEvent( EM_SERVER_TYPE_DATACENTER , _OnDataCenterDisconnect );

    Srv_NetCliReg( PG_Guild_CtoL_CreateGuild			);
	Srv_NetCliReg( PG_Guild_CtoL_NewGuildInvite			);
	Srv_NetCliReg( PG_Guild_CtoL_NewGuildInviteResult	);
	Srv_NetCliReg( PG_Guild_CtoL_LeaveNewGuild			);
	Srv_NetCliReg( PG_Guild_CtoL_DelNewGuild			);

	Srv_NetCliReg( PG_Guild_CtoL_GuildInvite			);
	Srv_NetCliReg( PG_Guild_CtoL_GuildInviteResult		);
	Srv_NetCliReg( PG_Guild_CtoL_LeaveGuild				);
	Srv_NetCliReg( PG_Guild_CtoL_DelGuild				);

	Srv_NetCliReg( PG_Guild_CtoL_GuildInfoRequet				);
	Srv_NetCliReg( PG_Guild_CtoL_GuildMemberListRequet			);
	Srv_NetCliReg( PG_Guild_CtoL_GuildMemberInfoRequet			);
	Srv_NetCliReg( PG_Guild_CtoL_ModifyGuildInfoRequest			);
	Srv_NetCliReg( PG_Guild_CtoL_ModifyGuildMemberInfoRequest	);
	Srv_NetCliReg( PG_Guild_CtoL_GuildWarHistroy				);


	_Net->RegOnSrvPacketFunction( EM_PG_Guild_DtoL_CreateGuildResult		, _PG_Guild_DtoL_CreateGuildResult		);
	_Net->RegOnSrvPacketFunction( EM_PG_Guild_DtoL_AddNewGuildMemberResult	, _PG_Guild_DtoL_AddNewGuildMemberResult);
	_Net->RegOnSrvPacketFunction( EM_PG_Guild_DtoL_LeaveNewGuildResult		, _PG_Guild_DtoL_LeaveNewGuildResult	);
	_Net->RegOnSrvPacketFunction( EM_PG_Guild_DtoL_DelNewGuildResult		, _PG_Guild_DtoL_DelNewGuildResult		);

	_Net->RegOnSrvPacketFunction( EM_PG_Guild_DtoL_AddGuildMemberResult		, _PG_Guild_DtoL_AddGuildMemberResult	);
	_Net->RegOnSrvPacketFunction( EM_PG_Guild_DtoL_LeaveGuildResult			, _PG_Guild_DtoL_LeaveGuildResult		);
	_Net->RegOnSrvPacketFunction( EM_PG_Guild_DtoL_DelGuildResult			, _PG_Guild_DtoL_DelGuildResult			);

	_Net->RegOnSrvPacketFunction( EM_PG_Guild_DtoL_AddGuild					, _PG_Guild_DtoL_AddGuild				);
	_Net->RegOnSrvPacketFunction( EM_PG_Guild_DtoL_DelGuild					, _PG_Guild_DtoL_DelGuild				);
	_Net->RegOnSrvPacketFunction( EM_PG_Guild_DtoL_AddGuildMember			, _PG_Guild_DtoL_AddGuildMember			);
	_Net->RegOnSrvPacketFunction( EM_PG_Guild_DtoL_DelGuildMember			, _PG_Guild_DtoL_DelGuildMember			);
	_Net->RegOnSrvPacketFunction( EM_PG_Guild_DtoL_ModifyGuild				, _PG_Guild_DtoL_ModifyGuild			);
	_Net->RegOnSrvPacketFunction( EM_PG_Guild_DtoL_ModifyGuildMember		, _PG_Guild_DtoL_ModifyGuildMember		);
	_Net->RegOnSrvPacketFunction( EM_PG_Guild_DtoX_ModifySimpleGuild		, _PG_Guild_DtoX_ModifySimpleGuild		);

	//留言板
	Srv_NetCliReg( PG_Guild_CtoL_BoardPostRequest			);
	Srv_NetCliReg( PG_Guild_CtoL_BoardListRequest			);
	Srv_NetCliReg( PG_Guild_CtoL_BoardMessageRequest		);
	Srv_NetCliReg( PG_Guild_CtoL_BoardModifyMessageRequest	);
	Srv_NetCliReg( PG_Guild_CtoL_BoardModifyModeRequest		);

	Srv_NetCliReg( PG_Guild_CtoL_CloseCreateGuild			);
	Srv_NetCliReg( PG_Guild_CtoL_GuildContributionItem		);
	Srv_NetCliReg( PG_Guild_CtoL_GuildContributionOK		);
	Srv_NetCliReg( PG_Guild_CtoL_GuildContributionClose		);

//	Srv_NetCliReg( PG_Guild_CtoL_GuildShopBuy				);
//	Srv_NetCliReg( PG_Guild_CtoL_GuildShopClose				);

	Srv_NetCliReg( PG_Guild_CtoL_AllGuildMemberCountRequest	);


	//公會戰競標
	_Net->RegOnSrvPacketFunction( EM_PG_Guild_DtoL_GuildFightAuctionInfo		, _PG_Guild_DtoL_GuildFightAuctionInfo		);
	Srv_NetCliReg( PG_Guild_CtoL_GuildFightAuctionClose	);
	Srv_NetCliReg( PG_Guild_CtoL_GuildFightAuctionBid	);


	_Net->RegOnSrvPacketFunction( EM_PG_Guild_DtoL_CreateGuildFightRoom		, _PG_Guild_DtoL_CreateGuildFightRoom		);

	Srv_NetCliReg( PG_Guild_CtoL_GuildFightRequestInvite	);
	Srv_NetCliReg( PG_Guild_CtoL_GuildFightInviteRespond	);


	Srv_NetCliReg( PG_Guild_CtoL_DeclareWar	);
	_Net->RegOnSrvPacketFunction( EM_PG_Guild_DtoL_DeclareWarResult			, _PG_Guild_DtoL_DeclareWarResult		);
	_Net->RegOnSrvPacketFunction( EM_PG_Guild_DtoL_WarInfo					, _PG_Guild_DtoL_WarInfo				);

	Srv_NetCliReg( PG_Guild_CtoL_SetGuildFlagInfoRequest	);
	Srv_NetCliReg( PG_Guild_CtoL_ChangeGuildNameRequest	);


	_Net->RegOnSrvPacketFunction( EM_PG_Guild_DtoL_AddGuildResourceResult	, _PG_Guild_DtoL_AddGuildResourceResult		);
	Srv_NetCliReg( PG_Guild_CtoL_GuildUpgradeRequest	);
	Srv_NetCliReg( PG_Guild_CtoL_SelectContributionLog	);

	return true;

}
//-------------------------------------------------------------------
bool NetSrv_Guild::_Release()
{
    return true;
}
//----------------------------------------------------------------------------------------------
void NetSrv_Guild::_PG_Guild_CtoL_GuildWarHistroy( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return;
	PG_Guild_CtoL_GuildWarHistroy* pg = (PG_Guild_CtoL_GuildWarHistroy*)Data;
	This->RC_GuildWarHistroy( Obj  );
}
void NetSrv_Guild::_PG_Guild_CtoL_SelectContributionLog		( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;

	PG_Guild_CtoL_SelectContributionLog* PG = (PG_Guild_CtoL_SelectContributionLog*)Data;
	This->RC_SelectContributionLog( Obj , PG->DayBefore );
}

//公會升級
void NetSrv_Guild::_PG_Guild_CtoL_GuildUpgradeRequest			( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;

	PG_Guild_CtoL_GuildUpgradeRequest* PG = (PG_Guild_CtoL_GuildUpgradeRequest*)Data;
	This->RC_GuildUpgradeRequest( Obj , PG->GuildLv );
}
//資源增減結果
void NetSrv_Guild::_PG_Guild_DtoL_AddGuildResourceResult		( int NetID , int Size , void* Data )
{
	PG_Guild_DtoL_AddGuildResourceResult* PG = (PG_Guild_DtoL_AddGuildResourceResult*)Data;
	This->RD_AddGuildResourceResult( PG->GuildID , PG->PlayerDBID , PG->Resource , (char*)PG->Ret_Runplot.Begin() , PG->Result );
}


void NetSrv_Guild::_PG_Guild_CtoL_ChangeGuildNameRequest		( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;

	PG_Guild_CtoL_ChangeGuildNameRequest* PG = (PG_Guild_CtoL_ChangeGuildNameRequest*)Data;
	This->RC_ChangeGuildNameRequest( Obj , (char*)PG->GuildName.Begin() );
}

void NetSrv_Guild::_PG_Guild_CtoL_SetGuildFlagInfoRequest		( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;

	PG_Guild_CtoL_SetGuildFlagInfoRequest* PG = (PG_Guild_CtoL_SetGuildFlagInfoRequest*)Data;
	This->RC_SetGuildFlagInfoRequest( Obj , PG->Flag );
}

void NetSrv_Guild::_OnDataCenterDisconnect			( EM_SERVER_TYPE ServerType , DWORD ZoneID  )
{
	This->OnDataCenterDisconnect( );
}

void NetSrv_Guild::_PG_Guild_CtoL_DeclareWar			( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;

	PG_Guild_CtoL_DeclareWar* PG = (PG_Guild_CtoL_DeclareWar*)Data;
	This->RC_DeclareWar( Obj , PG->Type , PG->TargetGuildID );
}
void NetSrv_Guild::_PG_Guild_DtoL_DeclareWarResult		( int NetID , int Size , void* Data )
{
	PG_Guild_DtoL_DeclareWarResult* PG = (PG_Guild_DtoL_DeclareWarResult*)Data;
	This->RD_DeclareWarResult( PG->PlayerDBID , PG->GuildID  , PG->Type , PG->TargetGuildID , PG->Result );
}
void NetSrv_Guild::_PG_Guild_DtoL_WarInfo				( int NetID , int Size , void* Data )
{
	PG_Guild_DtoL_WarInfo* PG = (PG_Guild_DtoL_WarInfo*)Data;
	This->RD_WarInfo( PG->IsDeclareWar , PG->GuildID , PG->Time );
}


void NetSrv_Guild::_PG_Guild_CtoL_GuildFightRequestInvite	( int NetID , int Size , void* Data )
{
	PG_Guild_CtoL_GuildFightRequestInvite* PG = (PG_Guild_CtoL_GuildFightRequestInvite*)Data;
	This->RC_GuildFightRequestInvite( PG );
}

void NetSrv_Guild::_PG_Guild_CtoL_GuildFightInviteRespond	( int NetID , int Size , void* Data )
{
	PG_Guild_CtoL_GuildFightInviteRespond* PG = (PG_Guild_CtoL_GuildFightInviteRespond*)Data;
	This->RC_GuildFightInviteRespond( PG );
}

void NetSrv_Guild::_PG_Guild_CtoL_GuildFightAuctionClose	( int NetID , int Size , void* Data )
{
	
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;
	PG_Guild_CtoL_GuildFightAuctionClose* PG = (PG_Guild_CtoL_GuildFightAuctionClose*)Data;

	This->RC_GuildFightAuctionClose( Obj );
	
}
void NetSrv_Guild::_PG_Guild_CtoL_GuildFightAuctionBid		( int NetID , int Size , void* Data )
{
	
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;
	PG_Guild_CtoL_GuildFightAuctionBid* PG = (PG_Guild_CtoL_GuildFightAuctionBid*)Data;

	This->RC_GuildFightAuctionBid( Obj , PG->CrystalID, PG->BidScore );
	
}
void NetSrv_Guild::_PG_Guild_DtoL_GuildFightAuctionInfo		( int NetID , int Size , void* Data )
{
	PG_Guild_DtoL_GuildFightAuctionInfo* PG = (PG_Guild_DtoL_GuildFightAuctionInfo*)Data;

	This->RD_GuildFightAuctionInfo( PG->CrystalID , PG->OwnerGuildID , PG->BitGuildID );
}
void NetSrv_Guild::_PG_Guild_CtoL_AllGuildMemberCountRequest	( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;
	PG_Guild_CtoL_AllGuildMemberCountRequest* PG = (PG_Guild_CtoL_AllGuildMemberCountRequest*)Data;

	This->RC_AllGuildMemberCountRequest( Obj );
}

/*
void NetSrv_Guild::_PG_Guild_CtoL_GuildShopBuy					( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;
	PG_Guild_CtoL_GuildShopBuy* PG = (PG_Guild_CtoL_GuildShopBuy*)Data;

	This->RC_GuildShopBuy( Obj , PG->FlagID );
}
void NetSrv_Guild::_PG_Guild_CtoL_GuildShopClose				( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;
	PG_Guild_CtoL_GuildShopClose* PG = (PG_Guild_CtoL_GuildShopClose*)Data;

	This->RC_GuildShopClose( Obj );
}
*/

void NetSrv_Guild::_PG_Guild_CtoL_CloseCreateGuild			( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;
	PG_Guild_CtoL_CloseCreateGuild* PG = (PG_Guild_CtoL_CloseCreateGuild*)Data;

	This->RC_CloseCreateGuild( Obj  );
}
void NetSrv_Guild::_PG_Guild_CtoL_GuildContributionItem		( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;
	PG_Guild_CtoL_GuildContributionItem* PG = (PG_Guild_CtoL_GuildContributionItem*)Data;

	This->RC_GuildContributionItem( Obj , PG->Pos , PG->Item , PG->IsPut );
}
void NetSrv_Guild::_PG_Guild_CtoL_GuildContributionOK		( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;
	PG_Guild_CtoL_GuildContributionOK* PG = (PG_Guild_CtoL_GuildContributionOK*)Data;

	This->RC_GuildContributionOK( Obj , PG->Money , PG->Bonus_Money );
}
void NetSrv_Guild::_PG_Guild_CtoL_GuildContributionClose	( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;
	PG_Guild_CtoL_GuildContributionClose* PG = (PG_Guild_CtoL_GuildContributionClose*)Data;

	This->RC_GuildContributionClose( Obj );
}

void NetSrv_Guild::_PG_Guild_CtoL_CreateGuild				( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;
	PG_Guild_CtoL_CreateGuild* PG = (PG_Guild_CtoL_CreateGuild*)Data;

	This->RC_CreateGuild( Obj , (char*)PG->GuildName.Begin() );
}

void NetSrv_Guild::_PG_Guild_CtoL_NewGuildInvite			( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;
	PG_Guild_CtoL_NewGuildInvite* PG = (PG_Guild_CtoL_NewGuildInvite*)Data;

	This->RC_NewGuildInvite( Obj , (char*)PG->PlayerName.Begin() );
}

void NetSrv_Guild::_PG_Guild_CtoL_NewGuildInviteResult		( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;
	PG_Guild_CtoL_NewGuildInviteResult* PG = (PG_Guild_CtoL_NewGuildInviteResult*)Data;

	This->RC_NewGuildInviteResult( Obj , PG->GuildID , PG->LeaderDBID , PG->Result );
}

void NetSrv_Guild::_PG_Guild_DtoL_CreateGuildResult			( int NetID , int Size , void* Data )
{
	PG_Guild_DtoL_CreateGuildResult* PG = (PG_Guild_DtoL_CreateGuildResult*)Data;
	This->RD_CreateGuildResult( PG->PlayerDBID , (char*)PG->GuildName.Begin() , PG->GuildID , PG->Result );
}

void NetSrv_Guild::_PG_Guild_DtoL_AddNewGuildMemberResult	( int NetID , int Size , void* Data )
{
	PG_Guild_DtoL_AddNewGuildMemberResult* PG = (PG_Guild_DtoL_AddNewGuildMemberResult*)Data;
	This->RD_AddNewGuildMemberResult( PG->LeaderDBID , (char*)PG->LeaderName.Begin() , PG->GuildID , (char*)PG->NewMember.Begin() , PG->Result );
}

void NetSrv_Guild::_PG_Guild_DtoL_AddGuild			( int NetID , int Size , void* Data )	
{
	PG_Guild_DtoL_AddGuild* PG = (PG_Guild_DtoL_AddGuild*)Data;
	This->RD_AddGuild( PG->Guild );
}
void NetSrv_Guild::_PG_Guild_DtoL_DelGuild			( int NetID , int Size , void* Data )
{
	PG_Guild_DtoL_DelGuild* PG = (PG_Guild_DtoL_DelGuild*)Data;
	This->RD_DelGuild( PG->GuildID );
}
void NetSrv_Guild::_PG_Guild_DtoL_AddGuildMember	( int NetID , int Size , void* Data )	
{
	PG_Guild_DtoL_AddGuildMember* PG = (PG_Guild_DtoL_AddGuildMember*)Data;
	This->RD_AddGuildMember( PG->Member );
}
void NetSrv_Guild::_PG_Guild_DtoL_DelGuildMember	( int NetID , int Size , void* Data )	
{
	PG_Guild_DtoL_DelGuildMember* PG = (PG_Guild_DtoL_DelGuildMember*)Data;
	This->RD_DelGuildMember( PG->GuildID , (char*)PG->MemberName.Begin() );
}
void NetSrv_Guild::_PG_Guild_CtoL_LeaveNewGuild			( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;
	PG_Guild_CtoL_LeaveNewGuild* PG = (PG_Guild_CtoL_LeaveNewGuild*)Data;

	This->RC_LeaveNewGuild( Obj , (char*)PG->PlayerName.Begin() );
}
void NetSrv_Guild::_PG_Guild_CtoL_DelNewGuild			( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;
	PG_Guild_CtoL_DelNewGuild* PG = (PG_Guild_CtoL_DelNewGuild*)Data;

	This->RC_DelNewGuild( Obj );
}
void NetSrv_Guild::_PG_Guild_DtoL_LeaveNewGuildResult		( int NetID , int Size , void* Data )
{
	PG_Guild_DtoL_LeaveNewGuildResult* PG = (PG_Guild_DtoL_LeaveNewGuildResult*)Data;
	This->RD_LeaveNewGuildResult( PG->LeaderDBID , (char*)PG->PlayerName.Begin() , PG->Result );
}
void NetSrv_Guild::_PG_Guild_DtoL_DelNewGuildResult		( int NetID , int Size , void* Data )
{
	PG_Guild_DtoL_DelNewGuildResult* PG = (PG_Guild_DtoL_DelNewGuildResult*)Data;
	This->RD_DelNewGuildResult( PG->LeaderDBID , PG->Result );
}
void NetSrv_Guild::_PG_Guild_CtoL_GuildInvite			( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;
	PG_Guild_CtoL_GuildInvite* PG = (PG_Guild_CtoL_GuildInvite*)Data;

	This->RC_GuildInvite( Obj , (char*)PG->PlayerName.Begin() );
}

void NetSrv_Guild::_PG_Guild_CtoL_GuildInviteResult		( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;
	PG_Guild_CtoL_GuildInviteResult* PG = (PG_Guild_CtoL_GuildInviteResult*)Data;

	This->RC_GuildInviteResult( Obj , PG->GuildID , PG->LeaderDBID , PG->Result );
}


void NetSrv_Guild::_PG_Guild_DtoL_AddGuildMemberResult	( int NetID , int Size , void* Data )
{
	PG_Guild_DtoL_AddGuildMemberResult* PG = (PG_Guild_DtoL_AddGuildMemberResult*)Data;
	This->RD_AddGuildMemberResult( PG->LeaderDBID , (char*)PG->LeaderName.Begin() , PG->GuildID , (char*)PG->NewMember.Begin() , PG->Result );
}

void NetSrv_Guild::_PG_Guild_CtoL_LeaveGuild			( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;
	PG_Guild_CtoL_LeaveGuild* PG = (PG_Guild_CtoL_LeaveGuild*)Data;

	This->RC_LeaveGuild( Obj , (char*)PG->PlayerName.Begin() );
}
void NetSrv_Guild::_PG_Guild_CtoL_DelGuild			( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;
	PG_Guild_CtoL_DelGuild* PG = (PG_Guild_CtoL_DelGuild*)Data;

	This->RC_DelGuild( Obj );
}
void NetSrv_Guild::_PG_Guild_DtoL_LeaveGuildResult		( int NetID , int Size , void* Data )
{
	PG_Guild_DtoL_LeaveGuildResult* PG = (PG_Guild_DtoL_LeaveGuildResult*)Data;
	This->RD_LeaveGuildResult( PG->LeaderDBID , (char*)PG->PlayerName.Begin() , PG->Result );
}
void NetSrv_Guild::_PG_Guild_DtoL_DelGuildResult		( int NetID , int Size , void* Data )
{
	PG_Guild_DtoL_DelGuildResult* PG = (PG_Guild_DtoL_DelGuildResult*)Data;
	This->RD_DelGuildResult( PG->LeaderDBID , PG->Result );
}

void NetSrv_Guild::_PG_Guild_DtoX_ModifySimpleGuild			( int NetID , int Size , void* Data )
{
	PG_Guild_DtoX_ModifySimpleGuild* PG = (PG_Guild_DtoX_ModifySimpleGuild*)Data;
	This->RD_ModifySimpleGuild( PG->SimpleGuildInfo );
}

void NetSrv_Guild::_PG_Guild_DtoL_ModifyGuild					( int NetID , int Size , void* Data )
{
	PG_Guild_DtoL_ModifyGuild* PG = (PG_Guild_DtoL_ModifyGuild*)Data;
	This->RD_ModifyGuild( PG->Guild );
}
void NetSrv_Guild::_PG_Guild_DtoL_ModifyGuildMember				( int NetID , int Size , void* Data )	
{
	PG_Guild_DtoL_ModifyGuildMember* PG = (PG_Guild_DtoL_ModifyGuildMember*)Data;
	This->RD_ModifyGuildMember( PG->Member );
}

void NetSrv_Guild::_PG_Guild_CtoL_GuildInfoRequet				( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;
	PG_Guild_CtoL_GuildInfoRequet* PG = (PG_Guild_CtoL_GuildInfoRequet*)Data;

	This->RC_GuildInfoRequet( Obj , PG->GuildID );
}
void NetSrv_Guild::_PG_Guild_CtoL_GuildMemberListRequet			( int NetID , int Size , void* Data )	
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;
	PG_Guild_CtoL_GuildMemberListRequet* PG = (PG_Guild_CtoL_GuildMemberListRequet*)Data;

	This->RC_GuildMemberListRequet( Obj , PG->GuildID );
}
void NetSrv_Guild::_PG_Guild_CtoL_GuildMemberInfoRequet			( int NetID , int Size , void* Data )	
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;
	PG_Guild_CtoL_GuildMemberInfoRequet* PG = (PG_Guild_CtoL_GuildMemberInfoRequet*)Data;

	This->RC_GuildMemberInfoRequet( Obj , PG->MemberDBID );
}
void NetSrv_Guild::_PG_Guild_CtoL_ModifyGuildInfoRequest		( int NetID , int Size , void* Data )	
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;
	PG_Guild_CtoL_ModifyGuildInfoRequest* PG = (PG_Guild_CtoL_ModifyGuildInfoRequest*)Data;

	This->RC_ModifyGuildInfoRequest( Obj , PG->Guild );

}
void NetSrv_Guild::_PG_Guild_CtoL_ModifyGuildMemberInfoRequest	( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;
	PG_Guild_CtoL_ModifyGuildMemberInfoRequest* PG = (PG_Guild_CtoL_ModifyGuildMemberInfoRequest*)Data;

	This->RC_ModifyGuildMemberInfoRequest( Obj , PG->Member );
}

void NetSrv_Guild::_PG_Guild_CtoL_BoardPostRequest			( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;
	PG_Guild_CtoL_BoardPostRequest* PG = (PG_Guild_CtoL_BoardPostRequest*)Data;

	This->RC_BoardPostRequest( Obj , PG->Message );
}
void NetSrv_Guild::_PG_Guild_CtoL_BoardListRequest			( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;
	PG_Guild_CtoL_BoardListRequest* PG = (PG_Guild_CtoL_BoardListRequest*)Data;

	This->RC_BoardListRequest( Obj , PG->PageID );
}
void NetSrv_Guild::_PG_Guild_CtoL_BoardMessageRequest		( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;
	PG_Guild_CtoL_BoardMessageRequest* PG = (PG_Guild_CtoL_BoardMessageRequest*)Data;

	This->RC_BoardMessageRequest( Obj , PG->GUID );
}
void NetSrv_Guild::_PG_Guild_CtoL_BoardModifyMessageRequest	( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;
	PG_Guild_CtoL_BoardModifyMessageRequest* PG = (PG_Guild_CtoL_BoardModifyMessageRequest*)Data;

	This->RC_BoardModifyMessageRequest( Obj , PG->MessageGUID , (char*)PG->Message.Begin() );
}
void NetSrv_Guild::_PG_Guild_CtoL_BoardModifyModeRequest	( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;
	PG_Guild_CtoL_BoardModifyModeRequest* PG = (PG_Guild_CtoL_BoardModifyModeRequest*)Data;

	This->RC_BoardModifyModeRequest( Obj , PG->MessageGUID , PG->Mode );
}

void NetSrv_Guild::_PG_Guild_DtoL_CreateGuildFightRoom	( int NetID , int Size , void* Data )
{
	//BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
	//if( Obj == NULL )
	//	return ;
	PG_Guild_DtoL_CreateGuildFightRoom* PG = (PG_Guild_DtoL_CreateGuildFightRoom*)Data;

	This->RD_CreateGuildFightRoom( PG->iCrystalID, PG->iDefenderGuildID, PG->iAttackerGuildID );
}
//////////////////////////////////////////////////////////////////////////
void NetSrv_Guild::SD_CreateGuild			( int PlayerDBID , char* GuildName )
{
	PG_Guild_LtoD_CreateGuild Send;
	Send.PlayerDBID = PlayerDBID;
	Send.GuildName = GuildName;
	SendToDBCenter( sizeof( Send ) , &Send );
}
void NetSrv_Guild::SD_AddNewGuildMember		( int GuildID , char* LeaderName , int LeaderDBID , char* NewMemberName , int NewMemberDBID )
{
	PG_Guild_LtoD_AddNewGuildMember Send;
	Send.GuildID = GuildID;
	Send.LeaderDBID = LeaderDBID;
	Send.LeaderName = LeaderName;
	Send.NewMember = NewMemberName;
	Send.NewMemberDBID = NewMemberDBID;

	SendToDBCenter( sizeof( Send ) , &Send );
}

void NetSrv_Guild::SC_CreateGuildResult		( int SenderDBID , int GuildID , char* GuildName , CreateGuildResultENUM Result )
{
	PG_Guild_LtoC_CreateGuildResult Send;
	Send.GuildName = GuildName;
	Send.Result = Result;
	Send.GuildID = GuildID;

	SendToCli_ByDBID( SenderDBID , sizeof( Send ) , &Send );
}
void NetSrv_Guild::SC_NewGuildInvite		( int SockID , int ProxyID , int GuildID , char* LeaderName , int LeaderDBID )
{
	PG_Guild_LtoC_NewGuildInvite Send;
	Send.GuildID = GuildID;
	Send.LeaderName = LeaderName;
	Send.LeaderDBID = LeaderDBID;

	SendToCli_ByProxyID( SockID , ProxyID , sizeof(Send) , &Send );
}
void NetSrv_Guild::SC_NewGuildInviteResult	( int SockID , int ProxyID  , char* PlayerName , NewGuildInviteResultENUM Result )
{
	PG_Guild_LtoC_NewGuildInviteResult Send;
	Send.PlayerName = PlayerName;
	Send.Result		= Result;

	SendToCli_ByProxyID( SockID , ProxyID , sizeof(Send) , &Send );
}
void NetSrv_Guild::SC_JoindNewGuild		( int SockID , int ProxyID , int GuildID , char* LeaderName )
{
	PG_Guild_LtoC_JoindNewGuild Send;
	Send.GuildID = GuildID;
	Send.LeaderName = LeaderName;

	SendToCli_ByProxyID( SockID , ProxyID , sizeof(Send) , &Send );
}

void NetSrv_Guild::SD_LeaveNewGuild( int LeaderDBID , char* PlayerName )
{
	PG_Guild_LtoD_LeaveNewGuild Send;
	Send.LeaderDBID = LeaderDBID;
	Send.PlayerName = PlayerName; 
	SendToDBCenter( sizeof( Send ) , &Send );
}
void NetSrv_Guild::SD_DelNewGuild( int LeaderDBID )
{
	PG_Guild_LtoD_DelNewGuild Send;
	Send.LeaderDBID = LeaderDBID;
	SendToDBCenter( sizeof( Send ) , &Send );
}
void NetSrv_Guild::SC_LeaveNewGuildResult( int SendToDBID , char* KickPlayerName, bool Result )
{
	PG_Guild_LtoC_LeaveNewGuildResult Send;
	Send.Result = Result;
	Send.KickPlayerName = KickPlayerName;
	Global::SendToCli_ByDBID( SendToDBID , sizeof( Send ) , &Send );
}
void NetSrv_Guild::SC_DelNewGuildResult( int SendToDBID , bool Result )
{
	PG_Guild_LtoC_DelNewGuildResult Send;
	Send.Result = Result;
	Global::SendToCli_ByDBID( SendToDBID , sizeof( Send ) , &Send );
}

void NetSrv_Guild::SD_AddGuildMember		( int GuildID , char* MemberName , int MemberDBID , char* NewMemberName , int NewMemberDBID )
{
	PG_Guild_LtoD_AddGuildMember Send;
	Send.GuildID = GuildID;
	Send.MemberDBID = MemberDBID;
	Send.MemberName = MemberName;
	Send.NewMember = NewMemberName;
	Send.NewMemberDBID = NewMemberDBID;

	SendToDBCenter( sizeof( Send ) , &Send );
}

void NetSrv_Guild::SC_GuildInvite		( int SockID , int ProxyID , int GuildID , char* LeaderName , int LeaderDBID )
{
	PG_Guild_LtoC_GuildInvite Send;
	Send.GuildID = GuildID;
	Send.LeaderName = LeaderName;
	Send.LeaderDBID = LeaderDBID;

	SendToCli_ByProxyID( SockID , ProxyID , sizeof(Send) , &Send );
}
void NetSrv_Guild::SC_GuildInviteResult	( int SockID , int ProxyID  , char* PlayerName , GuildInviteResultENUM Result )
{
	PG_Guild_LtoC_GuildInviteResult Send;
	Send.PlayerName = PlayerName;
	Send.Result		= Result;

	SendToCli_ByProxyID( SockID , ProxyID , sizeof(Send) , &Send );
}
void NetSrv_Guild::SC_JoindGuild		( int SockID , int ProxyID , int GuildID , char* LeaderName )
{
	PG_Guild_LtoC_JoindGuild Send;
	Send.GuildID = GuildID;
	Send.LeaderName = LeaderName;

	SendToCli_ByProxyID( SockID , ProxyID , sizeof(Send) , &Send );
}

void NetSrv_Guild::SD_LeaveGuild( int LeaderDBID , char* PlayerName )
{
	PG_Guild_LtoD_LeaveGuild Send;
	Send.LeaderDBID = LeaderDBID;
	Send.PlayerName = PlayerName; 
	SendToDBCenter( sizeof( Send ) , &Send );
}
void NetSrv_Guild::SD_DelGuild( int LeaderDBID )
{
	PG_Guild_LtoD_DelGuild Send;
	Send.LeaderDBID = LeaderDBID;
	SendToDBCenter( sizeof( Send ) , &Send );
}
void NetSrv_Guild::SC_LeaveGuildResult( int SendToDBID , char* KickPlayerName, bool Result )
{
	PG_Guild_LtoC_LeaveGuildResult Send;
	Send.Result = Result;
	Send.KickPlayerName = KickPlayerName;
	Global::SendToCli_ByDBID( SendToDBID , sizeof( Send ) , &Send );
}
void NetSrv_Guild::SC_DelGuildResult( int SendToDBID , bool Result )
{
	PG_Guild_LtoC_DelGuildResult Send;
	Send.Result = Result;
	Global::SendToCli_ByDBID( SendToDBID , sizeof( Send ) , &Send );
}


void NetSrv_Guild::SC_GuildInfo						( int SendToID , GuildBaseStruct& Guild , int MemberCount  )
{
	PG_Guild_LtoC_GuildInfo	 Send;
	Send.Guild = Guild;
	Send.MemberCount = MemberCount;
	Send.Guild.HousesWar.NextWarTime -= RoleDataEx::G_Now;
	if( Send.Guild.HousesWar.NextWarTime < 0 )
		Send.Guild.HousesWar.NextWarTime = 0;
	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void NetSrv_Guild::SC_GuildMemberListCount			( int SendToID , int GuildID , int Count )
{
	PG_Guild_LtoC_GuildMemberListCount	 Send;
	Send.MemberCount = Count;
	Send.GuildID = GuildID;
	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void NetSrv_Guild::SC_GuildMemberBaseInfo			( int SendToID , GuildMemberBaseInfoStruct& Info )
{
	PG_Guild_LtoC_GuildMemberBaseInfo	 Send;
	Send.Info = Info;
	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void NetSrv_Guild::SC_GuildMemberInfo				( int SendToID , GuildMemberStruct&	Member )
{
	PG_Guild_LtoC_GuildMemberInfo	 Send;
	Send.Member = Member;
	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}

void NetSrv_Guild::SD_ModifyGuildInfoRequest		( int PlayerDBID , GuildBaseStruct& Guild )
{
	PG_Guild_LtoD_ModifyGuildInfoRequest Send;
	Send.PlayerDBID = PlayerDBID;			//要求修改者
	Send.Guild = Guild;
	SendToDBCenter( sizeof( Send ) , &Send );
}
void NetSrv_Guild::SD_ModifyGuildMemberInfoRequest	( int PlayerDBID , GuildMemberStruct& Member )
{
	PG_Guild_LtoD_ModifyGuildMemberInfoRequest Send;
	Send.PlayerDBID = PlayerDBID;			//要求修改者
	Send.Member = Member;
	SendToDBCenter( sizeof( Send ) , &Send );
}

//設定為正式公會
void NetSrv_Guild::SD_SetGuildReady				( int GuildID )
{
	PG_Guild_LtoD_SetGuildReady	Send;
	Send.GuildID = GuildID;
	SendToDBCenter( sizeof( Send ) , &Send );
}

void NetSrv_Guild::SC_SetGuildReady				( int SendToID , bool Result )
{
	PG_Guild_LtoC_SetGuildReady	 Send;
	Send.Result = Result;
	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}

//留言板
void NetSrv_Guild::SD_BoardPostRequest			( int PlayerDBID , GuildBoardStruct& Message )
{
	PG_Guild_LtoD_BoardPostRequest	Send;
	Send.Message = Message;
	Send.PlayerDBID = PlayerDBID;
	SendToDBCenter( sizeof( Send ) , &Send );
}
void NetSrv_Guild::SD_BoardListRequest			( int PlayerDBID , int PageID )
{
	PG_Guild_LtoD_BoardListRequest	Send;
	Send.PlayerDBID = PlayerDBID;
	Send.PageID = PageID;
	SendToDBCenter( sizeof( Send ) , &Send );
}
void NetSrv_Guild::SD_BoardMessageRequest		( int PlayerDBID , int MessageGUID )
{
	PG_Guild_LtoD_BoardMessageRequest	Send;
	Send.GUID = MessageGUID;
	Send.PlayerDBID = PlayerDBID;
	SendToDBCenter( sizeof( Send ) , &Send );
}
void NetSrv_Guild::SD_BoardModifyMessageRequest	( int PlayerDBID , int MessageGUID , char* Message )
{
	PG_Guild_LtoD_BoardModifyMessageRequest	Send;
	Send.PlayerDBID = PlayerDBID;
	Send.Message = Message;
	Send.MessageGUID = MessageGUID;
	SendToDBCenter( sizeof( Send ) , &Send );
}
void NetSrv_Guild::SD_BoardModifyModeRequest	( int PlayerDBID , int MessageGUID , GuildBoardModeStruct	Mode )
{
	PG_Guild_LtoD_BoardModifyModeRequest	Send;
	Send.PlayerDBID = PlayerDBID;
	Send.Mode = Mode;
	Send.MessageGUID = MessageGUID;
	SendToDBCenter( sizeof( Send ) , &Send );
}

void NetSrv_Guild::SC_GuildBaseInfo				( int SendToID , const char* GuildName , int GuildID , GuildFlagStruct& Flag , bool IsReady )
{
	PG_Guild_LtoC_GuildBaseInfo Send;
	Send.GuildName = GuildName;
	Send.GuildID = GuildID;
	Send.IsReady = IsReady;
	Send.Flag = Flag;
	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}

void NetSrv_Guild::SC_All_GuildBaseInfo		( const char* GuildName , int GuildID , GuildFlagStruct& Flag , bool IsReady )
{
	PG_Guild_LtoC_GuildBaseInfo Send;
	Send.GuildName = GuildName;
	Send.GuildID = GuildID;
	Send.IsReady = IsReady;
	Send.Flag = Flag;
	Global::SendToAllCli( sizeof( Send ) , &Send );
}

void NetSrv_Guild::SC_All_GuildDestroy		( int GuildID )
{
	PG_Guild_LtoC_GuildDestroy Send;
	Send.GuildID = GuildID;
	Global::SendToAllCli( sizeof( Send ) , &Send );
}

//公會供獻
void NetSrv_Guild::SC_OpenCreateGuild				( int SendToID , int TargetNPCID )
{
	PG_Guild_LtoC_OpenCreateGuild Send;
	Send.TargetNPCID = TargetNPCID;
	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}

void NetSrv_Guild::SC_CloseCreateGuild				( int SendToID )
{
	PG_Guild_LtoC_CloseCreateGuild Send;
	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}

void NetSrv_Guild::SC_OpenGuildContribution			( int SendToID , int TargetNPCID )
{
	PG_Guild_LtoC_OpenGuildContribution Send;
	Send.TargetNPCID = TargetNPCID;
	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void NetSrv_Guild::SC_GuildContributionItemResult	( int SendToID , bool Result )
{
	PG_Guild_LtoC_GuildContributionItemResult Send;
	Send.Result = Result;
	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void NetSrv_Guild::SC_GuildContributionClose		( int SendToID )
{
	PG_Guild_LtoC_GuildContributionClose Send;
	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
/*
void NetSrv_Guild::SD_BuyFunction					( int GuildID , int PlayerDBID , int Score , int Type , int Value )
{
	PG_Guild_LtoD_BuyFunction	Send;
	Send.GuildID = GuildID;
	Send.PlayerDBID = PlayerDBID;
	Send.Score = Score;
	Send.Type = Type;
	Send.Value = Value;
	SendToDBCenter( sizeof( Send ) , &Send );
}
*/

void NetSrv_Guild::SD_AddGuildResource				( int GuildID , int PlayerDBID , GuildResourceStruct& Resource , const char* Ret_Runplot , AddGuildResourceTypeENUM LogType , int LogType_ObjID )
{
	PG_Guild_LtoD_AddGuildResource	Send;
	Send.GuildID = GuildID;
	Send.PlayerDBID = PlayerDBID;
	Send.Resource = Resource;
	Send.Ret_Runplot = Ret_Runplot;
	Send.LogType = LogType;
	Send.LogType_ObjID = LogType_ObjID;

	if( Resource.IsEmpty() != false )
		return;

	SendToDBCenter( sizeof( Send ) , &Send );

}
/*
void NetSrv_Guild::SC_GuildShopOpen				( int SendToID , int TargetNPCID , StaticFlag<256>& Flag , int Score )
{
	PG_Guild_LtoC_GuildShopOpen Send;
	Send.TargetNPCID = TargetNPCID;
	Send.Flag = Flag;
	Send.Score = Score;
	Global::SendToCli_ByGUID( SendToID , sizeof(Send) , &Send );
}
void NetSrv_Guild::SC_GuildShopClose			( int SendToID )
{
	PG_Guild_LtoC_GuildShopClose Send;
	Global::SendToCli_ByGUID( SendToID , sizeof(Send) , &Send );
}
void NetSrv_Guild::SD_GuildShopBuy				( int PlayerDBID , int FlagID )
{
	PG_Guild_LtoD_GuildShopBuy	Send;

	Send.PlayerDBID = PlayerDBID;
	Send.FlagID = FlagID;

	SendToDBCenter( sizeof( Send ) , &Send );
}
*/

void NetSrv_Guild::SC_AllGuildMemberCountResult	( int SendToID , vector< GuildStruct* >& GuildList )
{
	PG_Guild_LtoC_AllGuildMemberCountResult Send;

	int Count = 0;
	for( int i = 0 ; i < (int)GuildList.size() ; i++ )
	{
		if( Count == DEF_MAX_PG_GUILD_MEMBER_COUNT_ )
		{
			Send.Count = Count;
			Global::SendToCli_ByGUID( SendToID , Send.Size() , &Send );
			Count = 0;
			continue;
		}
		Send.MemberList[Count].Count = int( GuildList[i]->Member.size() );
		Send.Count = Count;
	}

	if( Count != 0 )
	{
		Send.Count = Count;
		Global::SendToCli_ByGUID( SendToID , Send.Size() , &Send );
	}
	
}
//----------------------------------------------------------------------------------------------------------
//公會戰競標
//----------------------------------------------------------------------------------------------------------
void NetSrv_Guild::SC_GuildFightAuctionOpen		( int SendToID , int TargetNPCID , int CrystalID , int OwnerGuildID , int BitGuildID , int BitScore )
{
	PG_Guild_LtoC_GuildFightAuctionOpen Send;
	Send.TargetNPCID = TargetNPCID;
	Send.CrystalID	 = CrystalID;			//水晶號碼
	Send.OwnerGuildID= OwnerGuildID;		//目前擁有的公會
	Send.BidGuildID  = BitGuildID;			//目前得標的公會
	Send.BidScore	 = BitScore;			//目前競標的金額

	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
//----------------------------------------------------------------------------------------------------------
void NetSrv_Guild::SC_GuildFightAuctionClose	( int SendToID )
{
	PG_Guild_LtoC_GuildFightAuctionClose Send;
	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
//----------------------------------------------------------------------------------------------------------
void NetSrv_Guild::SD_GuildFightAuctionBid		( int LeaderDBID , int CrystalID , int BitScore )
{
	PG_Guild_LtoD_GuildFightAuctionBid Send;
	Send.LeaderDBID = LeaderDBID;
	Send.CrystalID = CrystalID;
	Send.BidScore = BitScore;

	//Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
	Global::SendToDBCenter( sizeof( Send ) , &Send );
}
//----------------------------------------------------------------------------------------------------------
void NetSrv_Guild::SD_GuildFightInfoRequest		( int iSourceOwnerDBID, int iCrystalID )
{
	PG_Guild_LtoD_GuildFightInfoRequest Send;
	Send.RequestPlayerGUID	= iSourceOwnerDBID;
	Send.CrystalID			= iCrystalID;

	//Global::SendToDBCenter( iSourceOwnerDBID , sizeof( Send ) , &Send );
	Global::SendToDBCenter( sizeof( Send ) , &Send );
}
//----------------------------------------------------------------------------------------------------------
void NetSrv_Guild::SC_GuildFightLeaderInvite( int iGuildLeaderDBID, int iSrvID, int iRoomID, int iCrystalID, int iDefenderGuildID, int iAttackerGuildID )
{
	PG_Guild_LtoC_GuildFightLeaderInvite Send;
	Send.iSrvID				= iSrvID;
	Send.iRoomID			= iRoomID;
	Send.iCrystalID			= iCrystalID;
	Send.iAttackerGuildID	= iAttackerGuildID;
	Send.iDefenderGuildID	= iDefenderGuildID;
	Global::SendToCli_ByDBID( iGuildLeaderDBID, sizeof( Send ) , &Send );
}
//----------------------------------------------------------------------------------------------------------
void NetSrv_Guild::SC_GuildFightInviteNotify( int iMemberDBID, int iCrystalID, int iRoomID )
{
	PG_Guild_LtoC_GuildFightInviteNotify Send;
	Send.iCrystalID	= iCrystalID;
	Send.iRoomID	= iRoomID;
	Global::SendToCli_ByDBID( iMemberDBID, sizeof( Send ) , &Send );
}

void NetSrv_Guild::SC_GuildIDChange			( int SendToID , int GItemID , int GuildID )
{
	PG_Guild_LtoC_GuildIDChange Send;
	Send.GItemID	= GItemID;
	Send.GuildID	= GuildID;
	Global::SendToCli_ByGUID( SendToID, sizeof( Send ) , &Send );
}


void NetSrv_Guild::SD_DeclareWar			( int Type , int PlayerDBID , int TargetGuildID )
{
	PG_Guild_LtoD_DeclareWar Send;
	Send.Type = Type;			
	Send.PlayerDBID = PlayerDBID;
	Send.TargetGuildID = TargetGuildID;

	Global::SendToDBCenter( sizeof( Send ) , &Send );
}
void NetSrv_Guild::SC_DeclareWarResult		( int SendToID , int Type , int TargetGuildID , DeclareWarResultENUM Result )
{
	PG_Guild_LtoC_DeclareWarResult Send;
	Send.Type = Type;				//宣戰類型
	Send.TargetGuildID = TargetGuildID;
	Send.Result = Result;

	Global::SendToCli_ByGUID( SendToID, sizeof( Send ) , &Send );
}
void NetSrv_Guild::SC_WarInfo				( int SendToID , bool IsDeclareWar , int GuildID1 , int GuildID2 , int Time , int BeginTime )
{
	PG_Guild_LtoC_WarInfo Send;
	Send.IsDeclareWar = IsDeclareWar;
	Send.GuildID[0] = GuildID1;
	Send.GuildID[1] = GuildID2;
	Send.Time = Time;
	Send.BeginTime = BeginTime;

	Global::SendToCli_ByGUID( SendToID, sizeof( Send ) , &Send );
}
void NetSrv_Guild::SC_All_WarInfo				( bool IsDeclareWar , int GuildID1 , int GuildID2 , int Time , int BeginTime )
{
	PG_Guild_LtoC_WarInfo Send;
	Send.IsDeclareWar = IsDeclareWar;
	Send.GuildID[0] = GuildID1;
	Send.GuildID[1] = GuildID2;
	Send.Time = Time;
	Send.BeginTime = BeginTime;

	Global::SendToAllCli(sizeof( Send ) , &Send );
}
void NetSrv_Guild::SC_LoginWarInfo					( int SendToID , int GuildID1 , int GuildID2 , int Time , int BeginTime )
{
	PG_Guild_LtoC_LoginWarInfo Send;

	Send.GuildID[0] = GuildID1;
	Send.GuildID[1] = GuildID2;
	Send.Time = Time;
	Send.BeginTime = BeginTime;
	Global::SendToCli_ByGUID( SendToID, sizeof( Send ) , &Send );
}

void NetSrv_Guild::SD_DeclareWarScore				( int GuildID1 , int GuildID2 , int Score , int KillCount )
{
	PG_Guild_LtoD_DeclareWarScore Send;
	Send.GuildID[0] = GuildID1;
	Send.GuildID[1] = GuildID2;
	Send.KillCount = KillCount;
	Send.Score = Score;

	Global::SendToDBCenter( sizeof( Send ) , &Send );
}

void NetSrv_Guild::SD_SetGuildFlagInfoRequest		( int PlayerDBID , GuildFlagStruct&	Flag )
{
	PG_Guild_LtoD_SetGuildFlagInfoRequest Send;
	Send.PlayerDBID = PlayerDBID;
	Send.Flag = Flag;
	Global::SendToDBCenter( sizeof( Send ) , &Send );
}
void NetSrv_Guild::SC_SetGuildFlagInfoResult		( int SendToID , SetGuildFlagInfoResultENUM Result )
{
	PG_Guild_XtoC_SetGuildFlagInfoResult Send;
	Send.Result = Result;
	Global::SendToCli_ByGUID( SendToID, sizeof( Send ) , &Send );
}
void NetSrv_Guild::SD_ChangeGuildNameRequest		( int GuildID , int PlayerDBID , const char* GuildName )
{
	PG_Guild_LtoD_ChangeGuildNameRequest Send;
	Send.GuildID = GuildID;
	Send.PlayerDBID = PlayerDBID;
	Send.GuildName = GuildName;

	Global::SendToDBCenter( sizeof( Send ) , &Send );
}

void NetSrv_Guild::SD_GuildUpgradeRequest			( int GuildID , int GuildLv , int PlayerDBID )
{
	PG_Guild_LtoD_GuildUpgradeRequest Send;
	Send.GuildID = GuildID;
	Send.PlayerDBID = PlayerDBID;
	Send.GuildLv = GuildLv;

	Global::SendToDBCenter( sizeof( Send ) , &Send );
}

void NetSrv_Guild::SD_SelectContributionLog		( int DayBefore , int PlayerDBID )
{
	PG_Guild_LtoD_SelectContributionLog Send;
	Send.PlayerDBID = PlayerDBID;
	Send.DayBefore = DayBefore;

	Global::SendToDBCenter( sizeof( Send ) , &Send );
}

void NetSrv_Guild::SD_SetNeedChangeName			( int GuildID ) 
{
	PG_Guild_LtoD_SetNeedChangeName Send;
	Send.GuildID = GuildID;
	Global::SendToDBCenter( sizeof( Send ) , &Send );
}

void NetSrv_Guild::SD_EndWarResult( int GuildID , GameResultENUM GameResult , int GuildScore , GuildWarBaseInfoStruct& TargetGuild )
{
	PG_Guild_LtoD_GuildWarEndResult Send;

	Send.GuildID = GuildID;
	Send.GameResult = GameResult;
	Send.GuildScore = GuildScore;
	Send.TargetGuild = TargetGuild;
	Global::SendToDBCenter( sizeof(Send) , &Send );

}
//要求公會戰歷史資料
void NetSrv_Guild::SD_GuildWarHistroy( int playerDBID )
{
	PG_Guild_LtoD_GuildWarHistroy send;
	send.PlayerDBID = playerDBID;
	Global::SendToDBCenter( sizeof(send) , &send );
}
