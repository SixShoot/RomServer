	
#pragma once
#include "PG/PG_Guild.h"
#include "../../MainProc/Global.h"

enum EM_StructCrystal
{
	EM_StructCrystal_WaitBidStart	= 0,	// �}�l�v�Ыe, �W�g���|�Ե���
	EM_StructCrystal_WaitBidEnd		= 1,
	EM_StructCrystal_WaitFight		= 2,
	EM_StructCrystal_Fighting		= 3,
	//EM_StructCrystal_
};

struct StructCrystal
{
	StructCrystal()
	{
		iDefender			= 0;
		iAttacker			= 0;
		iDefenderGuildID	= 0;
		iAttackerGuildID	= 0;
		iBidAmount			= 0;
		iAdditionEndTime	= 0;
		iStartTime			= 0;
		iEndTime			= 0;
		iFightTime			= 0;
		iFightServerID		= 0;
	};

	int					iDefender;
	int					iAttacker;
	int					iDefenderGuildID;
	int					iAttackerGuildID;
	int					iBidAmount;
	int					iStartTime;
	int					iEndTime;
	int					iAdditionEndTime;
	int					iFightTime;
	int					iFightServerID;
	EM_StructCrystal	emStatus;
};


class CNetDC_Guild : public Global
{
protected:
	//-------------------------------------------------------------------
	static CNetDC_Guild*	This;

	static bool				_Init();
	static bool				_Release();

	static void _OnLocalSrvConnectEvent ( EM_SERVER_TYPE ServerType, DWORD SrvID );

	static void _PG_Guild_LtoD_CreateGuild					( int ServerID , int Size , void* Data );	
	static void _PG_Guild_LtoD_AddNewGuildMember			( int ServerID , int Size , void* Data );	
	static void _PG_Guild_LtoD_LeaveNewGuild				( int ServerID , int Size , void* Data );
	static void _PG_Guild_LtoD_DelNewGuild					( int ServerID , int Size , void* Data );

	static void _PG_Guild_LtoD_AddGuildMember				( int ServerID , int Size , void* Data );	
	static void _PG_Guild_LtoD_LeaveGuild					( int ServerID , int Size , void* Data );
	static void _PG_Guild_LtoD_DelGuild						( int ServerID , int Size , void* Data );

	static void _PG_Guild_LtoD_ModifyGuildInfoRequest		( int ServerID , int Size , void* Data );
	static void _PG_Guild_LtoD_ModifyGuildMemberInfoRequest	( int ServerID , int Size , void* Data );

	static void _PG_Guild_LtoD_SetGuildReady				( int ServerID , int Size , void* Data );

	static void _PG_Guild_LtoD_BoardPostRequest				( int ServerID , int Size , void* Data );
	static void _PG_Guild_LtoD_BoardListRequest				( int ServerID , int Size , void* Data );
	static void _PG_Guild_LtoD_BoardMessageRequest			( int ServerID , int Size , void* Data );
	static void _PG_Guild_LtoD_BoardModifyMessageRequest	( int ServerID , int Size , void* Data );
	static void _PG_Guild_LtoD_BoardModifyModeRequest		( int ServerID , int Size , void* Data );

//	static void _PG_Guild_LtoD_BuyFunction					( int ServerID , int Size , void* Data );
	static void _PG_Guild_LtoD_AddGuildResource				( int ServerID , int Size , void* Data );

//	static void _PG_Guild_LtoD_GuildShopBuy					( int ServerID , int Size , void* Data );

	static void _PG_Guild_LtoD_GuildFightAuctionBid			( int ServerID , int Size , void* Data );

	static void _PG_Guild_LtoD_GuildFightInfoRequest		( int ServerID , int Size , void* Data );

	static void _PG_Guild_LtoD_DeclareWar					( int ServerID , int Size , void* Data );
	static void _PG_Guild_LtoD_DeclareWarScore				( int ServerID , int Size , void* Data );

	static void _PG_Guild_LtoD_SetGuildFlagInfoRequest		( int ServerID , int Size , void* Data );

	static void _PG_Guild_LtoD_ChangeGuildNameRequest		( int ServerID , int Size , void* Data );

	static void _PG_Guild_LtoD_GuildUpgradeRequest			( int ServerID , int Size , void* Data );
	static void _PG_Guild_LtoD_SelectContributionLog		( int ServerID , int Size , void* Data );
	static void _PG_Guild_LtoD_SetNeedChangeName			( int ServerID , int Size , void* Data );

	static void _PG_Guild_LtoD_GuildWarEndResult			( int ServerID , int Size , void* Data );
	static void _PG_Guild_LtoD_GuildWarHistroy				( int ServerID , int Size , void* Data );

public:
	//--------------------------------------------------------------------------------------------------------------------
	//���|
	//--------------------------------------------------------------------------------------------------------------------
	static void SL_CreateGuildResult		( int SrvSockID , int PlayerDBID , char* GuildName , int GuildID , CreateGuildResultENUM Result );
	static void SL_AddNewGuildMemberResult	( int SrvSockID , char* LeaderName , int LeaderDBID , int GuildID , char* NewMember , bool Result );
	static void SL_LeaveNewGuildResult		( int SrvSockID , int LeaderDBID , char* PlayerName , bool Result );
	static void SL_DelNewGuildResult		( int SrvSockID , int LeaderDBID , bool Result );

	static void SL_AddGuildMemberResult		( int SrvSockID , char* LeaderName , int LeaderDBID , int GuildID , char* NewMember , bool Result );
	static void SL_LeaveGuildResult			( int SrvSockID , int LeaderDBID , char* PlayerName , bool Result );
	static void SL_DelGuildResult			( int SrvSockID , int LeaderDBID , bool Result );

	//static void SC_ALLMember_GuildCreate	( int GuildID );
	static void SC_ALLPlayer_GuildCreate	( int GuildID );

	static void SC_ALLMember_PlayerOnline	( int PlayerDBID , bool IsOnline );

	//-----------------------------------------------------------------------------------
	//�s�W�β������|����
	//�q���Ҧ� Server
	static void SL_All_AddGuild			( GuildBaseStruct& Guild  );
	static void SL_AddGuild				( int LocalID , GuildBaseStruct& Guild );
	static void SL_All_DelGuild			( int Guild );
	static void SL_All_AddGuildMember	( GuildMemberStruct& Member );
	static void SL_AddGuildMember		( int LocalID , GuildMemberStruct& Member );
	static void SL_All_DelGuildMember	( int Guild , char* MemberName );
	static void SL_All_ModifyGuild		( GuildBaseStruct& Guild );
	static void SL_All_ModifyGuildMember( GuildMemberStruct& Member );
	
	static void SL_All_ModifySimpleGuild( GuildBaseStruct& Guild );
	static void SC_AllMember_ModifySimpleGuild( GuildBaseStruct& Guild );
	static void SC_AllMember_GuildBaseInfoUpdate( GuildBaseStruct& Guild );
	//--------------------------------------------------------------------------------------------------------------------
	//�d���O
	//--------------------------------------------------------------------------------------------------------------------
	static void SC_BoardPostResult			( int PlayerDBID , bool Result );
	static void SC_BoardListCount			( int PlayerDBID , int MaxCount , int Count , int PageID );
	static void SC_BoardListData			( int PlayerDBID , int Index , GuildBoardStruct& Message );
	static void SC_BoardMessageResult		( int PlayerDBID , GuildBoardStruct& Message );
	static void SC_BoardModifyMessageResult	( int PlayerDBID , bool Result );
	static void SC_BoardModifyModeResult	( int PlayerDBID , bool Result );
	//--------------------------------------------------------------------------------------------------------------------
	//�q�����a�s�W�������|����
	//--------------------------------------------------------------------------------------------------------------------
	static void SC_JoinGuildMember			( int PlayerDBID , GuildMemberStruct& Member );
	static void SC_LeaveGuildMember			( int PlayerDBID , int LeaderDBID , int MemberDBID );

	//--------------------------------------------------------------------------------------------------------------------
	//�q��Client �ʶR���G
//	static void SC_GuildShopBuyResult		( int PlayerDBID , int FlagID , bool Result );


	//--------------------------------------------------------------------------------------------------------------------
	//���|��
	static void SC_GuildLeaderChange		( int PlayerDBID , int NewLeaderDBID , int OldLeaderDBID );
	static void SC_GuildMemberRankChange	( int PlayerDBID , int MemberDBID , int Rank );


	static void SC_ALL_GuildBaseInfo		( GuildBaseStruct& Guild );

	//--------------------------------------------------------------------------------------------------------------------
	//���|
	//--------------------------------------------------------------------------------------------------------------------

	virtual void RL_CreateGuild			( int SrvSockID , int PlayerDBID , char* GuildName ) = 0;
	virtual void RL_AddNewGuildMember	( int SrvSockID , int GuildID , char* LeaderName , int LeaderDBID , char* NewMember , int NewMemberDBID ) = 0;
	virtual void RL_LeaveNewGuild		( int SrvSockID , int LeaderDBID , char* PlayerName ) = 0;
	virtual void RL_DelNewGuild			( int SrvSockID , int LeaderDBID ) = 0;

	virtual void RL_AddGuildMember		( int SrvSockID , int GuildID , char* MemberName , int MemberDBID , char* NewMember , int NewMemberDBID ) = 0;
	virtual void RL_LeaveGuild			( int SrvSockID , int LeaderDBID , char* PlayerName ) = 0;
	virtual void RL_DelGuild			( int SrvSockID , int LeaderDBID ) = 0;

	virtual void RL_ModifyGuildInfoRequest		( int SrvSockID , int PlayerDBID , GuildBaseStruct& Guild ) = 0;
	virtual void RL_ModifyGuildMemberInfoRequest( int SrvSockID , int PlayerDBID , GuildMemberStruct& Member) = 0;

	virtual	void RL_SetGuildReady		( int GuildID ) = 0;

	virtual void OnLocalSrvConnect( int SrvID ) = 0;

	//--------------------------------------------------------------------------------------------------------------------
	//�d���O
	//--------------------------------------------------------------------------------------------------------------------
	virtual void RL_BoardPostRequest			( int PlayerDBID , GuildBoardStruct& Message ) = 0;
	virtual void RL_BoardListRequest			( int PlayerDBID , int PageID ) = 0;
	virtual void RL_BoardMessageRequest			( int PlayerDBID , int MessageGUID ) = 0;
	virtual void RL_BoardModifyMessageRequest	( int PlayerDBID , int MessageGUID , char* MessageStr ) = 0;
	virtual void RL_BoardModifyModeRequest		( int PlayerDBID , int MessageGUID , GuildBoardModeStruct	Mode ) = 0;

	//--------------------------------------------------------------------------------------------------------------------
	//���|���v��
	//--------------------------------------------------------------------------------------------------------------------
	static void SC_GuildFightAuctionBidResult( int PlayerDBID , int CrystalID , int OwnerGuildID , int BitGuildID , int iHighestAmount, int Result );
		// �q���ثe�U�Ъ����|�|����
	static void SC_GuildFightAuctionBidNewOne( int PlayerDBID , int CrystalID , int OwnerGuildID , int BitGuildID, int iHighestAmount, int iRemainTime );		
		// �q���Q�W�L�v�������|�|����, ���s���X��

	static void SC_GuildFightAuctionEnd		( int PlayerDBID , int CrystalID , int iDefenderGuildID , int iAttackerGuildID , int iFightTime, int iHighestAmount );
		// �q�����ê̤��|�Χ�����, ��浲��

	static void SL_GuildFightAuctionInfo		( int SrvSockID , int CrystalID , int OwnerGuildID , int BitGuildID  );		
	static void SC_GuildFightInfo				( int iRequestPlayerDBID, int iCrystalID, int iDefenderGuildID, int iAttackerGuildID, int iHighestAmount, int iRemainTime, int iFightTime, int iOtherCrystalID, int iGuildPoint, EM_GuildFightInfo_Result	emResult  );

	virtual void RL_GuildFightAuctionBid		( int LeaderDBID , int CrystalID , int BidScore ) = 0;
	virtual void RL_GuildFightInfoRequest		( int iRequestPlayerDBID, int iCrystalID ) = 0;

	static void	SC_GuildFightBeginNotify		( int iPlayerDBID, int iCrystalID, int iRemainTime );
	static void SL_CreateGuildFightRoom			( int iSrvID, int CrystalID, int iDefenderGuildID, int iAttackerGuildID );

	//////////////////////////////////////////////////////////////////////////
	//���|�ۥ�PK
	//////////////////////////////////////////////////////////////////////////
	virtual void RL_DeclareWar				( int ServerID , int Type , int PlayerDBID , int TargetGuildID ) = 0;
	virtual void RL_DeclareWarScore			( int GuildID[2] , int Score , int KillCount ) = 0;

	static void SL_DeclareWarResult			( int ServerID , int Type , int PlayerDBID , int GuildID , int TargetGuildID , DeclareWarResultENUM Result );
	static void SL_WarInfo					( int LocalServerID , bool IsDeclareWar , int GuildID1 , int GuildID2 , int Time );
	static void SL_All_WarInfo				( bool IsDeclareWar , int GuildID1 , int GuildID2 , int Time );
	static void SC_All_DeclareWarFinalResult( int Guild1 , int Guild2 , int Score1 , int Score2 , int KillCount1 , int KillCount2 );

	static void SC_ALLMember_PrepareWar		( int SendToGuildID , int GuildID ,	int Time ); 

	//////////////////////////////////////////////////////////////////////////
	//���|�X�]�w
	//////////////////////////////////////////////////////////////////////////
	virtual void RL_SetGuildFlagInfoRequest	( int PlayerDBID , GuildFlagStruct& Flag ) = 0;

	static void SC_SetGuildFlagInfoResult	( int PlayerDBID , SetGuildFlagInfoResultENUM Result );

	//////////////////////////////////////////////////////////////////////////
	//���|��W
	//////////////////////////////////////////////////////////////////////////
	virtual void RL_ChangeGuildNameRequest	( int GuildID , int PlayerDBID , char* GuildName ) = 0;
	static void SC_ChangeGuildNameResult	( int PlayerDBID , ChangeGuildNameResultENUM Result );
	static void SC_ALL_ChangeGuildName		( int GuildID , const char* GuildName  );

	virtual void RL_SetNeedChangeName		( int GuildID ) = 0;
	//////////////////////////////////////////////////////////////////////////
	//���|�ɯ�
	//////////////////////////////////////////////////////////////////////////
	virtual void RL_GuildUpgradeRequest		( int GuildID , int PlayerDBID , int GuildLv ) = 0;
	static void SC_GuildUpgradeResult		( int PlayerDBID , int GuildLv , bool Result );
	static void SC_AllMember_Guild_LevelUp	( int GuildID , int Level );
	//////////////////////////////////////////////////////////////////////////
	//���|���m
	//////////////////////////////////////////////////////////////////////////
	virtual void RL_AddGuildResource	( int ServerID , int GuildID , int PlayerDBID , GuildResourceStruct& Resource , const char* Ret_Runplot , AddGuildResourceTypeENUM LogType , int LogType_ObjID ) = 0;
	static void SL_AddGuildResourceResult( int ServerID , int GuildID , int	PlayerDBID , GuildResourceStruct& Resource ,const char* Ret_Runplot , bool Result );
	virtual void RL_SelectContributionLog( int PlayerDBID , int DayBefore ) = 0;
	static void SC_SelectContributionLogResult( int PlayerDBID , vector<ContributionInfoLogStruct> LogList );

	static void SC_GuildInfoChange		( int GuildID , GuildInfoChangeStruct ChangeMode );
	//////////////////////////////////////////////////////////////////////////
	virtual void RL_GuildWarEndResult( int GuildID , GameResultENUM GameResult , int GuildScore , GuildWarBaseInfoStruct& TargetGuild ) = 0;

	//////////////////////////////////////////////////////////////////////////
	//�n�D���|�Ծ��v���
	virtual void RL_GuildWarHistroy( int ServerI , int playerDBID ) = 0;
	//�n�D���|�Ծ��v���
	static void SC_GuildWarHistroyResult( int PlayerDBID , deque< GuildHouseWarHistoryStruct > log );
};
