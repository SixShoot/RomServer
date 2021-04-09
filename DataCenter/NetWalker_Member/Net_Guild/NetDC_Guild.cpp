#include "NetDC_Guild.h"
#include "../net_serverlist/Net_ServerList_Child.h"
#include "../../MainProc/GuildManage/GuildManage.h"
//-------------------------------------------------------------------
CNetDC_Guild*	CNetDC_Guild::This = NULL;
//-------------------------------------------------------------------
bool CNetDC_Guild::_Init()
{
	_Net->RegOnSrvPacketFunction( EM_PG_Guild_LtoD_CreateGuild			, _PG_Guild_LtoD_CreateGuild		);
	_Net->RegOnSrvPacketFunction( EM_PG_Guild_LtoD_AddNewGuildMember	, _PG_Guild_LtoD_AddNewGuildMember	);
	_Net->RegOnSrvPacketFunction( EM_PG_Guild_LtoD_LeaveNewGuild		, _PG_Guild_LtoD_LeaveNewGuild		);
	_Net->RegOnSrvPacketFunction( EM_PG_Guild_LtoD_DelNewGuild			, _PG_Guild_LtoD_DelNewGuild		);

	_Net->RegOnSrvPacketFunction( EM_PG_Guild_LtoD_AddGuildMember		, _PG_Guild_LtoD_AddGuildMember		);
	_Net->RegOnSrvPacketFunction( EM_PG_Guild_LtoD_LeaveGuild			, _PG_Guild_LtoD_LeaveGuild			);
	_Net->RegOnSrvPacketFunction( EM_PG_Guild_LtoD_DelGuild				, _PG_Guild_LtoD_DelGuild			);

	_Net->RegOnSrvPacketFunction( EM_PG_Guild_LtoD_ModifyGuildInfoRequest		, _PG_Guild_LtoD_ModifyGuildInfoRequest			);
	_Net->RegOnSrvPacketFunction( EM_PG_Guild_LtoD_ModifyGuildMemberInfoRequest	, _PG_Guild_LtoD_ModifyGuildMemberInfoRequest	);

	_Net->RegOnSrvPacketFunction( EM_PG_Guild_LtoD_SetGuildReady		, _PG_Guild_LtoD_SetGuildReady			);

	
	//Net_ServerList->RegServerLoginEvent( EM_SERVER_TYPE_LOCAL , _OnLocalSrvConnectEvent );
	Net_ServerList->RegServerReadyEvent( EM_SERVER_TYPE_LOCAL , _OnLocalSrvConnectEvent );

	_Net->RegOnSrvPacketFunction( EM_PG_Guild_LtoD_BoardPostRequest			, _PG_Guild_LtoD_BoardPostRequest				);
	_Net->RegOnSrvPacketFunction( EM_PG_Guild_LtoD_BoardListRequest			, _PG_Guild_LtoD_BoardListRequest				);
	_Net->RegOnSrvPacketFunction( EM_PG_Guild_LtoD_BoardMessageRequest		, _PG_Guild_LtoD_BoardMessageRequest			);
	_Net->RegOnSrvPacketFunction( EM_PG_Guild_LtoD_BoardModifyMessageRequest, _PG_Guild_LtoD_BoardModifyMessageRequest		);
	_Net->RegOnSrvPacketFunction( EM_PG_Guild_LtoD_BoardModifyModeRequest	, _PG_Guild_LtoD_BoardModifyModeRequest			);

//	_Net->RegOnSrvPacketFunction( EM_PG_Guild_LtoD_BuyFunction				, _PG_Guild_LtoD_BuyFunction		);
	_Net->RegOnSrvPacketFunction( EM_PG_Guild_LtoD_AddGuildResource			, _PG_Guild_LtoD_AddGuildResource		);

//	_Net->RegOnSrvPacketFunction( EM_PG_Guild_LtoD_GuildShopBuy				, _PG_Guild_LtoD_GuildShopBuy		);


	_Net->RegOnSrvPacketFunction( EM_PG_Guild_LtoD_GuildFightInfoRequest	, _PG_Guild_LtoD_GuildFightInfoRequest		);
	_Net->RegOnSrvPacketFunction( EM_PG_Guild_LtoD_GuildFightAuctionBid		, _PG_Guild_LtoD_GuildFightAuctionBid		);

	_Net->RegOnSrvPacketFunction( EM_PG_Guild_LtoD_DeclareWar				, _PG_Guild_LtoD_DeclareWar					);

	_Net->RegOnSrvPacketFunction( EM_PG_Guild_LtoD_DeclareWarScore			, _PG_Guild_LtoD_DeclareWarScore			);
	_Net->RegOnSrvPacketFunction( EM_PG_Guild_LtoD_SetGuildFlagInfoRequest	, _PG_Guild_LtoD_SetGuildFlagInfoRequest	);

	_Net->RegOnSrvPacketFunction( EM_PG_Guild_LtoD_ChangeGuildNameRequest	, _PG_Guild_LtoD_ChangeGuildNameRequest		);
	_Net->RegOnSrvPacketFunction( EM_PG_Guild_LtoD_GuildUpgradeRequest		, _PG_Guild_LtoD_GuildUpgradeRequest		);
	_Net->RegOnSrvPacketFunction( EM_PG_Guild_LtoD_SelectContributionLog	, _PG_Guild_LtoD_SelectContributionLog		);

	_Net->RegOnSrvPacketFunction( EM_PG_Guild_LtoD_SetNeedChangeName		, _PG_Guild_LtoD_SetNeedChangeName			);

	_Net->RegOnSrvPacketFunction( EM_PG_Guild_LtoD_GuildWarEndResult		, _PG_Guild_LtoD_GuildWarEndResult			);
	_Net->RegOnSrvPacketFunction( EM_PG_Guild_LtoD_GuildWarHistroy			, _PG_Guild_LtoD_GuildWarHistroy			);

	return false;
}
//-------------------------------------------------------------------
bool CNetDC_Guild::_Release()
{
	return true;
}
//-------------------------------------------------------------------
void CNetDC_Guild::_PG_Guild_LtoD_GuildWarHistroy( int ServerID , int Size , void* Data )
{
	PG_Guild_LtoD_GuildWarHistroy* pg = (PG_Guild_LtoD_GuildWarHistroy*)Data;
	This->RL_GuildWarHistroy( ServerID , pg->PlayerDBID );
}
void  CNetDC_Guild::_PG_Guild_LtoD_GuildWarEndResult				( int ServerID , int Size , void* Data )
{
	PG_Guild_LtoD_GuildWarEndResult* PG = ( PG_Guild_LtoD_GuildWarEndResult* )Data;
	This->RL_GuildWarEndResult( PG->GuildID , PG->GameResult , PG->GuildScore , PG->TargetGuild );
}
void  CNetDC_Guild::_PG_Guild_LtoD_SetNeedChangeName			( int ServerID , int Size , void* Data )
{
	PG_Guild_LtoD_SetNeedChangeName* PG = ( PG_Guild_LtoD_SetNeedChangeName* )Data;
	This->RL_SetNeedChangeName( PG->GuildID );
}
void  CNetDC_Guild::_PG_Guild_LtoD_SelectContributionLog		( int ServerID , int Size , void* Data )
{
	PG_Guild_LtoD_SelectContributionLog* PG = ( PG_Guild_LtoD_SelectContributionLog* )Data;
	This->RL_SelectContributionLog( PG->PlayerDBID , PG->DayBefore );
}
void CNetDC_Guild::_PG_Guild_LtoD_GuildUpgradeRequest			( int ServerID , int Size , void* Data )
{
	PG_Guild_LtoD_GuildUpgradeRequest* PG = ( PG_Guild_LtoD_GuildUpgradeRequest* )Data;
	This->RL_GuildUpgradeRequest( PG->GuildID , PG->PlayerDBID , PG->GuildLv );
}

void CNetDC_Guild::_PG_Guild_LtoD_ChangeGuildNameRequest		( int ServerID , int Size , void* Data )
{
	PG_Guild_LtoD_ChangeGuildNameRequest* PG = ( PG_Guild_LtoD_ChangeGuildNameRequest* )Data;
	This->RL_ChangeGuildNameRequest( PG->GuildID , PG->PlayerDBID , (char*)PG->GuildName.Begin() );
}
void CNetDC_Guild::_PG_Guild_LtoD_SetGuildFlagInfoRequest		( int ServerID , int Size , void* Data )
{
	PG_Guild_LtoD_SetGuildFlagInfoRequest* PG = ( PG_Guild_LtoD_SetGuildFlagInfoRequest* )Data;
	This->RL_SetGuildFlagInfoRequest( PG->PlayerDBID , PG->Flag );
}
void CNetDC_Guild::_PG_Guild_LtoD_DeclareWarScore				( int ServerID , int Size , void* Data )
{
	PG_Guild_LtoD_DeclareWarScore* PG = ( PG_Guild_LtoD_DeclareWarScore* )Data;
	This->RL_DeclareWarScore( PG->GuildID , PG->Score , PG->KillCount );
}


void CNetDC_Guild::_PG_Guild_LtoD_DeclareWar					( int ServerID , int Size , void* Data )
{
	PG_Guild_LtoD_DeclareWar* PG = ( PG_Guild_LtoD_DeclareWar* )Data;
	This->RL_DeclareWar( ServerID , PG->Type , PG->PlayerDBID , PG->TargetGuildID );
}

void CNetDC_Guild::_PG_Guild_LtoD_GuildFightAuctionBid			( int ServerID , int Size , void* Data )
{
	PG_Guild_LtoD_GuildFightAuctionBid* PG = ( PG_Guild_LtoD_GuildFightAuctionBid* )Data;
	This->RL_GuildFightAuctionBid( PG->LeaderDBID , PG->CrystalID , PG->BidScore );
}
/*
void CNetDC_Guild::_PG_Guild_LtoD_BuyFunction		( int ServerID , int Size , void* Data )
{
	PG_Guild_LtoD_BuyFunction* PG = ( PG_Guild_LtoD_BuyFunction* )Data;
	This->RL_BuyFunction( ServerID , PG->GuildID , PG->PlayerDBID , PG->Score , PG->Type , PG->Value  );
}
*/
/*
void CNetDC_Guild::_PG_Guild_LtoD_GuildShopBuy		( int ServerID , int Size , void* Data )
{
	PG_Guild_LtoD_GuildShopBuy* PG = ( PG_Guild_LtoD_GuildShopBuy* )Data;
	This->RL_GuildShopBuy( PG->PlayerDBID , PG->FlagID );
}
*/
void CNetDC_Guild::_PG_Guild_LtoD_AddGuildResource		( int ServerID , int Size , void* Data )
{
	PG_Guild_LtoD_AddGuildResource* PG = ( PG_Guild_LtoD_AddGuildResource* )Data;
	This->RL_AddGuildResource(ServerID , PG->GuildID , PG->PlayerDBID , PG->Resource , PG->Ret_Runplot.Begin() , PG->LogType , PG->LogType_ObjID );
}
void CNetDC_Guild::_PG_Guild_LtoD_CreateGuild		( int ServerID , int Size , void* Data )
{
	PG_Guild_LtoD_CreateGuild* PG = ( PG_Guild_LtoD_CreateGuild* )Data;
	This->RL_CreateGuild( ServerID , PG->PlayerDBID , (char*)PG->GuildName.Begin() );
}
//-------------------------------------------------------------------
void CNetDC_Guild::_PG_Guild_LtoD_AddNewGuildMember( int ServerID , int Size , void* Data )
{
	PG_Guild_LtoD_AddNewGuildMember* PG = ( PG_Guild_LtoD_AddNewGuildMember* )Data;
	This->RL_AddNewGuildMember( ServerID , PG->GuildID , (char*)PG->LeaderName.Begin() , PG->LeaderDBID , (char*)PG->NewMember.Begin() , PG->NewMemberDBID );
}
void CNetDC_Guild::_PG_Guild_LtoD_LeaveNewGuild	( int ServerID , int Size , void* Data )
{
	PG_Guild_LtoD_LeaveNewGuild* PG = ( PG_Guild_LtoD_LeaveNewGuild* )Data;
	This->RL_LeaveNewGuild( ServerID , PG->LeaderDBID , (char*)PG->PlayerName.Begin() );
}
void CNetDC_Guild::_PG_Guild_LtoD_DelNewGuild	( int ServerID , int Size , void* Data )
{
	PG_Guild_LtoD_DelNewGuild* PG = ( PG_Guild_LtoD_DelNewGuild* )Data;
	This->RL_DelNewGuild( ServerID , PG->LeaderDBID );
}
//-------------------------------------------------------------------
void CNetDC_Guild::_PG_Guild_LtoD_AddGuildMember( int ServerID , int Size , void* Data )
{
	PG_Guild_LtoD_AddGuildMember* PG = ( PG_Guild_LtoD_AddGuildMember* )Data;
	This->RL_AddGuildMember( ServerID , PG->GuildID , (char*)PG->MemberName.Begin() , PG->MemberDBID , (char*)PG->NewMember.Begin() , PG->NewMemberDBID );
}
void CNetDC_Guild::_PG_Guild_LtoD_LeaveGuild	( int ServerID , int Size , void* Data )
{
	PG_Guild_LtoD_LeaveGuild* PG = ( PG_Guild_LtoD_LeaveGuild* )Data;
	This->RL_LeaveGuild( ServerID , PG->LeaderDBID , (char*)PG->PlayerName.Begin() );
}
void CNetDC_Guild::_PG_Guild_LtoD_DelGuild	( int ServerID , int Size , void* Data )
{
	PG_Guild_LtoD_DelGuild* PG = ( PG_Guild_LtoD_DelGuild* )Data;
	This->RL_DelGuild( ServerID , PG->LeaderDBID );
}
//-------------------------------------------------------------------
void CNetDC_Guild::_PG_Guild_LtoD_ModifyGuildInfoRequest		( int ServerID , int Size , void* Data )
{
	PG_Guild_LtoD_ModifyGuildInfoRequest* PG = ( PG_Guild_LtoD_ModifyGuildInfoRequest* )Data;
	This->RL_ModifyGuildInfoRequest( ServerID , PG->PlayerDBID , PG->Guild );
}
void CNetDC_Guild::_PG_Guild_LtoD_ModifyGuildMemberInfoRequest	( int ServerID , int Size , void* Data )
{
	PG_Guild_LtoD_ModifyGuildMemberInfoRequest* PG = ( PG_Guild_LtoD_ModifyGuildMemberInfoRequest* )Data;
	This->RL_ModifyGuildMemberInfoRequest( ServerID , PG->PlayerDBID , PG->Member );
}
//-------------------------------------------------------------------
void CNetDC_Guild::_PG_Guild_LtoD_GuildFightInfoRequest		( int ServerID , int Size , void* Data )
{
	PG_Guild_LtoD_GuildFightInfoRequest* PG = ( PG_Guild_LtoD_GuildFightInfoRequest* ) Data;
	This->RL_GuildFightInfoRequest( PG->RequestPlayerGUID, PG->CrystalID );
}
//-------------------------------------------------------------------
void CNetDC_Guild::_OnLocalSrvConnectEvent ( EM_SERVER_TYPE ServerType, DWORD SrvID )
{
	This->OnLocalSrvConnect( SrvID );
}
void CNetDC_Guild::_PG_Guild_LtoD_SetGuildReady	( int ServerID , int Size , void* Data )
{
	PG_Guild_LtoD_SetGuildReady* PG = ( PG_Guild_LtoD_SetGuildReady* )Data;
	This->RL_SetGuildReady( PG->GuildID );
}

void CNetDC_Guild::_PG_Guild_LtoD_BoardPostRequest			( int ServerID , int Size , void* Data )
{
	PG_Guild_LtoD_BoardPostRequest* PG = ( PG_Guild_LtoD_BoardPostRequest* )Data;
	This->RL_BoardPostRequest( PG->PlayerDBID , PG->Message );
}
void CNetDC_Guild::_PG_Guild_LtoD_BoardListRequest			( int ServerID , int Size , void* Data )
{
	PG_Guild_LtoD_BoardListRequest* PG = ( PG_Guild_LtoD_BoardListRequest* )Data;
	This->RL_BoardListRequest( PG->PlayerDBID , PG->PageID );
}
void CNetDC_Guild::_PG_Guild_LtoD_BoardMessageRequest		( int ServerID , int Size , void* Data )
{
	PG_Guild_LtoD_BoardMessageRequest* PG = ( PG_Guild_LtoD_BoardMessageRequest* )Data;
	This->RL_BoardMessageRequest( PG->PlayerDBID , PG->GUID );
}
void CNetDC_Guild::_PG_Guild_LtoD_BoardModifyMessageRequest	( int ServerID , int Size , void* Data )
{
	PG_Guild_LtoD_BoardModifyMessageRequest* PG = ( PG_Guild_LtoD_BoardModifyMessageRequest* )Data;
	This->RL_BoardModifyMessageRequest( PG->PlayerDBID , PG->MessageGUID , (char*)PG->Message.Begin() );
}
void CNetDC_Guild::_PG_Guild_LtoD_BoardModifyModeRequest	( int ServerID , int Size , void* Data )
{
	PG_Guild_LtoD_BoardModifyModeRequest* PG = ( PG_Guild_LtoD_BoardModifyModeRequest* )Data;
	This->RL_BoardModifyModeRequest( PG->PlayerDBID , PG->MessageGUID , PG->Mode );
}
//////////////////////////////////////////////////////////////////////////
void CNetDC_Guild::SL_CreateGuildResult			( int SrvSockID , int PlayerDBID , char* GuildName , int GuildID , CreateGuildResultENUM Result )
{
	PG_Guild_DtoL_CreateGuildResult Send;
	Send.GuildName = GuildName;
	Send.PlayerDBID = PlayerDBID;
	Send.GuildID = GuildID;
	Send.Result = Result;

	Global::SendToSrvBySockID( SrvSockID , sizeof( Send ) , &Send );
}
void CNetDC_Guild::SL_AddNewGuildMemberResult	( int SrvSockID , char* LeaderName , int LeaderDBID , int GuildID , char* NewMember , bool Result )
{
	PG_Guild_DtoL_AddNewGuildMemberResult Send;
	Send.LeaderName = LeaderName;		//邀請人名
	Send.GuildID	= GuildID;			//公會ID
	Send.NewMember	= NewMember;		//新加入者人名
	Send.LeaderDBID = LeaderDBID;
	Send.Result		= Result;

	Global::SendToSrvBySockID( SrvSockID , sizeof( Send ) , &Send );
}

void CNetDC_Guild::SL_All_AddGuild( GuildBaseStruct& Guild )
{
	PG_Guild_DtoL_AddGuild Send;
	Send.Guild = Guild;
	Global::SendToAllLocal( sizeof( Send ) , &Send );
}
void CNetDC_Guild::SL_AddGuild( int LocalID ,  GuildBaseStruct& Guild )
{
	PG_Guild_DtoL_AddGuild Send;
	Send.Guild = Guild;
	Global::SendToLocal( LocalID , sizeof( Send ) , &Send );
}
void CNetDC_Guild::SL_All_DelGuild( int GuildID )
{
	PG_Guild_DtoL_DelGuild Send;
	Send.GuildID = GuildID;
	Global::SendToAllLocal( sizeof( Send ) , &Send );
}
void CNetDC_Guild::SL_All_AddGuildMember( GuildMemberStruct& Member )
{
	PG_Guild_DtoL_AddGuildMember Send;
	Send.Member = Member;
	Global::SendToAllLocal( sizeof( Send ) , &Send );
}
void CNetDC_Guild::SL_AddGuildMember( int LocalID , GuildMemberStruct& Member )
{
	PG_Guild_DtoL_AddGuildMember Send;
	Send.Member = Member;
	Global::SendToLocal( LocalID , sizeof( Send ) , &Send );
}
void CNetDC_Guild::SL_All_DelGuildMember( int Guild , char* MemberName )
{
	PG_Guild_DtoL_DelGuildMember Send;
	Send.GuildID = Guild;
	Send.MemberName = MemberName;
	Global::SendToAllLocal( sizeof( Send ) , &Send );
}
void CNetDC_Guild::SL_LeaveNewGuildResult	( int SrvSockID , int LeaderDBID , char* PlayerName , bool Result )
{
	PG_Guild_DtoL_LeaveNewGuildResult Send;
	Send.LeaderDBID = LeaderDBID;
	Send.PlayerName = PlayerName;
	Send.Result = Result;
	Global::SendToSrvBySockID( SrvSockID ,  sizeof( Send ) , &Send );
}
void CNetDC_Guild::SL_DelNewGuildResult		( int SrvSockID , int LeaderDBID , bool Result )
{
	PG_Guild_DtoL_DelNewGuildResult	 Send;
	Send.LeaderDBID = LeaderDBID;
	Send.Result = Result;
	Global::SendToSrvBySockID( SrvSockID ,  sizeof( Send ) , &Send );

}

void CNetDC_Guild::SL_AddGuildMemberResult	( int SrvSockID , char* LeaderName , int LeaderDBID , int GuildID , char* NewMember , bool Result )
{
	PG_Guild_DtoL_AddGuildMemberResult Send;
	Send.LeaderName = LeaderName;		//邀請人名
	Send.GuildID	= GuildID;			//公會ID
	Send.NewMember	= NewMember;		//新加入者人名
	Send.LeaderDBID = LeaderDBID;
	Send.Result		= Result;

	Global::SendToSrvBySockID( SrvSockID , sizeof( Send ) , &Send );
}
void CNetDC_Guild::SL_LeaveGuildResult	( int SrvSockID , int LeaderDBID , char* PlayerName , bool Result )
{
	PG_Guild_DtoL_LeaveGuildResult Send;
	Send.LeaderDBID = LeaderDBID;
	Send.PlayerName = PlayerName;
	Send.Result = Result;
	Global::SendToSrvBySockID( SrvSockID ,  sizeof( Send ) , &Send );
}
void CNetDC_Guild::SL_DelGuildResult		( int SrvSockID , int LeaderDBID , bool Result )
{
	PG_Guild_DtoL_DelGuildResult	 Send;
	Send.LeaderDBID = LeaderDBID;
	Send.Result = Result;
	Global::SendToSrvBySockID( SrvSockID ,  sizeof( Send ) , &Send );

}

void CNetDC_Guild::SC_ALLPlayer_GuildCreate	( int GuildID )
{
	PG_Guild_DtoC_GuildCreate Send;
	Send.GuildID = GuildID;
	Global::SendToAllPlayer( sizeof( Send ) , &Send );
}
/*
void CNetDC_Guild::SC_ALLMember_GuildCreate	( int GuildID )
{
	PG_Guild_DtoC_GuildCreate Send;
	GuildStruct* Guild = GuildManageClass::This()->GetGuildInfo( GuildID );
	if( GuildID == NULL )
		return;

	for( int i = 0 ; i < (int)Guild->Member.size() ; i++ )
	{
		GuildMemberStruct* Member = Guild->Member[i];
		if( Member->IsOnline == false )
			return;
		Global::SendToCli_ByDBID( Member->MemberDBID ,  sizeof( Send ) , &Send );

	}
}
*/

void CNetDC_Guild::SC_ALLMember_PlayerOnline	( int PlayerDBID , bool IsOnline )
{
	PG_Guild_DtoL_GuildPlayerOnline Send;
	GuildMemberStruct* Member = GuildManageClass::This()->GetMember( PlayerDBID );
	if( Member == NULL )
		return;

	GuildStruct* Guild = Member->Guild;
	if( Guild == NULL )
		return;

	Send.PlayerDBID = PlayerDBID;
	Send.IsOnline = IsOnline;

	for( int i = 0 ; i < (int)Guild->Member.size() ; i++ )
	{
		GuildMemberStruct* Member = Guild->Member[i];
		if( Member->IsOnline == false )
			continue;
		Global::SendToCli_ByDBID( Member->MemberDBID ,  sizeof( Send ) , &Send );
	}
}

void CNetDC_Guild::SC_ALLMember_PrepareWar	( int SendToGuildID , int GuildID ,	int Time )
{
	PG_Guild_DtoC_PrepareWar Send;

	GuildStruct* Guild = GuildManageClass::This()->GetGuildInfo( SendToGuildID );
	if( Guild == NULL )
		return;

	Send.GuildID = GuildID;
	Send.Time = Time;

	for( int i = 0 ; i < (int)Guild->Member.size() ; i++ )
	{
		GuildMemberStruct* Member = Guild->Member[i];
		if( Member->IsOnline == false )
			continue;
		Global::SendToCli_ByDBID( Member->MemberDBID ,  sizeof( Send ) , &Send );
	}
}



void CNetDC_Guild::SL_All_ModifyGuild		( GuildBaseStruct& Guild )
{
	PG_Guild_DtoL_ModifyGuild Send;
	Send.Guild = Guild;

	Global::SendToAllLocal( sizeof( Send ) , &Send );
}
void CNetDC_Guild::SL_All_ModifyGuildMember	( GuildMemberStruct& Member )
{
	PG_Guild_DtoL_ModifyGuildMember Send;
	Send.Member = Member;

	Global::SendToAllLocal( sizeof( Send ) , &Send );
}

void CNetDC_Guild::SL_All_ModifySimpleGuild( GuildBaseStruct& GuildBase )
{
	PG_Guild_DtoX_ModifySimpleGuild Send;
	Send.SimpleGuildInfo.GuildID = GuildBase.GuildID;
	Send.SimpleGuildInfo.Resource = GuildBase.Resource;
	/*
	Send.SimpleGuildInfo.WarMyScore = GuildBase.WarMyScore;
	Send.SimpleGuildInfo.WarMyKillCount = GuildBase.WarMyKillCount;
	Send.SimpleGuildInfo.WarEnemyScore = GuildBase.WarEnemyScore;
	Send.SimpleGuildInfo.WarEnemyKillCount = GuildBase.WarEnemyKillCount;
	*/
	Global::SendToAllLocal( sizeof( Send ) , &Send );
}
void CNetDC_Guild::SC_AllMember_GuildBaseInfoUpdate( GuildBaseStruct& GuildBase )
{
	PG_Guild_DtoC_GuildBaseInfoUpdate Send;

	GuildStruct* Guild = GuildManageClass::This()->GetGuildInfo( GuildBase.GuildID	);
	if( Guild == NULL )
		return;

	for( int i = 0 ; i < (int)Guild->Member.size() ; i++ )
	{
		GuildMemberStruct* Member = Guild->Member[i];
		if( Member->IsOnline == false )
			continue;
		Global::SendToCli_ByDBID( Member->MemberDBID ,  sizeof( Send ) , &Send );
	}
}

void CNetDC_Guild::SC_AllMember_ModifySimpleGuild( GuildBaseStruct& GuildBase )
{
	PG_Guild_DtoX_ModifySimpleGuild Send;
	Send.SimpleGuildInfo.GuildID = GuildBase.GuildID;
	Send.SimpleGuildInfo.Resource = GuildBase.Resource;
	/*
	Send.SimpleGuildInfo.WarMyScore = GuildBase.WarMyScore;
	Send.SimpleGuildInfo.WarMyKillCount = GuildBase.WarMyKillCount;
	Send.SimpleGuildInfo.WarEnemyScore = GuildBase.WarEnemyScore;
	Send.SimpleGuildInfo.WarEnemyKillCount = GuildBase.WarEnemyKillCount;
*/
	GuildStruct* Guild = GuildManageClass::This()->GetGuildInfo( GuildBase.GuildID	);
	if( Guild == NULL )
		return;

	for( int i = 0 ; i < (int)Guild->Member.size() ; i++ )
	{
		GuildMemberStruct* Member = Guild->Member[i];
		if( Member->IsOnline == false )
			continue;
		Global::SendToCli_ByDBID( Member->MemberDBID ,  sizeof( Send ) , &Send );
	}
}
//--------------------------------------------------------------------------------------------------------------------
//留言板
//--------------------------------------------------------------------------------------------------------------------
void CNetDC_Guild::SC_BoardPostResult			( int PlayerDBID , bool Result )
{
	PG_Guild_DtoC_BoardPostResult Send;
	Send.Result = Result;

	Global::SendToCli_ByDBID( PlayerDBID , sizeof( Send ) , &Send );
}
void CNetDC_Guild::SC_BoardListCount			( int PlayerDBID , int MaxCount , int Count , int PageID )
{
	PG_Guild_DtoC_BoardListCount Send;
	Send.Count = Count;
	Send.MaxCount = MaxCount;
	Send.PageID = PageID;

	Global::SendToCli_ByDBID( PlayerDBID , sizeof( Send ) , &Send );
}
void CNetDC_Guild::SC_BoardListData				( int PlayerDBID , int Index , GuildBoardStruct& Message )
{
	PG_Guild_DtoC_BoardListData Send;

	Send.Index	= Index;					//第幾筆資料
	Send.GUID	= Message.GUID;				//序號
	Send.PlayerName = Message.PlayerName;	//發表人
	Send.Type	= Message.Type;				//狀態
	Send.Time	= Message.Time;				//發表時間
	Send.Title	= Message.Title;			//標題

	Global::SendToCli_ByDBID( PlayerDBID , sizeof( Send ) , &Send );
}
void CNetDC_Guild::SC_BoardMessageResult		( int PlayerDBID , GuildBoardStruct& Message )
{
	PG_Guild_DtoC_BoardMessageResult Send;
	Send.Message = Message;
	Global::SendToCli_ByDBID( PlayerDBID , sizeof( Send ) , &Send );
}
void CNetDC_Guild::SC_BoardModifyMessageResult	( int PlayerDBID , bool Result )
{
	PG_Guild_DtoC_BoardModifyMessageResult Send;
	Send.Result = Result;

	Global::SendToCli_ByDBID( PlayerDBID , sizeof( Send ) , &Send );
}
void CNetDC_Guild::SC_BoardModifyModeResult		( int PlayerDBID , bool Result )
{
	PG_Guild_DtoC_BoardModifyModeResult Send;
	Send.Result = Result;

	Global::SendToCli_ByDBID( PlayerDBID , sizeof( Send ) , &Send );
}
//--------------------------------------------------------------------------------------------------------------------
//通知玩家新增移除公會成員
//--------------------------------------------------------------------------------------------------------------------
void CNetDC_Guild::SC_JoinGuildMember				( int PlayerDBID , GuildMemberStruct& Member )
{
	PG_Guild_DtoC_JoinGuildMember Send;
	Send.Member = Member;

	Global::SendToCli_ByDBID( PlayerDBID , sizeof( Send ) , &Send );
}
void CNetDC_Guild::SC_LeaveGuildMember				( int PlayerDBID , int LeaderDBID , int MemberDBID )
{
	PG_Guild_DtoC_LeaveGuildMember	 Send;
	Send.MemberDBID = MemberDBID;
	Send.LeaderDBID = LeaderDBID;

	Global::SendToCli_ByDBID( PlayerDBID , sizeof( Send ) , &Send );
}

/*
//通知Client 購買結果
void CNetDC_Guild::SC_GuildShopBuyResult		( int PlayerDBID , int FlagID , bool Result )
{
	PG_Guild_DtoC_GuildShopBuyResult	 Send;
	Send.FlagID = FlagID;
	Send.Result = Result;

	Global::SendToCli_ByDBID( PlayerDBID , sizeof( Send ) , &Send );
}
*/
//--------------------------------------------------------------------------------------------------------------------
//公會戰競標
//--------------------------------------------------------------------------------------------------------------------
void CNetDC_Guild::SC_GuildFightAuctionBidResult( int PlayerDBID , int CrystalID , int OwnerGuildID , int BitGuildID, int iHighestAmount, int Result )
{
	PG_Guild_DtoC_GuildFightAuctionBidResult	 Send;
	Send.CrystalID = CrystalID;				//水晶號碼
	Send.OwnerGuildID = OwnerGuildID;		//目前擁有的公會
	Send.BidGuildID = BitGuildID;			//目前得標的公會
	Send.iResult = Result;

	Global::SendToCli_ByDBID( PlayerDBID , sizeof( Send ) , &Send );
}
void CNetDC_Guild::SC_GuildFightAuctionBidNewOne( int PlayerDBID , int CrystalID , int OwnerGuildID , int BitGuildID, int iHighestAmount, int iRemainTime )
{
	PG_Guild_DtoC_GuildFightAuctionBidNewOne	 Send;

	Send.CrystalID		= CrystalID;		//水晶號碼
	Send.OwnerGuildID	= OwnerGuildID;		//目前擁有的公會
	Send.BidGuildID		= BitGuildID;		//目前得標的公會
	Send.iHighestAmount	= iHighestAmount;
	Send.iRemainTime	= iRemainTime;

	Global::SendToCli_ByDBID( PlayerDBID , sizeof( Send ) , &Send );
}
void CNetDC_Guild::SL_GuildFightAuctionInfo	   ( int SrvSockID , int CrystalID , int OwnerGuildID , int BitGuildID )		
{
	PG_Guild_DtoL_GuildFightAuctionInfo	 Send;
	Send.CrystalID = CrystalID;			//水晶號碼
	Send.OwnerGuildID = OwnerGuildID;		//目前擁有的公會
	Send.BitGuildID = BitGuildID;			//目前得標的公會

	Global::SendToSrvBySockID( SrvSockID ,  sizeof( Send ) , &Send );
}
//------------------------------------------------------------------------------------------------------------------
//換會長
void CNetDC_Guild::SC_GuildLeaderChange		( int PlayerDBID , int NewLeaderDBID , int OldLeaderDBID )
{
	PG_Guild_DtoC_GuildLeaderChange Send;
	Send.NewLeaderDBID = NewLeaderDBID;
	Send.OldLeaderDBID = OldLeaderDBID;

	Global::SendToCli_ByDBID( PlayerDBID , sizeof( Send ) , &Send );
}
//------------------------------------------------------------------------------------------------------------------
void CNetDC_Guild::SC_GuildMemberRankChange		( int PlayerDBID , int MemberDBID , int Rank )
{
	PG_Guild_DtoC_GuildMemberRankChange Send;
	Send.MemberDBID = MemberDBID;
	Send.Rank = Rank;

	Global::SendToCli_ByDBID( PlayerDBID , sizeof( Send ) , &Send );
}
//------------------------------------------------------------------------------------------------------------------
void CNetDC_Guild::SC_GuildFightInfo ( int iRequestPlayerDBID, int iCrystalID, int iDefenderGuildID, int iAttackerGuildID, int iHighestAmount, int iRemainTime, int iFightTime, int iOtherCrystalID, int iGuildPoint, EM_GuildFightInfo_Result	emResult  )
{
	PG_Guild_DtoC_GuildFightInfo Send;

	Send.emResult			= emResult;
	Send.iCrystalID			= iCrystalID;
	Send.iDefenderGuildID	= iDefenderGuildID;
	Send.iAttackerGuildID	= iAttackerGuildID;
	Send.iHighestAmount		= iHighestAmount;
	Send.iRemainTime		= iRemainTime;
	Send.iFightTime			= iFightTime;
	Send.iOtherCrystalID	= iOtherCrystalID;
	Send.iGuildPoint		= iGuildPoint;

	Global::SendToCli_ByDBID( iRequestPlayerDBID , sizeof( Send ) , &Send );
}
//------------------------------------------------------------------------------------------------------------------
void CNetDC_Guild::SC_GuildFightAuctionEnd( int PlayerDBID , int CrystalID , int iDefenderGuildID , int iAttackerGuildID , int iFightTime, int iHighestAmount )
{
	PG_Guild_DtoC_GuildFightAuctionEnd	Send;

	Send.iCrystalID			= CrystalID;
	Send.iDefenderGuildID	= iDefenderGuildID;
	Send.iAttackerGuildID	= iAttackerGuildID;
	Send.iFightTime			= iFightTime;
	Send.iHighestAmount		= iHighestAmount;

	Global::SendToCli_ByDBID( PlayerDBID , sizeof( Send ) , &Send );
}
//------------------------------------------------------------------------------------------------------------------
void CNetDC_Guild::SC_GuildFightBeginNotify ( int iPlayerDBID, int iCrystalID, int iRemainTime )
{
	PG_Guild_DtoC_GuildFightBeginNotify Send;

	Send.iCrystalID		= iCrystalID;
	Send.iRemainTime	= iRemainTime;

	Global::SendToCli_ByDBID( iPlayerDBID , sizeof( Send ) , &Send );
}
//------------------------------------------------------------------------------------------------------------------
void CNetDC_Guild::SL_CreateGuildFightRoom( int iSrvID, int CrystalID, int iDefenderGuildID, int iAttackerGuildID )
{
	PG_Guild_DtoL_CreateGuildFightRoom Send;

}
//////////////////////////////////////////////////////////////////////////
//公會自由PK
//////////////////////////////////////////////////////////////////////////
void CNetDC_Guild::SL_DeclareWarResult			( int ServerID , int Type , int PlayerDBID , int GuildID , int TargetGuildID , DeclareWarResultENUM Result )
{
	PG_Guild_DtoL_DeclareWarResult Send;
	Send.Type			= Type;				//宣戰類型
	Send.PlayerDBID		= PlayerDBID;
	Send.GuildID		= GuildID;
	Send.TargetGuildID	= TargetGuildID;
	Send.Result			= Result;

	Global::SendToSrvBySockID( ServerID ,  sizeof( Send ) , &Send );
}
void CNetDC_Guild::SL_WarInfo					( int LocalServerID , bool IsDeclareWar , int GuildID1 , int GuildID2 , int Time )
{
	PG_Guild_DtoL_WarInfo Send;
	Send.IsDeclareWar = IsDeclareWar;
	Send.GuildID[0]   = GuildID1;
	Send.GuildID[1]   = GuildID2;
	Send.Time		  = Time;
	Global::SendToLocal( LocalServerID ,  sizeof( Send ) , &Send );
}
void CNetDC_Guild::SL_All_WarInfo				( bool IsDeclareWar , int GuildID1 , int GuildID2 , int Time )
{
	PG_Guild_DtoL_WarInfo Send;
	Send.IsDeclareWar = IsDeclareWar;
	Send.GuildID[0]   = GuildID1;
	Send.GuildID[1]   = GuildID2;
	Send.Time		  = Time;
	Global::SendToAllLocal( sizeof( Send ) , &Send );
}
//////////////////////////////////////////////////////////////////////////
void CNetDC_Guild::SC_All_DeclareWarFinalResult( int Guild1 , int Guild2 , int Score1 , int Score2 , int KillCount1 , int KillCount2 )
{
	PG_Guild_DtoC_DeclareWarFinalResult Send;
	Send.GuildID[0] = Guild1;
	Send.GuildID[1] = Guild2;
	Send.Score[0] = Score1;
	Send.Score[1] = Score2;
	Send.KillCount[0] = KillCount1;
	Send.KillCount[1] = KillCount2;
	Global::SendToAllPlayer( sizeof( Send ) , &Send );
}

void CNetDC_Guild::SC_SetGuildFlagInfoResult	( int PlayerDBID , SetGuildFlagInfoResultENUM Result )
{
	PG_Guild_XtoC_SetGuildFlagInfoResult Send;
	Send.Result = Result;
	Global::SendToCli_ByDBID( PlayerDBID , sizeof( Send ) , &Send );
}

void CNetDC_Guild::SC_ALL_GuildBaseInfo		( GuildBaseStruct& Guild )
{
	PG_Guild_LtoC_GuildBaseInfo Send;
	Send.GuildName = Guild.GuildName;
	Send.GuildID = Guild.GuildID;
	Send.IsReady = Guild.IsReady;
	Send.Flag = Guild.GuildFlag;
	Global::SendToAllPlayer( sizeof( Send ) , &Send );
}

void CNetDC_Guild::SC_ChangeGuildNameResult		( int PlayerDBID , ChangeGuildNameResultENUM Result )
{
	PG_Guild_DtoC_ChangeGuildNameResult Send;
	Send.Result = Result;
	Global::SendToCli_ByDBID( PlayerDBID , sizeof( Send ) , &Send );
}
void CNetDC_Guild::SC_ALL_ChangeGuildName		( int GuildID , const char* GuildName )
{
	PG_Guild_DtoC_ChangeGuildName Send;
	Send.GuildID = GuildID;
	Send.GuildName = GuildName;
	Global::SendToAllPlayer( sizeof( Send ) , &Send );
}
void CNetDC_Guild::SL_AddGuildResourceResult( int ServerID , int GuildID , int	PlayerDBID , GuildResourceStruct& Resource ,const char* Ret_Runplot , bool Result )
{
	PG_Guild_DtoL_AddGuildResourceResult Send;
	Send.GuildID = GuildID;
	Send.PlayerDBID = PlayerDBID;
	Send.Resource = Resource;
	Send.Ret_Runplot = Ret_Runplot;	//回傳後值行的劇情   bool Runplot( Result )
	Send.Result = Result;
	Global::SendToSrvBySockID( ServerID ,  sizeof( Send ) , &Send );
}

void CNetDC_Guild::SC_SelectContributionLogResult( int PlayerDBID , vector<ContributionInfoLogStruct> LogList )
{
	PG_Guild_DtoC_SelectContributionLogResult Send;
	Send.Count = (int)LogList.size();
	if( Send.Count > 500 )
		Send.Count = 500;

	for( int i = 0 ; i < Send.Count ; i++ )
		Send.Log[i] = LogList[i];
	
	Global::SendToCli_ByDBID( PlayerDBID , Send.Size() , &Send );
}

void CNetDC_Guild::SC_GuildUpgradeResult( int PlayerDBID , int GuildLv , bool Result )
{
	PG_Guild_DtoC_GuildUpgradeResult Send;
	Send.GuildLv = GuildLv;
	Send.Result = Result;
	Global::SendToCli_ByDBID( PlayerDBID , sizeof( Send ) , &Send );
}

void CNetDC_Guild::SC_AllMember_Guild_LevelUp		( int GuildID , int Level )
{
	PG_Guild_DtoC_Guild_LevelUp Send;
	Send.GuildID = GuildID;
	Send.Level	 = Level;

	GuildStruct* Guild = GuildManageClass::This()->GetGuildInfo( GuildID	);
	if( Guild == NULL )
		return;

	for( int i = 0 ; i < (int)Guild->Member.size() ; i++ )
	{
		GuildMemberStruct* Member = Guild->Member[i];
		if( Member->IsOnline == false )
			continue;
		Global::SendToCli_ByDBID( Member->MemberDBID ,  sizeof( Send ) , &Send );
	}
	//Global::SendToCli_ByDBID( PlayerDBID , sizeof( Send ) , &Send );
}

void CNetDC_Guild::SC_GuildInfoChange		( int GuildID , GuildInfoChangeStruct ChangeMode )
{
	PG_Guild_DtoC_GuildInfoChange Send;
	Send.ChangeMode = ChangeMode;

	GuildStruct* Guild = GuildManageClass::This()->GetGuildInfo( GuildID	);
	if( Guild == NULL )
		return;

	for( int i = 0 ; i < (int)Guild->Member.size() ; i++ )
	{
		GuildMemberStruct* Member = Guild->Member[i];
		if( Member->IsOnline == false )
			continue;
		Global::SendToCli_ByDBID( Member->MemberDBID ,  sizeof( Send ) , &Send );
	}
}
//要求公會戰歷史資料
void CNetDC_Guild::SC_GuildWarHistroyResult( int PlayerDBID , deque< GuildHouseWarHistoryStruct > log )
{
	PG_Guild_DtoC_GuildWarHistroyResult send;
	send.Count = log.size();
	if( send.Count > 50 )
		send.Count = 50;
	for( int i = 0 ; i < send.Count ; i++ )
	{
		send.Log[i] = log[i];
	}
	Global::SendToCli_ByDBID( PlayerDBID ,  send.PGSize() , &send );
}
