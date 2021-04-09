#pragma once

#include "NetDC_Guild.h"

//--------------------------------------------------------------------------------------------
class CNetDC_GuildChild : public CNetDC_Guild
{
protected:
	
	static int  _OnTimeProc( SchedularInfo* Obj , void* InputClass );
	static int  _OnGuildFightBidTimeProc( SchedularInfo* Obj , void* InputClass );

	//初始載入
	static bool _SQLCmdInitProc( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdInitProcResult ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	//更新儲存
	static bool _SQLCmdUpDateProc( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdUpDateProcResult ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	//公會新增
//	static bool _SQLCmdNewGuildProc( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
//	static void _SQLCmdNewGuildProcResult ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );
	static bool _SQLCmdCreateGuildProc( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdCreateGuildProcResult ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	//公會成員新增
	static bool _SQLCmdNewGuildMemberProc( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdNewGuildMemberProcResult ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	//公會刪除
	static bool _SQLCmdDelGuildProc( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdDelGuildProcResult ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	//公會成員刪除
	static bool _SQLCmdDelGuildMemberProc( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdDelGuildMemberProcResult ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	//////////////////////////////////////////////////////////////////////////
	//公會訊息新增
	static bool _SQLCmdNewMessageProc( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdNewMessageProcResult ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	//公會訊息修改
	static bool _SQLCmdUpdateMessageProc( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdUpdateMessageProcResult ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	//公會訊息刪除
	static bool _SQLCmdDelMessageProc( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdDelMessageProcResult ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	//更新公會名稱
	static bool _SQLChangeGuildNameProc( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLChangeGuildNameProcResult ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	static bool _SQLCmdSelectResourceLog( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdSelectResourceLogResult ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );


	static void	_WarProc( GuildStruct* Guild );

	static void LocalSrvConnectProc( int SrvID );
protected:
	static bool	_IsLoadGuildOK;
	static vector<GuildBaseStruct>		_NeedUpDateGuild;		//須要更新的公會
	static vector<GuildMemberStruct>	_NeedUpDateMember;		//須要更新的成員
	static deque<int>					_OnConnectSrvID;
	

public:
//--------------------------------------------------------------------------------------------
	static bool Init();
	static bool Release();
//--------------------------------------------------------------------------------------------
	virtual void RL_CreateGuild( int SrvSockID , int PlayerDBID , char* GuildName );
	virtual void RL_AddNewGuildMember( int SrvSockID , int GuildID , char* LeaderName , int LeaderDBID , char* NewMember , int NewMemberDBID );
	virtual void RL_LeaveNewGuild( int SrvSockID , int LeaderDBID , char* PlayerName );
	virtual void RL_DelNewGuild  ( int SrvSockID , int LeaderDBID );
	virtual void OnLocalSrvConnect( int SrvID );

	virtual void RL_AddGuildMember		( int SrvSockID , int GuildID , char* MemberName , int MemberDBID , char* NewMember , int NewMemberDBID );
	virtual void RL_LeaveGuild			( int SrvSockID , int LeaderDBID , char* PlayerName );
	virtual void RL_DelGuild			( int SrvSockID , int LeaderDBID );

	virtual void RL_ModifyGuildInfoRequest		( int SrvSockID , int PlayerDBID , GuildBaseStruct& Guild );
	virtual void RL_ModifyGuildMemberInfoRequest( int SrvSockID , int PlayerDBID , GuildMemberStruct& Member);

	virtual	void RL_SetGuildReady		( int GuildID );
	//--------------------------------------------------------------------------------------------------------------------
	//留言板
	//--------------------------------------------------------------------------------------------------------------------
	virtual void RL_BoardPostRequest			( int PlayerDBID , GuildBoardStruct& Message );
	virtual void RL_BoardListRequest			( int PlayerDBID , int PageID );
	virtual void RL_BoardMessageRequest			( int PlayerDBID , int MessageGUID );
	virtual void RL_BoardModifyMessageRequest	( int PlayerDBID , int MessageGUID , char* MessageStr );
	virtual void RL_BoardModifyModeRequest		( int PlayerDBID , int MessageGUID , GuildBoardModeStruct	Mode );

//	virtual void RL_BuyFunction					( int ServerID , int GuildID , int PlayerDBID , int Score , int Type , int Value );
//	virtual void RL_GuildShopBuy				( int PlayerDBID , int FlagID );
	//--------------------------------------------------------------------------------------------
	//公會戰競標
	virtual void RL_GuildFightAuctionBid		( int LeaderDBID , int CrystalID , int BitScore );

	virtual void RL_GuildFightInfoRequest		( int iRequestPlayerGUID, int iCrystalID );
	//--------------------------------------------------------------------------------------------
	//資料庫儲存 新增公會
//	static void SaveNewGuild( GuildBaseStruct& Guild );
	//資料庫儲存 新增公會成員
	static void SaveNewGuildMember( GuildMemberStruct& Member );

	//資料庫儲存 刪除公會
	static void DelGuild( int GuildID );
	//資料庫儲存 刪除公會成員
	static void DelGuildMember( int PlayerDBID );

	//--------------------------------------------------------------------------------------------
	//資料庫 新增訊息
	static void SaveGuildMessage( GuildBoardStruct& Msg );

	//資料庫 刪除訊息
	static void DelGuildMessage( int GuildID , int MessageGUID  );

	//資料庫 修改訊息
	static void UpdateGuildMessage( GuildBoardStruct& Msg  );

	//--------------------------------------------------------------------------------------------------------------------
	//通知玩家新增移除公會成員
	//--------------------------------------------------------------------------------------------------------------------
	static void SendAllGuildMember_JoinGuildMember				( int GuildID , GuildMemberStruct& Member );
	static void SendAllGuildMember_LeaveGuildMember				( int GuildID , int LeaderDBID , int MemberDBID );
	static void SendAllGuildMember_ChangeLeader					( int GuildID , int NewLeaderDBID , int OldLeaderDBID );
	static void SendAllGuildMember_ChangeMemberRank				( int GuildID , int MemberDBID , int Rank );

	static void	WriteCrystalInfoToDB							( int iCrystalID, int iAttacker, int iDefender, int iBidAmount, int iAdditionTime );
	static	int	GetRemainTime									( int iStartTime, int iEndTime );
	static void	SendGuildFightBeginNotify						( int iCrystalID, int iDefenderGuildID, int iAttackerGuildID, int iRemainTime );

	static map< int, StructCrystal >	m_mapCrystal;

	//////////////////////////////////////////////////////////////////////////
	//公會自由PK
	//////////////////////////////////////////////////////////////////////////
	virtual void RL_DeclareWar				( int ServerID , int Type , int PlayerDBID , int TargetGuildID );
	virtual void RL_DeclareWarScore			( int GuildID[2] , int Score , int KillCount );

	//公會旗設定
	virtual void RL_SetGuildFlagInfoRequest( int PlayerDBID , GuildFlagStruct& Flag );

	//////////////////////////////////////////////////////////////////////////
	//公會改名
	//////////////////////////////////////////////////////////////////////////
	virtual void RL_ChangeGuildNameRequest	( int GuildID , int PlayerDBID , char* GuildName );
	virtual void RL_SetNeedChangeName		( int GuildID );
	//////////////////////////////////////////////////////////////////////////
	//公會升級
	//////////////////////////////////////////////////////////////////////////
	virtual void RL_GuildUpgradeRequest( int GuildID , int PlayerDBID , int GuildLv );

	//////////////////////////////////////////////////////////////////////////
	//公會供獻
	//////////////////////////////////////////////////////////////////////////
	virtual void RL_AddGuildResource	( int ServerID , int GuildID , int PlayerDBID , GuildResourceStruct& Resource , const char* Ret_Runplot , AddGuildResourceTypeENUM LogType , int LogType_ObjID );
	virtual void RL_SelectContributionLog( int PlayerDBID , int DayBefore );

	//儲存公會資料
	static void SaveGuildInfo();

	//////////////////////////////////////////////////////////////////////////
	virtual void RL_GuildWarEndResult( int GuildID , GameResultENUM GameResult , int GuildScore , GuildWarBaseInfoStruct& TargetGuild );

	//要求公會戰歷史資料
	virtual void RL_GuildWarHistroy( int ServerID , int playerDBID );
};
