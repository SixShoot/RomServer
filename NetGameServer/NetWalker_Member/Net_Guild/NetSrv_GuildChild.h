#pragma once

#include "NetSrv_Guild.h"
#include "../../mainproc/GuildManage/GuildManage.h"

enum EM_GuildFightRoomType
{
	EM_GuildFightRoomType_GuildFight	= 0,	// 水晶競標爭奪
	EM_GuildFightRoomType_GMEvent		= 1,	// GM 邀請
};

struct StructGuildFightRoom
{
	StructGuildFightRoom()
	{
		CrystalID			= 0;
		iDefenderGuildID	= 0;
		iAttackerGuildID	= 0;
		emType				= EM_GuildFightRoomType_GuildFight;
	}

	int						CrystalID;
	int						iDefenderGuildID;
	int						iAttackerGuildID;

	EM_GuildFightRoomType	emType;
};

class NetSrv_GuildChild : public NetSrv_Guild
{
public:
    static bool Init();
    static bool Release();

	virtual void  RC_CreateGuild			( BaseItemObject* Sender , char* GuildName );
	virtual void  RC_NewGuildInvite			( BaseItemObject* Sender , char* PlayerName );
	virtual void  RC_NewGuildInviteResult	( BaseItemObject* Sender , int GuildID , int LeaderDBID , bool Result );
	virtual void  RC_LeaveNewGuild			( BaseItemObject* Sender , char* KickPlayerName);
	virtual void  RC_DelNewGuild			( BaseItemObject* Sender );

	virtual void  RD_CreateGuildResult		( int PlayerDBID , char* GuildName , int GuildID , CreateGuildResultENUM Result );
	virtual void  RD_AddNewGuildMemberResult( int LeaderDBID , char* LeaderName , int GuildID , char* NewMemberName , bool Result );
	virtual void  RD_LeaveNewGuildResult	( int LeaderDBID , char* PlayerName , bool Result );
	virtual void  RD_DelNewGuildResult		( int LeaderDBID , bool Result);

	virtual void  RC_GuildInvite			( BaseItemObject* Sender , char* PlayerName );
	virtual void  RC_GuildInviteResult		( BaseItemObject* Sender , int GuildID , int LeaderDBID , bool Result );
	virtual void  RC_LeaveGuild				( BaseItemObject* Sender , char* KickPlayerName);
	virtual void  RC_DelGuild				( BaseItemObject* Sender );	

	virtual void  RD_AddGuildMemberResult	( int LeaderDBID , char* LeaderName , int GuildID , char* NewMemberName , bool Result );
	virtual void  RD_LeaveGuildResult		( int LeaderDBID , char* PlayerName , bool Result );	
	virtual void  RD_DelGuildResult			( int LeaderDBID , bool Result);

	virtual void RD_AddGuild( GuildBaseStruct& Guild );
	virtual void RD_DelGuild( int GuildID );		
	virtual void RD_AddGuildMember( GuildMemberStruct& Member );
	virtual void RD_DelGuildMember( int GuildID , char* MemberName );
	virtual void OnDataCenterDisconnect( );

	virtual void  RD_ModifyGuild					( GuildBaseStruct&	Guild );
	virtual void  RD_ModifyGuildMember				( GuildMemberStruct&	Member );	
	virtual void  RD_ModifySimpleGuild				( SimpleGuildInfoStruct& SimpleGuildInfo );

	virtual void  RC_GuildInfoRequet				( BaseItemObject* Sender , int GuildID );
	virtual void  RC_GuildMemberListRequet			( BaseItemObject* Sender , int GuildID );	
	virtual void  RC_GuildMemberInfoRequet			( BaseItemObject* Sender , int MemberDBID );	
	virtual void  RC_ModifyGuildInfoRequest			( BaseItemObject* Sender , GuildBaseStruct&	Guild );	
	virtual void  RC_ModifyGuildMemberInfoRequest	( BaseItemObject* Sender , GuildMemberStruct&	Member );

	virtual void  RC_AllGuildMemberCountRequest		( BaseItemObject* Sender );
	//----------------------------------------------------------------------------------------------
	//留言板
	//----------------------------------------------------------------------------------------------
	virtual void  RC_BoardPostRequest			( BaseItemObject* SenderObj , GuildBoardStruct& Message );
	virtual void  RC_BoardListRequest			( BaseItemObject* SenderObj , int PageID );
	virtual void  RC_BoardMessageRequest		( BaseItemObject* SenderObj , int MessageGUID );
	virtual void  RC_BoardModifyMessageRequest	( BaseItemObject* SenderObj , int MessageGUID , char* Message );
	virtual void  RC_BoardModifyModeRequest		( BaseItemObject* SenderObj , int MessageGUID , GuildBoardModeStruct Mode );
	//----------------------------------------------------------------------------------------------------------
	virtual void  RC_CloseCreateGuild			( BaseItemObject* SenderObj );
	virtual void  RC_GuildContributionItem		( BaseItemObject* SenderObj , int Pos , ItemFieldStruct& Item , bool IsPut );
	virtual void  RC_GuildContributionOK		( BaseItemObject* SenderObj , int Money , int Bonus_Money );
	virtual void  RC_GuildContributionClose		( BaseItemObject* SenderObj );
	//----------------------------------------------------------------------------------------------------------
	//公會商店
	//----------------------------------------------------------------------------------------------------------
//	virtual void  RC_GuildShopBuy				( BaseItemObject* SenderObj , int FlagID );
//	virtual void  RC_GuildShopClose				( BaseItemObject* SenderObj );
	//----------------------------------------------------------------------------------------------------------
	//公會競標
	//----------------------------------------------------------------------------------------------------------
	virtual void  RC_GuildFightAuctionClose		( BaseItemObject* SenderObj );
	virtual void  RC_GuildFightAuctionBid		( BaseItemObject* SenderObj, int CrystalID, int BitScore );	
	virtual void  RD_GuildFightAuctionInfo		( int CrystalID , int OwnerGuildID , int BitGuildID );
	virtual	void  RD_CreateGuildFightRoom		( int CrystalID , int iDefenderGuildID , int iAttackerGuildID );

	virtual	void  RC_GuildFightRequestInvite	( PG_Guild_CtoL_GuildFightRequestInvite* pPacket );
	virtual	void  RC_GuildFightInviteRespond	( PG_Guild_CtoL_GuildFightInviteRespond* pPacket );

	
	//送所有的公會基本資料給Client
	static void SendAllGuildBaseInfo( int SendID );



	static bool CheckIsEnabledGuildBoard	( RoleDataEx* Role );

	//送公會ID改變 給周圍的完家
	static void SendRangeGuildIDChange		( RoleDataEx* Role );


	//----------------------------------------------------------------------------------------------
	//公會自由對戰
	//----------------------------------------------------------------------------------------------
	virtual void  RC_DeclareWar					( BaseItemObject* Obj , int Type , int TargetGuildID );
	virtual void  RD_DeclareWarResult			( int PlayerDBID , int GuildID  , int Type , int TargetGuildID , DeclareWarResultENUM Result );
	virtual void  RD_WarInfo					( bool IsDeclareWar , int GuildID[2] , int Time );

	//----------------------------------------------------------------------------------------------
	//公會旗設定
	//----------------------------------------------------------------------------------------------
	virtual void RC_SetGuildFlagInfoRequest		( BaseItemObject* Obj , GuildFlagStruct& Flag );

	//----------------------------------------------------------------------------------------------
	//公會改名
	//----------------------------------------------------------------------------------------------
	virtual void RC_ChangeGuildNameRequest		( BaseItemObject*  Obj , char* GuildName ) ;	

	//----------------------------------------------------------------------------------------------
	//公會升級
	//----------------------------------------------------------------------------------------------	
	virtual void RC_GuildUpgradeRequest			( BaseItemObject* Obj , int GuildLv );
	virtual void RD_AddGuildResourceResult		( int GuildID , int PlayerDBID , GuildResourceStruct& Resource , char* Ret_Runplot , bool Result );


	virtual void  RC_SelectContributionLog		( BaseItemObject* Obj , int DayBefore );

	//要求公會戰歷史資料
	virtual void RC_GuildWarHistroy				( BaseItemObject* obj  );
	//----------------------------------------------------------------------------------------------------------
	// 公會戰房間管理
	//----------------------------------------------------------------------------------------------------------
	static map< int, StructGuildFightRoom >			m_mapGuildFightRoom;
};

