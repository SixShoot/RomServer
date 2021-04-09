#pragma once
#include "../../MainProc/Global.h"
#include "PG/PG_Guild.h"

class NetSrv_Guild : public Global
{
protected:
    //-------------------------------------------------------------------
    static NetSrv_Guild* This;
    static bool _Init();
    static bool _Release();

	static void _PG_Guild_CtoL_CreateGuild					( int NetID , int Size , void* Data );
	static void _PG_Guild_CtoL_NewGuildInvite				( int NetID , int Size , void* Data );	
	static void _PG_Guild_CtoL_NewGuildInviteResult			( int NetID , int Size , void* Data );
	static void _PG_Guild_CtoL_LeaveNewGuild				( int NetID , int Size , void* Data );	
	static void _PG_Guild_CtoL_DelNewGuild					( int NetID , int Size , void* Data );	

	static void _PG_Guild_DtoL_CreateGuildResult			( int NetID , int Size , void* Data );	
	static void _PG_Guild_DtoL_AddNewGuildMemberResult		( int NetID , int Size , void* Data );
	static void _PG_Guild_DtoL_LeaveNewGuildResult			( int NetID , int Size , void* Data );	
	static void _PG_Guild_DtoL_DelNewGuildResult			( int NetID , int Size , void* Data );

	static void _PG_Guild_CtoL_GuildInvite					( int NetID , int Size , void* Data );	
	static void _PG_Guild_CtoL_GuildInviteResult			( int NetID , int Size , void* Data );
	static void _PG_Guild_CtoL_LeaveGuild					( int NetID , int Size , void* Data );	
	static void _PG_Guild_CtoL_DelGuild						( int NetID , int Size , void* Data );	

	static void _PG_Guild_DtoL_AddGuildMemberResult			( int NetID , int Size , void* Data );
	static void _PG_Guild_DtoL_LeaveGuildResult				( int NetID , int Size , void* Data );	
	static void _PG_Guild_DtoL_DelGuildResult				( int NetID , int Size , void* Data );


	static void _PG_Guild_DtoL_AddGuild						( int NetID , int Size , void* Data );	
	static void _PG_Guild_DtoL_DelGuild						( int NetID , int Size , void* Data );
	static void _PG_Guild_DtoL_AddGuildMember				( int NetID , int Size , void* Data );	
	static void _PG_Guild_DtoL_DelGuildMember				( int NetID , int Size , void* Data );	

	static void _PG_Guild_DtoL_ModifyGuild					( int NetID , int Size , void* Data );
	static void _PG_Guild_DtoL_ModifyGuildMember			( int NetID , int Size , void* Data );	
	static void _PG_Guild_DtoX_ModifySimpleGuild			( int NetID , int Size , void* Data );	

	static void _PG_Guild_CtoL_GuildInfoRequet				( int NetID , int Size , void* Data );
	static void _PG_Guild_CtoL_GuildMemberListRequet		( int NetID , int Size , void* Data );	
	static void _PG_Guild_CtoL_GuildMemberInfoRequet		( int NetID , int Size , void* Data );	
	static void _PG_Guild_CtoL_ModifyGuildInfoRequest		( int NetID , int Size , void* Data );	
	static void _PG_Guild_CtoL_ModifyGuildMemberInfoRequest	( int NetID , int Size , void* Data );

	static void _PG_Guild_CtoL_AllGuildMemberCountRequest	( int NetID , int Size , void* Data );
	//----------------------------------------------------------------------------------------------
	//留言板
	//----------------------------------------------------------------------------------------------
	static void _PG_Guild_CtoL_BoardPostRequest				( int NetID , int Size , void* Data );
	static void _PG_Guild_CtoL_BoardListRequest				( int NetID , int Size , void* Data );
	static void _PG_Guild_CtoL_BoardMessageRequest			( int NetID , int Size , void* Data );
	static void _PG_Guild_CtoL_BoardModifyMessageRequest	( int NetID , int Size , void* Data );
	static void _PG_Guild_CtoL_BoardModifyModeRequest		( int NetID , int Size , void* Data );

	static void _PG_Guild_CtoL_CloseCreateGuild				( int NetID , int Size , void* Data );
	static void _PG_Guild_CtoL_GuildContributionItem		( int NetID , int Size , void* Data );
	static void _PG_Guild_CtoL_GuildContributionOK			( int NetID , int Size , void* Data );
	static void _PG_Guild_CtoL_GuildContributionClose		( int NetID , int Size , void* Data );

	//----------------------------------------------------------------------------------------------
	//公會商店
	//----------------------------------------------------------------------------------------------
//	static void _PG_Guild_CtoL_GuildShopBuy					( int NetID , int Size , void* Data );	
//	static void _PG_Guild_CtoL_GuildShopClose				( int NetID , int Size , void* Data );	

	//----------------------------------------------------------------------------------------------
	//公會戰競標
	//----------------------------------------------------------------------------------------------
	static void _PG_Guild_CtoL_GuildFightAuctionClose		( int NetID , int Size , void* Data );
	static void _PG_Guild_CtoL_GuildFightAuctionBid			( int NetID , int Size , void* Data );
	static void _PG_Guild_DtoL_GuildFightAuctionInfo		( int NetID , int Size , void* Data );
	static void _PG_Guild_DtoL_CreateGuildFightRoom			( int NetID , int Size , void* Data );
	static void _PG_Guild_CtoL_GuildFightRequestInvite		( int NetID , int Size , void* Data );
	static void _PG_Guild_CtoL_GuildFightInviteRespond		( int NetID , int Size , void* Data );

	//----------------------------------------------------------------------------------------------
	//公會自由對戰
	//----------------------------------------------------------------------------------------------
	static void _PG_Guild_CtoL_DeclareWar					( int NetID , int Size , void* Data );
	static void _PG_Guild_DtoL_DeclareWarResult				( int NetID , int Size , void* Data );
	static void _PG_Guild_DtoL_WarInfo						( int NetID , int Size , void* Data );
	//----------------------------------------------------------------------------------------------
	//公會旗設定
	//----------------------------------------------------------------------------------------------
	static void _PG_Guild_CtoL_SetGuildFlagInfoRequest		( int NetID , int Size , void* Data );
	//----------------------------------------------------------------------------------------------
	//公會改名
	//----------------------------------------------------------------------------------------------
	static void _PG_Guild_CtoL_ChangeGuildNameRequest		( int NetID , int Size , void* Data );

	//----------------------------------------------------------------------------------------------
	//公會升級
	static void _PG_Guild_CtoL_GuildUpgradeRequest			( int NetID , int Size , void* Data );
	//資源增減結果
	static void _PG_Guild_DtoL_AddGuildResourceResult		( int NetID , int Size , void* Data );

	static void _PG_Guild_CtoL_SelectContributionLog		( int NetID , int Size , void* Data );
	static void _PG_Guild_CtoL_GuildWarHistroy				( int NetID , int Size , void* Data );



	static void _OnDataCenterDisconnect			( EM_SERVER_TYPE ServerType , DWORD ZoneID  );
public:    
	
	static void SC_GuildBaseInfo			( int SendID , const char* GuildName , int GuildID , GuildFlagStruct& Flag , bool IsReady );
	static void SC_All_GuildBaseInfo		( const char* GuildName , int GuildID , GuildFlagStruct& Flag , bool IsReady );
	static void SC_All_GuildDestroy			( int GuildID  );

	//公會連署	
	static void SD_CreateGuild				( int PlayerDBID , char* GuildName );
	static void SD_AddNewGuildMember		( int GuildID , char* LeaderName , int LeaderDBID , char* NewMemberName , int NewMemberDBID );
	static void SD_LeaveNewGuild			( int LeaderDBID , char* PlayerName );
	static void SD_DelNewGuild				( int LeaderDBID );

	static void SC_CreateGuildResult		( int SenderDBID , int GuildID , char* GuildName , CreateGuildResultENUM Result );	
	static void SC_NewGuildInvite			( int SockID , int ProxyID , int GuildID , char* LeaderName , int LeaderDBID );	
	static void SC_NewGuildInviteResult		( int SockID , int ProxyID , char* PlayerName , NewGuildInviteResultENUM Result );
	static void SC_JoindNewGuild			( int SockID , int ProxyID , int GuildID , char* LeaderName );	
	static void SC_LeaveNewGuildResult		( int SendToDBID , char* KickPlayerName , bool Result );
	static void SC_DelNewGuildResult		( int SendToDBID , bool Result );

	//正式公會
	static void SD_AddGuildMember			( int GuildID , char* MemberName , int MemberDBID , char* NewMemberName , int NewMemberDBID );
	static void SD_LeaveGuild				( int LeaderDBID , char* PlayerName );
	static void SD_DelGuild					( int LeaderDBID );

	static void SC_GuildInvite				( int SockID , int ProxyID , int GuildID , char* LeaderName , int LeaderDBID );	
	static void SC_GuildInviteResult		( int SockID , int ProxyID , char* PlayerName , GuildInviteResultENUM Result );
	static void SC_JoindGuild				( int SockID , int ProxyID , int GuildID , char* LeaderName );	
	static void SC_LeaveGuildResult			( int SendToDBID , char* KickPlayerName , bool Result );
	static void SC_DelGuildResult			( int SendToDBID , bool Result );

	static void SC_GuildInfo					( int SendToID , GuildBaseStruct& Guild , int MemberCount );
	static void SC_GuildMemberListCount			( int SendToID , int GuildID , int Count );
	static void SC_GuildMemberBaseInfo			( int SendToID , GuildMemberBaseInfoStruct& Info );
	static void SC_GuildMemberInfo				( int SendToId , GuildMemberStruct&	Member );
	static void SD_ModifyGuildInfoRequest		( int PlayerDBID , GuildBaseStruct& Guild );	
	static void SD_ModifyGuildMemberInfoRequest	( int PlayerDBID , GuildMemberStruct& Member );

	static void SC_AllGuildMemberCountResult	( int SendToID , vector< GuildStruct* >& GuildList );

	//設定為正式公會
	static void SD_SetGuildReady				( int GuildID );
	static void SC_SetGuildReady				( int SendToID , bool Result );

	//留言板
	static void SD_BoardPostRequest				( int PlayerDBID , GuildBoardStruct& Message );
	static void SD_BoardListRequest				( int PlayerDBID , int PageID );
	static void SD_BoardMessageRequest			( int PlayerDBID , int MessageGUID );
	static void SD_BoardModifyMessageRequest	( int PlayerDBID , int MessageGUID , char* Message );
	static void SD_BoardModifyModeRequest		( int PlayerDBID , int MessageGUID , GuildBoardModeStruct	Mode );

	//公會供獻
	static void SC_OpenCreateGuild				( int SendToID , int TargetNPCID );
	static void SC_CloseCreateGuild				( int SendToID );
	static void SC_OpenGuildContribution		( int SendToID , int TargetNPCID );
	static void SC_GuildContributionItemResult	( int SendToID , bool Result );
	static void SC_GuildContributionClose		( int SendToID );

//	static void SD_BuyFunction					( int GuildID , int PlayerDBID , int Score , int Type , int Value );
	static void SD_AddGuildResource				( int GuildID , int PlayerDBID , GuildResourceStruct& Resource , const char* RetRunplot , AddGuildResourceTypeENUM LogType , int LogType_ObjID );

	//公會商店
//	static void SC_GuildShopOpen				( int SendToID , int TargetNPCID , StaticFlag<256>& Flag , int Score );
//	static void SC_GuildShopClose				( int SendToID );
//	static void SD_GuildShopBuy					( int PlayerDBID , int FlagID );

	static void SC_GuildIDChange				( int SendToID , int GItemID , int GuildID );
	static void SD_DeclareWarScore				( int GuildID1 , int GuildID2 , int Score , int KillCount );
	//----------------------------------------------------------------------------------------------
	//公會旗設定
	//----------------------------------------------------------------------------------------------
	static void SD_SetGuildFlagInfoRequest		( int PlayerDBID , GuildFlagStruct&	Flag );
	static void SC_SetGuildFlagInfoResult		( int SendToID , SetGuildFlagInfoResultENUM Result );

	//----------------------------------------------------------------------------------------------------------------------
	virtual void  RC_CreateGuild			( BaseItemObject* Sender , char* GuildName ) = 0;
	virtual void  RC_NewGuildInvite			( BaseItemObject* Sender , char* PlayerName ) = 0;
	virtual void  RC_NewGuildInviteResult	( BaseItemObject* Sender , int GuildID , int LeaderDBID , bool Result ) = 0;
	virtual void  RC_LeaveNewGuild			( BaseItemObject* Sender , char* KickPlayerName) = 0;
	virtual void  RC_DelNewGuild			( BaseItemObject* Sender ) = 0;	
	
	virtual void  RD_CreateGuildResult		( int PlayerDBID , char* GuildName , int GuildID , CreateGuildResultENUM Result ) = 0;
	virtual void  RD_AddNewGuildMemberResult( int LeaderDBID , char* LeaderName , int GuildID , char* NewMemberName , bool Result ) = 0;
	virtual void  RD_LeaveNewGuildResult	( int LeaderDBID , char* PlayerName , bool Result ) = 0;	
	virtual void  RD_DelNewGuildResult		( int LeaderDBID , bool Result) = 0;

	virtual void  RC_GuildInvite			( BaseItemObject* Sender , char* PlayerName ) = 0;
	virtual void  RC_GuildInviteResult		( BaseItemObject* Sender , int GuildID , int LeaderDBID , bool Result ) = 0;
	virtual void  RC_LeaveGuild				( BaseItemObject* Sender , char* KickPlayerName) = 0;
	virtual void  RC_DelGuild				( BaseItemObject* Sender ) = 0;	

	virtual void  RD_AddGuildMemberResult	( int LeaderDBID , char* LeaderName , int GuildID , char* NewMemberName , bool Result ) = 0;
	virtual void  RD_LeaveGuildResult		( int LeaderDBID , char* PlayerName , bool Result ) = 0;	
	virtual void  RD_DelGuildResult			( int LeaderDBID , bool Result) = 0;

	virtual void RD_AddGuild( GuildBaseStruct& Guild ) = 0;
	virtual void RD_DelGuild( int GuildID ) = 0;		
	virtual void RD_AddGuildMember( GuildMemberStruct& Member ) =0;
	virtual void RD_DelGuildMember( int GuildID , char* MemberName ) = 0;

	virtual void OnDataCenterDisconnect( ) = 0;


	virtual void  RD_ModifyGuild			( GuildBaseStruct&	Guild ) = 0;
	virtual void  RD_ModifyGuildMember		( GuildMemberStruct&	Member ) = 0;	
	virtual void  RD_ModifySimpleGuild		( SimpleGuildInfoStruct& SimpleGuildInfo ) = 0;

	virtual void  RC_GuildInfoRequet				( BaseItemObject* Sender , int GuildID ) = 0;
	virtual void  RC_GuildMemberListRequet			( BaseItemObject* Sender , int GuildID ) = 0;	
	virtual void  RC_GuildMemberInfoRequet			( BaseItemObject* Sender , int MemberDBID ) = 0;	
	virtual void  RC_ModifyGuildInfoRequest			( BaseItemObject* Sender , GuildBaseStruct&	Guild ) = 0;	
	virtual void  RC_ModifyGuildMemberInfoRequest	( BaseItemObject* Sender , GuildMemberStruct&	Member ) = 0;

	virtual void  RC_AllGuildMemberCountRequest		( BaseItemObject* Sender ) = 0;
	//----------------------------------------------------------------------------------------------
	//留言板
	//----------------------------------------------------------------------------------------------
	virtual void  RC_BoardPostRequest			( BaseItemObject* SenderObj , GuildBoardStruct& Message ) = 0;
	virtual void  RC_BoardListRequest			( BaseItemObject* SenderObj , int PageID ) = 0;
	virtual void  RC_BoardMessageRequest		( BaseItemObject* SenderObj , int MessageGUID ) = 0;
	virtual void  RC_BoardModifyMessageRequest	( BaseItemObject* SenderObj , int MessageGUID , char* Message ) = 0;
	virtual void  RC_BoardModifyModeRequest		( BaseItemObject* SenderObj , int MessageGUID , GuildBoardModeStruct Mode ) = 0;

	//----------------------------------------------------------------------------------------------------------
	//公會商店
	//----------------------------------------------------------------------------------------------------------
//	virtual void  RC_GuildShopBuy				( BaseItemObject* SenderObj , int FlagID ) = 0;
//	virtual void  RC_GuildShopClose				( BaseItemObject* SenderObj ) = 0;
	//----------------------------------------------------------------------------------------------------------
	//公會戰競標
	//----------------------------------------------------------------------------------------------------------
	virtual void  RC_GuildFightAuctionClose		( BaseItemObject* SenderObj ) = 0;
	virtual void  RC_GuildFightAuctionBid		( BaseItemObject* SenderObj , int CrystalID, int BitScore ) = 0;	
	virtual void  RD_GuildFightAuctionInfo		( int CrystalID , int OwnerGuildID , int BitGuildID ) = 0;

	virtual	void  RD_CreateGuildFightRoom		( int CrystalID , int iDefenderGuildID , int iAttackerGuildID ) = 0;

	virtual	void  RC_GuildFightRequestInvite	( PG_Guild_CtoL_GuildFightRequestInvite* pPacket ) = 0;
	virtual	void  RC_GuildFightInviteRespond	( PG_Guild_CtoL_GuildFightInviteRespond* pPacket ) = 0;

	//----------------------------------------------------------------------------------------------------------
	//公會戰競標
	//----------------------------------------------------------------------------------------------------------
	static void SC_GuildFightAuctionOpen		( int SendToID , int TargetNPCID , int CrystalID , int OwnerGuildID , int BidGuildID , int BidScore );
	static void SC_GuildFightAuctionClose		( int SendToID );
	static void SD_GuildFightAuctionBid			( int LeaderDBID , int CrystalID , int BidScore );
	static void SD_GuildFightInfoRequest		( int iSourceOwnerDBID, int iCrystalID );
	static void SC_GuildFightLeaderInvite		( int iGuildLeaderDBID, int iSrvID, int iRoomID, int iCrystalID, int iDefenderGuildID, int iAttackerGuildID  );
	static void SC_GuildFightInviteNotify		( int iMemberDBID, int iCrystalID, int iRoomID );

	//----------------------------------------------------------------------------------------------
	//公會自由對戰
	//----------------------------------------------------------------------------------------------
	static void SD_DeclareWar					( int Type , int PlayerDBID , int TargetGuildID );
	static void SC_DeclareWarResult				( int SendToID , int Type , int TargetGuildID , DeclareWarResultENUM Result );
	static void SC_WarInfo						( int SendToID , bool IsDeclareWar , int GuildID1 , int GuildID2 , int Time , int BeginTime );
	static void SC_All_WarInfo					( bool IsDeclareWar , int GuildID1 , int GuildID2 , int Time , int BeginTime );
	static void SC_LoginWarInfo					( int SendToID , int GuildID1 , int GuildID2 , int Time , int BeginTime );

	virtual void RC_DeclareWar					( BaseItemObject* Obj , int Type , int TargetGuildID ) = 0;
	virtual void RD_DeclareWarResult			( int PlayerDBID , int GuildID  , int Type , int TargetGuildID , DeclareWarResultENUM Result ) = 0;
	virtual void RD_WarInfo						( bool IsDeclareWar , int GuildID[2] , int Time ) = 0;
	//----------------------------------------------------------------------------------------------
	//公會旗設定
	//----------------------------------------------------------------------------------------------
	virtual void RC_SetGuildFlagInfoRequest		( BaseItemObject* Obj , GuildFlagStruct& Flag ) = 0;
	//----------------------------------------------------------------------------------------------
	//公會改名
	//----------------------------------------------------------------------------------------------	
	static void SD_ChangeGuildNameRequest		( int GuildID , int PlayerDBID , const char* GuildName );
	virtual void RC_ChangeGuildNameRequest		( BaseItemObject*  Obj , char* GuildName ) = 0;	
	static void SD_SetNeedChangeName			( int GuildID ); 
	//----------------------------------------------------------------------------------------------
	//公會升級
	//----------------------------------------------------------------------------------------------	
	static void SD_GuildUpgradeRequest			( int GuildID , int GuildLv , int PlayerDBID );

	virtual void RC_GuildUpgradeRequest			( BaseItemObject* Obj , int GuildLv ) = 0;
	virtual void RD_AddGuildResourceResult		( int GuildID , int PlayerDBID , GuildResourceStruct& Resource , char* Ret_Runplot , bool Result ) = 0;

	//----------------------------------------------------------------------------------------------------------
	//公會貢獻
	//----------------------------------------------------------------------------------------------------------
	virtual void  RC_CloseCreateGuild			( BaseItemObject* SenderObj ) = 0;
	virtual void  RC_GuildContributionItem		( BaseItemObject* SenderObj , int Pos , ItemFieldStruct& Item , bool IsPut ) = 0;
	virtual void  RC_GuildContributionOK		( BaseItemObject* SenderObj , int Money , int Bonus_Money ) = 0;
	virtual void  RC_GuildContributionClose		( BaseItemObject* SenderObj ) = 0;

	virtual void  RC_SelectContributionLog		( BaseItemObject* Obj , int DayBefore ) = 0;
	static void SD_SelectContributionLog		( int DayBefore , int PlayerDBID );

	//////////////////////////////////////////////////////////////////////////
	static void SD_EndWarResult( int GuildID , GameResultENUM GameResult , int GuildScore , GuildWarBaseInfoStruct& TargetGuild );
	//////////////////////////////////////////////////////////////////////////

	//要求公會戰歷史資料
	virtual void RC_GuildWarHistroy( BaseItemObject* obj  ) = 0;
	//要求公會戰歷史資料
	static void SD_GuildWarHistroy( int playerDBID );

};

